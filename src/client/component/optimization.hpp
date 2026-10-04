#pragma once
#include "game/game.hpp"
#include <utils/hook.hpp>
#include <condition_variable>
#include <deque>

namespace optimization
{
	void begin_zone_load(const char* zone_name);
	void end_zone_load();
	bool throttle_streaming();
}
