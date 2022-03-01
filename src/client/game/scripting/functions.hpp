#pragma once
#include "game/game.hpp"

namespace scripting
{
	extern std::unordered_map<std::string, unsigned> method_map;
	extern std::unordered_map<std::string, unsigned> function_map;
	extern std::unordered_map<std::string, unsigned> token_map;

	using script_function = void(*)(game::scr_entref_t);

	std::string find_token(unsigned int id);
	unsigned int find_token_id(const std::string& name);

	script_function find_function(const std::string& name, const bool prefer_global);
}
