#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "bullet.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

static game::dvar_t* bg_surface_penetration;
static utils::hook::detour bg_get_surface_penetration_depth_hook;

void bullet::post_unpack()
{
	if (game::environment::is_sp())
	{
		return;
	}

	bg_surface_penetration = dvars::register_float("bg_surfacePenetration", 0.0f, 0.0f, std::numeric_limits<float>::max(), 0,
		"Set to a value greater than 0 to override the bullet surface penetration depth");

	bg_get_surface_penetration_depth_hook.create(0x1401F8280, &bg_get_surface_penetration_depth_stub); // BG_GetSurfacePenetrationDepth
}

float bullet::bg_get_surface_penetration_depth_stub(game::Weapon weapon, bool is_alternate, int surface_type)
{
	if (bg_surface_penetration->current.value > 0.0f)
	{
		return bg_surface_penetration->current.value;
	}

	return bg_get_surface_penetration_depth_hook.invoke<float>(weapon, is_alternate, surface_type);
}

REGISTER_COMPONENT(bullet)
