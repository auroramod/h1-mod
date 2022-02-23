#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <component/scheduler.hpp>

namespace patches
{
	namespace
	{
		utils::hook::detour gscr_set_save_dvar_hook;
		utils::hook::detour dvar_register_float_hook;

		void gscr_set_save_dvar_stub()
		{
			const auto string = utils::string::to_lower(utils::hook::invoke<const char*>(SELECT_VALUE(0x140375210, 0x140443150), 0));
			if (string == "cg_fov" || string == "cg_fovscale")
			{
				return;
			}

			gscr_set_save_dvar_hook.invoke<void>();
		}

		game::dvar_t* cg_fov = nullptr;
		game::dvar_t* cg_fovScale = nullptr;

		game::dvar_t* dvar_register_float_stub(int hash, const char* dvarName, float value, float min, float max, unsigned int flags)
		{
			static const auto cg_fov_hash = game::generateHashValue("cg_fov");
			static const auto cg_fov_scale_hash = game::generateHashValue("cg_fovscale");

			if (hash == cg_fov_hash)
			{
				return cg_fov;
			}

			if (hash == cg_fov_scale_hash)
			{
				return cg_fovScale;
			}

			return dvar_register_float_hook.invoke<game::dvar_t*>(hash, dvarName, value, min, max, flags);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Unlock fps in main menu
			utils::hook::set<BYTE>(SELECT_VALUE(0x14018D47B, 0x14025B86B), 0xEB); // H1(1.4)

			// Fix mouse lag
			utils::hook::nop(SELECT_VALUE(0x1403E3C05, 0x1404DB1AF), 6);
			scheduler::loop([]()
			{
				SetThreadExecutionState(ES_DISPLAY_REQUIRED);
			}, scheduler::pipeline::main);

			// Prevent game from overriding cg_fov and cg_fovscale values
			gscr_set_save_dvar_hook.create(SELECT_VALUE(0x1402AE020, 0x14036B600), &gscr_set_save_dvar_stub);

			// Make cg_fov and cg_fovscale saved dvars
			cg_fov = dvars::register_float("cg_fov", 65.f, 40.f, 200.f, game::DvarFlags::DVAR_FLAG_SAVED, true);
			cg_fovScale = dvars::register_float("cg_fovScale", 1.f, 0.1f, 2.f, game::DvarFlags::DVAR_FLAG_SAVED, true);

			dvar_register_float_hook.create(game::Dvar_RegisterFloat.get(), dvar_register_float_stub);
		}
	};
}

REGISTER_COMPONENT(patches::component)
