#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "download.hpp"

#include <utils/hook.hpp>
#include <utils/info_string.hpp>

class party final : public component_interface
{
public:
	void post_unpack() override;

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

	static void user_download_response(bool response);
	static void menu_error(const std::string& error);
	static void clear_sv_motd();
	static int get_client_num_by_name(const std::string& name);
	static void reset_server_connection_state();
	static int get_client_count();
	static int get_bot_count();
	static void connect(const game::netadr_s& target);
	static void start_map(const std::string& mapname, bool dev = false);
	static connection_state get_server_connection_state();
	static std::optional<discord_information> get_server_discord_info();

private:
	static std::string get_www_url();
	static void perform_game_initialization();
	static void connect_to_party(const game::netadr_s& target, const std::string& mapname, const std::string& gametype);
	static std::string get_dvar_string(const std::string& dvar);
	static int get_dvar_int(const std::string& dvar);
	static bool get_dvar_bool(const std::string& dvar);
	static void set_didyouknow_stub(const char* table, int column, const char* dvar_name);
	static void disconnect();
	static void cl_disconnect_stub(int show_main_menu); // possibly bool
	static std::string get_file_hash(const std::string& file);
	static std::string get_usermap_file_path(const std::string& mapname, const std::string& extension);
	static void generate_hashes(const std::string& mapname);
	static void check_download_map(const utils::info_string& info, std::vector<download::file_t>& files);
	static bool check_download_mod(const utils::info_string& info, std::vector<download::file_t>& files);
	static void close_joining_popups();
	static std::string get_whitelist_json_path();
	static nlohmann::json get_whitelist_json_object();
	static std::string target_ip_to_string(const game::netadr_s& target);
	static bool should_user_confirm(const game::netadr_s& target);
	static bool download_files(const game::netadr_s& target, const utils::info_string& info, bool allow_download);
	static void set_new_map(const char* mapname, const char* gametype, game::msg_t* msg);
	static void loading_new_map_cl_stub(utils::hook::assembler& a);
	static void sv_spawn_server_stub(char* map, int is_preloaded, int savegame, int is_restart);
	static void net_out_of_band_print_stub(game::netsrc_t sock, game::netadr_s* addr, const char* data);
};
