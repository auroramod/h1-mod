#pragma once

namespace logfile
{
	extern std::unordered_map<const char*, sol::protected_function> vm_execute_hooks;

	void add_player_damage_callback(const sol::protected_function& callback);
	void add_player_killed_callback(const sol::protected_function& callback);
	void clear_callbacks();

	void enable_vm_execute_hook();
	void disable_vm_execute_hook();
}