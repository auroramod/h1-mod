#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace renderer
{
	namespace
	{
		utils::hook::detour r_init_draw_method_hook;
		utils::hook::detour r_update_front_end_dvar_options_hook;

		int get_fullbright_technique()
		{
			return game::TECHNIQUE_UNLIT;
		}

		void gfxdrawmethod()
		{
			game::gfxDrawMethod->drawScene = game::GFX_DRAW_SCENE_STANDARD;

			game::gfxDrawMethod->baseTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : game::TECHNIQUE_LIT;
			game::gfxDrawMethod->emissiveTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : game::TECHNIQUE_EMISSIVE;
			game::gfxDrawMethod->forceTechType = dvars::r_fullbright->current.enabled ? get_fullbright_technique() : 242;
		}

		void r_init_draw_method_stub()
		{
			gfxdrawmethod();
		}

		bool r_update_front_end_dvar_options_stub()
		{
			if (dvars::r_fullbright->modified)
			{
				//game::Dvar_ClearModified(dvars::r_fullbright);
				game::R_SyncRenderThread();

				gfxdrawmethod();
			}

			return r_update_front_end_dvar_options_hook.invoke<bool>();
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || !game::environment::is_mp())
			{
				return;
			}

			dvars::r_fullbright = dvars::register_int("r_fullbright", 0, 0, 3, game::DVAR_FLAG_SAVED, "Toggles rendering without lighting");

			r_init_draw_method_hook.create(SELECT_VALUE(0x1404BD140, 0x1405C46E0), &r_init_draw_method_stub);
			r_update_front_end_dvar_options_hook.create(SELECT_VALUE(0x1404F8870, 0x1405FF9E0), &r_update_front_end_dvar_options_stub);

			// use "saved" flags for "r_normalMap"
			utils::hook::set<uint8_t>(SELECT_VALUE(0x1404CF5CA, 0x1405D460E), game::DVAR_FLAG_SAVED);

			// use "saved" flags for "r_specularMap"
			utils::hook::set<uint8_t>(SELECT_VALUE(0x1404CF5F5, 0x1405D4639), game::DVAR_FLAG_SAVED);

			// use "saved" flags for "r_specOccMap"
			utils::hook::set<uint8_t>(SELECT_VALUE(0x1404CF620, 0x1405D4664), game::DVAR_FLAG_SAVED);
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(renderer::component)
#endif