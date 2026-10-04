#pragma once
#include "game/game.hpp"

namespace gsc
{
	std::optional<std::pair<std::string, std::string>> find_function(const char* pos);
}