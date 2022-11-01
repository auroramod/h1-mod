#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace dvar_cheats
{
	void apply_sv_cheats(const game::dvar_t* dvar, const game::DvarSetSource source, game::dvar_value* value)
	{
		static const auto sv_cheats_hash = game::generateHashValue("sv_cheats");

		if (dvar && dvar->hash == sv_cheats_hash)
		{
			// if dedi, do not allow internal to change value so servers can allow cheats if they want to
			if (game::environment::is_dedi() && source == game::DvarSetSource::DVAR_SOURCE_INTERNAL)
			{
				value->enabled = dvar->current.enabled;
			}

			// if sv_cheats was enabled and it changes to disabled, we need to reset all cheat dvars
			else if (dvar->current.enabled && !value->enabled)
			{
				for (auto i = 0; i < *game::dvarCount; ++i)
				{
					const auto var = &game::dvarPool[i];
					if (var && (var->flags & game::DvarFlags::DVAR_FLAG_CHEAT))
					{
						game::Dvar_Reset(var, game::DvarSetSource::DVAR_SOURCE_INTERNAL);
					}
				}
			}
		}
	}

	bool dvar_flag_checks(const game::dvar_t* dvar, const game::DvarSetSource source)
	{
		const auto info = dvars::get_dvar_info_from_hash(dvar->hash);
		const auto name = info.has_value()
			? info.value().name.data()
			: utils::string::va("0x%lX", dvar->hash);

		if ((dvar->flags & game::DvarFlags::DVAR_FLAG_WRITE))
		{
			console::error("%s is write protected\n", name);
			return false;
		}

		if ((dvar->flags & game::DvarFlags::DVAR_FLAG_READ))
		{
			console::error("%s is read only\n", name);
			return false;
		}

		// only check cheat/replicated values when the source is external
		if (source == game::DvarSetSource::DVAR_SOURCE_EXTERNAL)
		{
			const auto cl_ingame = game::Dvar_FindVar("cl_ingame");
			const auto sv_running = game::Dvar_FindVar("sv_running");

			if ((dvar->flags & game::DvarFlags::DVAR_FLAG_REPLICATED) && (cl_ingame && cl_ingame->current.enabled) && (
				sv_running && !sv_running->current.enabled))
			{
				console::error("%s can only be changed by the server\n", name);
				return false;
			}

			const auto sv_cheats = game::Dvar_FindVar("sv_cheats");
			if ((dvar->flags & game::DvarFlags::DVAR_FLAG_CHEAT) && (sv_cheats && !sv_cheats->current.enabled))
			{
				console::error("%s is cheat protected\n", name);
				return false;
			}
		}

		// pass all the flag checks, allow dvar to be changed
		return true;
	}

	void* get_dvar_flag_checks_stub()
	{
		return utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto can_set_value = a.newLabel();
			const auto zero_source = a.newLabel();

			a.pushad64();
			a.mov(r8, rdi);
			a.mov(edx, esi);
			a.mov(rcx, rbx);
			a.call_aligned(apply_sv_cheats); // check if we are setting sv_cheats
			a.popad64();
			a.cmp(esi, 0);
			a.jz(zero_source); // if the SetSource is 0 (INTERNAL) ignore flag checks

			a.pushad64();
			a.mov(edx, esi); // source
			a.mov(rcx, rbx); // dvar
			a.call_aligned(dvar_flag_checks); // protect read/write/cheat/replicated dvars
			a.cmp(al, 1);
			a.jz(can_set_value);

			// if we get here, we are non-zero source and CANNOT set values
			a.popad64(); // if I do this before the jz it won't work. for some reason the popad64 is affecting the ZR flag
			a.jmp(0x18655C_b);

			// if we get here, we are non-zero source and CAN set values
			a.bind(can_set_value);
			a.popad64(); // if I do this before the jz it won't work. for some reason the popad64 is affecting the ZR flag
			a.cmp(esi, 1);
			a.jmp(0x1861EE_b);

			// if we get here, we are zero source and ignore flags
			a.bind(zero_source);
			a.jmp(0x18628F_b);
		});
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			utils::hook::nop(0x1861D4_b, 8); // let our stub handle zero-source sets
			utils::hook::jump(0x1861DF_b, get_dvar_flag_checks_stub(), true); // check extra dvar flags when setting values

			scheduler::once([]
			{
				dvars::register_bool("sv_cheats", false, game::DvarFlags::DVAR_FLAG_REPLICATED,
				                     "Allow cheat commands and dvars on this server");
			}, scheduler::pipeline::main);
		}
	};
}

REGISTER_COMPONENT(dvar_cheats::component)
