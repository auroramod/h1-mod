#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class bullet final : public component_interface
{
public:
	void post_unpack() override;

private:
	static float bg_get_surface_penetration_depth_stub(game::Weapon weapon, bool is_alternate, int surface_type);
};
