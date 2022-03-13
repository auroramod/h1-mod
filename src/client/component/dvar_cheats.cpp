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

	const auto dvar_flag_checks_stub = utils::hook::assemble([](utils::hook::assembler& a)
	{
		const auto can_set_value = a.newLabel();
		const auto zero_source = a.newLabel();

		a.pushad64();
		a.mov(r8, rdi);
		a.mov(edx, esi);
		a.mov(rcx, rbx);
		a.call_aligned(apply_sv_cheats); //check if we are setting sv_cheats
		a.popad64();
		a.cmp(esi, 0);
		a.jz(zero_source); //if the SetSource is 0 (INTERNAL) ignore flag checks

		a.pushad64();
		a.mov(edx, esi); //source
		a.mov(rcx, rbx); //dvar
		a.call_aligned(dvar_flag_checks); //protect read/write/cheat/replicated dvars
		a.cmp(al, 1);
		a.jz(can_set_value);

		// if we get here, we are non-zero source and CANNOT set values
		a.popad64(); // if I do this before the jz it won't work. for some reason the popad64 is affecting the ZR flag
		a.jmp(0x1404FDCAB);

		// if we get here, we are non-zero source and CAN set values
		a.bind(can_set_value);
		a.popad64(); // if I do this before the jz it won't work. for some reason the popad64 is affecting the ZR flag
		a.cmp(esi, 1);
		a.jmp(0x1404FDA22);

		// if we get here, we are zero source and ignore flags
		a.bind(zero_source);
		a.jmp(0x1404FDA62);
	});

	void cg_set_client_dvar_from_server(const int local_client_num, void* cg, const char* dvar_id, const char* value)
	{
		const auto* dvar = game::Dvar_FindVar(dvar_id);
		if (dvar)
		{
			// If we send as string, it can't be set with source SERVERCMD because the game only allows that source on real server cmd dvars. 
			// Just use external instead as if it was being set by the console
			game::Dvar_SetFromStringByNameFromSource(dvar_id, value, game::DvarSetSource::DVAR_SOURCE_EXTERNAL);
		}
		else
		{
			// Not a dvar name, assume it is an id and the game will handle normally
			game::CG_SetClientDvarFromServer(local_client_num, cg, dvar_id, value);
		}
	}

	void set_client_dvar_by_string(const int entity_num, const char* value)
	{
		const auto* dvar = game::Scr_GetString(0); // grab the original dvar again since it's never stored on stack
		const auto* command = utils::string::va("q %s \"%s\"", dvar, value);

		game::SV_GameSendServerCommand(entity_num, game::SV_CMD_RELIABLE, command);
	}

	const auto player_cmd_set_client_dvar = utils::hook::assemble([](utils::hook::assembler& a)
	{
		const auto set_by_string = a.newLabel();

		a.pushad64();

		// check if we didn't find a network dvar index
		a.mov(ecx, dword_ptr(rsp, 0x8C8));
		a.cmp(ecx, 0);
		a.je(set_by_string);

		// we found an index, handle normally
		a.popad64();
		a.mov(r8d, ptr(rsp, 0x848));
		a.lea(r9, ptr(rsp, 0x30));
		a.jmp(0x1402E2E57);

		// no index, let's send the dvar as a string
		a.bind(set_by_string);
		a.movzx(ecx, word_ptr(rsp, 0x8C0)); //entity_num 
		a.lea(rdx, ptr(rsp, 0xB0)); //value
		a.call_aligned(set_client_dvar_by_string);
		a.popad64();
		a.jmp(0x1402E2E7D);
	});

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			utils::hook::nop(0x1404FDA0D, 4); // let our stub handle zero-source sets
			utils::hook::jump(0x1404FDA14, dvar_flag_checks_stub, true); // check extra dvar flags when setting values

			// utils::hook::nop(0x14032AACC, 5); // remove error in PlayerCmd_SetClientDvar if setting a non-network dvar
			// utils::hook::set<uint8_t>(0x14032AA9B, 0xEB);
			// don't check flags on the dvars, send any existing dvar instead
			// utils::hook::jump(0x14032AB14, player_cmd_set_client_dvar, true); // send non-network dvars as string
			// utils::hook::call(0x1401BB782, cg_set_client_dvar_from_server);
			// check for dvars being sent as string before parsing ids

			scheduler::once([]()
			{
				dvars::register_bool("sv_cheats", false, game::DvarFlags::DVAR_FLAG_REPLICATED,
				                        "Allow cheat commands and dvars on this server");
			}, scheduler::pipeline::main);
		}
	};
}

REGISTER_COMPONENT(dvar_cheats::component)
