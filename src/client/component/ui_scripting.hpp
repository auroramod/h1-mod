#pragma once
#include "game/ui_scripting/lua/value_conversion.hpp"

namespace ui_scripting
{
	int main_function_handler(game::hks::lua_State* state);
	void add_converted_function(game::hks::cclosure* closure, const sol::protected_function& function);
	void clear_converted_functions();

	void enable_error_hook();
	void disable_error_hook();

	bool lui_running();
}