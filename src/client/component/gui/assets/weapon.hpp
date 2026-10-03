#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_weapon final : public component_interface
{
public:
	void post_unpack() override;

private:
	static bool draw_weapon_window(game::WeaponDef* asset);
	static void give_weapon(game::WeaponDef* asset);
	static void spawn_model(game::WeaponDef* asset);
};
#endif
