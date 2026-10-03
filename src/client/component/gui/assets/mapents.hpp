#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_mapents final : public component_interface
{
public:
	void post_unpack() override;
	void pre_destroy() override;

private:
	static void draw_entity_string(game::MapEnts* asset);
	static bool draw_asset(game::MapEnts* asset);
};
#endif
