#pragma once
#include "game/game.hpp"

namespace scripting
{
	using script_function = void(*)(game::scr_entref_t);

	std::string find_token(std::uint32_t id);
	std::string find_token_single(std::uint32_t id);
	unsigned int find_token_id(const std::string& name);

	script_function get_function_by_index(std::uint32_t index);
	script_function find_function(const std::string& name, const bool prefer_global);
}
