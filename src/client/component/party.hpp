#pragma once
#include "game/game.hpp"

namespace party
{
	struct connection_state
	{
		game::netadr_s host;
		std::string challenge;
		bool hostDefined;
		std::string motd;
		int max_clients;
		std::string base_url;
	};

	struct discord_information
	{
		std::string image;
		std::string image_text;
	};

	void user_download_response(bool response);

	void menu_error(const std::string& error);

	void reset_server_connection_state();

	void connect(const game::netadr_s& target);
	void start_map(const std::string& mapname, bool dev = false);

	void clear_sv_motd();
	connection_state get_server_connection_state();
	discord_information get_server_discord_information();

	int get_client_num_by_name(const std::string& name);

	int get_client_count();
	int get_bot_count();
}