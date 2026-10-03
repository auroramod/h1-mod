#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_comworld final : public component_interface
{
public:
	void post_unpack() override;

private:
	static bool draw_window(game::ComWorld* asset);
};
#endif
