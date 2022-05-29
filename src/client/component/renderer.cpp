#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace renderer
{
	static int tech = 0;

	void custom_gfx_draw_method()
	{
		game::gfxDrawMethod->drawScene = game::GFX_DRAW_SCENE_STANDARD;
		game::gfxDrawMethod->baseTechType = tech;
		game::gfxDrawMethod->emissiveTechType = tech;
		game::gfxDrawMethod->forceTechType = tech;
	}

	void default_gfx_draw_method()
	{
		game::gfxDrawMethod->drawScene = game::GFX_DRAW_SCENE_STANDARD;
		game::gfxDrawMethod->baseTechType = game::TECHNIQUE_LIT;
		game::gfxDrawMethod->emissiveTechType = game::TECHNIQUE_EMISSIVE;
		game::gfxDrawMethod->forceTechType = 242;
	}

	void update_tech(int new_tech)
	{
		tech = new_tech;
		custom_gfx_draw_method();
	}

	namespace
	{
		utils::hook::detour r_init_draw_method_hook;
		utils::hook::detour r_update_front_end_dvar_options_hook;

		void r_init_draw_method_stub()
		{
			default_gfx_draw_method();
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

			r_init_draw_method_hook.create(SELECT_VALUE(0x5467E0_b, 0x669580_b), &r_init_draw_method_stub);

			// use "saved" flags
			dvars::override::register_enum("r_normalMap", game::DVAR_FLAG_SAVED);
			dvars::override::register_enum("r_specularMap", game::DVAR_FLAG_SAVED);
			dvars::override::register_enum("r_specOccMap", game::DVAR_FLAG_SAVED);
		}
	};
}

REGISTER_COMPONENT(renderer::component)
