#ifdef _DEBUG
#pragma once

#include "game/game.hpp"
#include "game/dvars.hpp"

namespace gui::asset_list::xmodel
{
	void spawn_xmodel(game::XModel* asset, const float scale = 1.f);
}
#endif
