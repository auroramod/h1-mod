#pragma once

#include "game/game.hpp"

namespace gsc
{
	game::ScriptFile* find_script(game::XAssetType /*type*/, const char* name, int /*allow_create_default*/);
}
