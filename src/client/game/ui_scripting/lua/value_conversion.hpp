#pragma once
#include "context.hpp"
#include "../script_value.hpp"

namespace ui_scripting::lua
{
	script_value convert(const sol::lua_value& value);
	sol::lua_value convert(lua_State* state, const script_value& value);
}
