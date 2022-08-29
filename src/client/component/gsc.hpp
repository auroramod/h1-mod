#pragma once

#include "game/game.hpp"

namespace gsc
{
	using builtin_function = void(*)();
	using builtin_method = void(*)(game::scr_entref_t);

	extern void* func_table[0x1000];

	game::ScriptFile* find_script(game::XAssetType /*type*/, const char* name, int /*allow_create_default*/);

	void load_main_handles();
	void load_init_handles();

	namespace function
	{
		void add(const std::string& name, builtin_function function);
	}

	namespace method
	{
		void add(const std::string& name, builtin_method function);
	}
}
