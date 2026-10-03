#pragma once
#include "loader/component_loader.hpp"

class dedicated final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void init_dedicated_server();
	static void sv_kill_server_f();
	static void kill_server();
	static void send_heartbeat();
	static std::vector<std::string>& get_startup_command_queue();
	static void execute_startup_command(int client, int controller_index, const char* command);
	static void execute_startup_command_queue();
	static std::vector<std::string>& get_console_command_queue();
	static void execute_console_command(int local_client_num, const char* command);
	static void execute_console_command_queue();
	static void sync_gpu_stub();
	static void ui_set_active_menu_stub(void* local_client_num, int menu);
};
