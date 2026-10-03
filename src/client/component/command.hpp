#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class command final : public component_interface
{
public:
	void post_unpack() override;

	class params
	{
	public:
		params();

		int size() const;
		const char* get(int index) const;
		std::string join(int index) const;
		std::vector<std::string> get_all() const;

		const char* operator[](const int index) const
		{
			return this->get(index); //
		}

	private:
		int nesting_;
	};

	class params_sv
	{
	public:
		params_sv();

		int size() const;
		const char* get(int index) const;
		std::string join(int index) const;
		std::vector<std::string> get_all() const;

		const char* operator[](const int index) const
		{
			return this->get(index); //
		}

	private:
		int nesting_;
	};

	static void read_startup_variable(const std::string& dvar);
	static void add_raw(const char* name, void (*callback)());
	static void add(const char* name, const std::function<void(const params&)>& callback);
	static void add(const char* name, const std::function<void()>& callback);
	static void add_sv(const char* name, std::function<void(int, const params_sv&)> callback);
	static void execute(std::string command, bool sync = false);
	static void add_test(const char* name, void (*callback)());

private:
	static void main_handler();
	static void client_command(const char client_num);
	static void parse_command_line();
	static void parse_startup_variables();
	static void parse_commandline_stub(char* commandline);
	static game::dvar_t* dvar_command_stub();
	static void client_println(int client_num, const std::string& text);
	static bool check_cheats(int client_num);
	static void cmd_give_weapon(const int client_num, const std::vector<std::string>& params);
	static void cmd_drop_weapon(int client_num);
	static void cmd_take_weapon(int client_num, const std::vector<std::string>& params);
	static void cmd_kill(int client_num);
	static void toggle_entity_flag(int client_num, int value, const std::string& name);
	static void toggle_entity_flag(int value, const std::string& name);
	static void toggle_client_flag(int client_num, int value, const std::string& name);
	static void toggle_client_flag(int value, const std::string& name);
	static void add_commands_generic();
	static void add_commands_sp();
	static void add_commands_mp();
};
