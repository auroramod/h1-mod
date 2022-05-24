#pragma once

#include "game/scripting/entity.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/lua/value_conversion.hpp"
#include "game/scripting/lua/error.hpp"

namespace logfile
{
	extern std::unordered_map<const char*, sol::protected_function> vm_execute_hooks;

	void add_player_damage_callback(const sol::protected_function& callback);
	void add_player_killed_callback(const sol::protected_function& callback);
	void clear_callbacks();

	void enable_vm_execute_hook();
	void disable_vm_execute_hook();

	bool client_command_stub(const int client_num);
}