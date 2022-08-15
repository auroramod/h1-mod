#pragma once

#include "context.hpp"

namespace scripting
{
	namespace lua
	{
		void handle_error(const sol::protected_function_result& result);
	}

	void invoke_error(const char* reason);
}
