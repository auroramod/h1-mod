#pragma once

#include "game/game.hpp"

namespace gsc
{
	extern void* func_table[0x1000];

	game::ScriptFile* find_script(game::XAssetType /*type*/, const char* name, int /*allow_create_default*/);

	void load_main_handles();
	void load_init_handles();
}
