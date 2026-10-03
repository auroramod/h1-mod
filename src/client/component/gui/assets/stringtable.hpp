#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_stringtable final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void copy_table(game::StringTable* asset);
	static bool draw_asset(game::StringTable* asset);
};
#endif
