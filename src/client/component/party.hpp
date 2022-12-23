#pragma once
#include "game/game.hpp"

namespace party
{
	std::string get_www_url();
	void user_download_response(bool response);

	void menu_error(const std::string& error);

	void reset_connect_state();

	void connect(const game::netadr_s& target);
	void start_map(const std::string& mapname, bool dev = false);

	void clear_sv_motd();
	game::netadr_s get_state_host();
	int server_client_count();

	int get_client_num_by_name(const std::string& name);

	int get_client_count();
	int get_bot_count();
}