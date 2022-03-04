#include <std_include.hpp>
#include "error.hpp"
#include "../execution.hpp"

#include "component/console.hpp"

namespace scripting::lua
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

	void handle_error(const sol::protected_function_result& result)
	{
		if (!result.valid())
		{
			console::error("************** Script execution error **************\n");

			const sol::error err = result;
			console::error("%s\n", err.what());

			console::error("****************************************************\n");

			notify_error();
		}
	}
}
