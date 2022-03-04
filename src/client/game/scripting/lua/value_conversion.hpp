#pragma once

#include "context.hpp"

namespace scripting::lua
{
	sol::lua_value entity_to_struct(lua_State* state, unsigned int parent_id);

	script_value convert(const sol::lua_value& value);
	sol::lua_value convert(lua_State* state, const script_value& value);
}
