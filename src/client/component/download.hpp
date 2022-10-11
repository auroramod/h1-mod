#pragma once

#include "game/game.hpp"
#include <utils/info_string.hpp>

namespace download
{
	struct file_t
	{
		std::string name;
		std::string hash;
	};

	void start_download(const game::netadr_s& target, const utils::info_string& info, const std::vector<file_t>& files);
	void stop_download();
}
