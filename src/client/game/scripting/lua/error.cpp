#include <std_include.hpp>
#include "error.hpp"
#include "../execution.hpp"

#include "component/console.hpp"

#include <utils/string.hpp>

namespace scripting
{
	namespace
	{
		void notify_error()
		{
			try
			{
				call("iprintln", {"^1Script execution error!"});
			}
			catch (...)
			{
			}
		}
	}

	namespace lua
	{
		void handle_error(const sol::protected_function_result& result)
		{
			if (!result.valid())
			{
				const sol::error err = result;
				scripting::invoke_error(err.what());
			}
		}
	}

	void invoke_error(const char* reason)
	{
		console::error("************** Script execution error **************\n");
		console::error("%s\n", reason);
		console::error("****************************************************\n");

		notify_error();
	}
}
