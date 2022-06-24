#pragma once

#include "context.hpp"

namespace scripting::lua
{
	void handle_error(const sol::protected_function_result& result);
}
