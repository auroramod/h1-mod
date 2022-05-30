#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"

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
			switch (dvars::r_fullbright->current.integer)
			{
			case 4:
				return 3;
			case 3:
				return 13;
			case 2:
				return 25;
			default:
				return game::TECHNIQUE_UNLIT;
			}
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
				game::Dvar_ClearModified(dvars::r_fullbright);
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
			if (game::environment::is_dedi())
			{
				return;
			}

			dvars::r_fullbright = dvars::register_int("r_fullbright", 0, 0, 4, game::DVAR_FLAG_SAVED, "Toggles rendering without lighting");

			r_init_draw_method_hook.create(SELECT_VALUE(0x5467E0_b, 0x669580_b), &r_init_draw_method_stub);
			r_update_front_end_dvar_options_hook.create(SELECT_VALUE(0x583560_b, 0x6A78C0_b), &r_update_front_end_dvar_options_stub);

			// use "saved" flags
			dvars::override::register_enum("r_normalMap", game::DVAR_FLAG_SAVED);
			dvars::override::register_enum("r_specularMap", game::DVAR_FLAG_SAVED);
			dvars::override::register_enum("r_specOccMap", game::DVAR_FLAG_SAVED);
		}
	};
}

REGISTER_COMPONENT(renderer::component)
