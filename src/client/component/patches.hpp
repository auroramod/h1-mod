#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class patches final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void patch_mp();

	static const char* live_get_local_client_name();
	static void sv_kick_client_num(const int client_num, const char* reason);
	static std::string get_login_username();
	static void com_register_dvars_stub();
	static void cg_set_client_dvar_from_server_stub(void* client_num, void* cgame_glob, const char* dvar_hash, const char* value);
	static game::dvar_t* get_client_dvar(const char* name);
	static bool get_client_dvar_hash(game::dvar_t* dvar, int* hash);
	static const char* db_read_raw_file_stub(const char* filename, char* buf, const int size);
	static void bsp_sys_error_stub(const char* error, const char* arg1);
	static void cmd_lui_notify_server_stub(game::gentity_s* ent);
	static void sv_execute_client_message_stub(game::client_t* client, game::msg_t* msg);
	static void aim_assist_add_to_target_list(void* aa_glob, void* screen_target);
	static void missing_content_error_stub(int, const char*);
	static void init_network_dvars_stub(game::dvar_t* dvar);
	static int ui_draw_crosshair();
	static void create_2d_texture_stub_1(const char* fmt, ...);
	static void create_2d_texture_stub_2(game::errorParm code, const char* fmt, ...);
	static void swap_chain_stub(game::errorParm code, const char* fmt, ...);
	static void dvar_set_bool(game::dvar_t* dvar, const bool value);
	static void sub_157FA0_stub();
	static void cl_gamepad_scrolling_buttons_stub(int local_client_num, int controller_index);
	static game::dvar_t* register_bool_stub(const int hash, __int64 name, const bool value, const unsigned int flags);
	static void sv_shutdown_stub(const char* finalmsg);
	static void com_quit_f_stub();
	static void cg_calc_agent_lerp_positions_stub(int local_client_num, std::uint8_t* cent);
};
