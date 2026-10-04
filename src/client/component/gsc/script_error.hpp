#pragma once
#include "game/game.hpp"

namespace script_error
{
	std::optional<std::pair<std::string, std::string>> find_function(const char* pos);
}
