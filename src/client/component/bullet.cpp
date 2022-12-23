#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace bullet
{
	namespace
	{
		game::dvar_t* bg_surface_penetration;
		utils::hook::detour bg_get_surface_penetration_depth_hook;

		float bg_get_surface_penetration_depth_stub(game::Weapon weapon, bool is_alternate, int surface_type)
		{
			if (bg_surface_penetration->current.value > 0.0f)
			{
				return bg_surface_penetration->current.value;
			}

			return bg_get_surface_penetration_depth_hook.invoke<float>(weapon, is_alternate, surface_type);
		}
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

			bg_surface_penetration = dvars::register_float("bg_surfacePenetration", 0.0f, 0.0f, std::numeric_limits<float>::max(), 0,
				"Set to a value greater than 0 to override the bullet surface penetration depth");

			bg_get_surface_penetration_depth_hook.create(0x2E1110_b, &bg_get_surface_penetration_depth_stub);
		}
	};
}

REGISTER_COMPONENT(bullet::component)
