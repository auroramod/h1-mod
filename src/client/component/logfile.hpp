#pragma once

#include "game/scripting/entity.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/lua/value_conversion.hpp"
#include "game/scripting/lua/error.hpp"

namespace logfile
{
	extern bool hook_enabled;

	void set_lua_hook(const char* pos, const sol::protected_function&);
	void set_gsc_hook(const char* source, const char* target);
	void clear_hook(const char* pos);
	size_t get_hook_count();

	void add_player_damage_callback(const sol::protected_function& callback);
	void add_player_killed_callback(const sol::protected_function& callback);
	void clear_callbacks();

	void enable_vm_execute_hook();
	void disable_vm_execute_hook();

	bool client_command_stub(const int client_num);
}