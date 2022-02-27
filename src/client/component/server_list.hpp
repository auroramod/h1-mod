#pragma once

#include "game/game.hpp"
#include <utils/info_string.hpp>

namespace server_list
{
	bool get_master_server(game::netadr_s& address);
	void handle_info_response(const game::netadr_s& address, const utils::info_string& info);

	bool sl_key_event(int key, int down);
}