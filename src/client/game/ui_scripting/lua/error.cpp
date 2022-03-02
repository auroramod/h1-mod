#include <std_include.hpp>
#include "error.hpp"

namespace ui_scripting::lua
{
	void handle_error(const sol::protected_function_result& result)
	{
		if (!result.valid())
		{
			printf("************** UI Script execution error **************\n");

			const sol::error err = result;
			printf("%s\n", err.what());

			printf("****************************************************\n");
		}
	}
}
