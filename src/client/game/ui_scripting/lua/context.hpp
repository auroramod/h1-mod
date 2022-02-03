#pragma once
#pragma warning(push)
#pragma warning(disable: 4702)

#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 0
#include <sol/sol.hpp>

#include "scheduler.hpp"

namespace ui_scripting::lua
{
	class context
	{
	public:
		context(std::string folder);
		~context();

		context(context&&) noexcept = delete;
		context& operator=(context&&) noexcept = delete;

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		void run_frame();

	private:
		sol::state state_{};
		std::string folder_;
		std::unordered_set<std::string> loaded_scripts_;

		scheduler scheduler_;

		void load_script(const std::string& script);
	};
}
