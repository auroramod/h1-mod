#pragma once

#include "game/game.hpp"
#include <utils/info_string.hpp>

namespace download
{
	void start_download(const game::netadr_s& target, const utils::info_string& info);
	void stop_download();
}
