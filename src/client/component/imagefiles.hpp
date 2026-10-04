#pragma once
#include "game/game.hpp"
#include <utils/hook.hpp>

namespace imagefiles
{
	void close_custom_handles();
	void close_handle(const std::string& fastfile);
}
