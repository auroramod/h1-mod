#include <std_include.hpp>
#include "context.hpp"
#include "error.hpp"
#include "value_conversion.hpp"
#include "../script_value.hpp"
#include "../execution.hpp"

#include "../../../component/ui_scripting.hpp"
#include "../../../component/command.hpp"

#include "component/game_console.hpp"
#include "component/scheduler.hpp"

#include <utils/string.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>

namespace ui_scripting::lua
{
	namespace
	{
		void setup_types(sol::state& state, scheduler& scheduler)
		{
			struct game
			{
			};
			auto game_type = state.new_usertype<game>("game_");
			state["game"] = game();

			game_type["ontimeout"] = [&scheduler](const game&, const sol::protected_function& callback,
			                                      const long long milliseconds)
			{
				return scheduler.add(callback, milliseconds, true);
			};

			game_type["oninterval"] = [&scheduler](const game&, const sol::protected_function& callback,
			                                       const long long milliseconds)
			{
				return scheduler.add(callback, milliseconds, false);
			};

			auto userdata_type = state.new_usertype<userdata>("userdata_");

			userdata_type["new"] = sol::property(
				[](const userdata& userdata, const sol::this_state s)
				{
					return convert(s, userdata.get("new"));
				},
				[](const userdata& userdata, const sol::this_state s, const sol::lua_value& value)
				{
					userdata.set("new", convert({s, value}));
				}
			);

			
			userdata_type["get"] = [](const userdata& userdata, const sol::this_state s,
				const sol::lua_value& key)
			{
				return convert(s, userdata.get(convert({s, key})));
			};

			userdata_type["set"] = [](const userdata& userdata, const sol::this_state s,
				const sol::lua_value& key, const sol::lua_value& value)
			{
				userdata.set(convert({s, key}), convert({s, value}));
			};

			userdata_type[sol::meta_function::index] = [](const userdata& userdata, const sol::this_state s, 
				const sol::lua_value& key)
			{
				return convert(s, userdata.get(convert({s, key})));
			};

			userdata_type[sol::meta_function::new_index] = [](const userdata& userdata, const sol::this_state s, 
				const sol::lua_value& key, const sol::lua_value& value)
			{
				userdata.set(convert({s, key }), convert({s, value}));
			};

			auto table_type = state.new_usertype<table>("table_");

			table_type["new"] = sol::property(
				[](const table& table, const sol::this_state s)
				{
					return convert(s, table.get("new"));
				},
				[](const table& table, const sol::this_state s, const sol::lua_value& value)
				{
					table.set("new", convert({s, value}));
				}
			);

			table_type["get"] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key)
			{
				return convert(s, table.get(convert({s, key})));
			};

			table_type["set"] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key, const sol::lua_value& value)
			{
				table.set(convert({s, key}), convert({s, value}));
			};

			table_type[sol::meta_function::index] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key)
			{
				return convert(s, table.get(convert({s, key})));
			};

			table_type[sol::meta_function::new_index] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key, const sol::lua_value& value)
			{
				table.set(convert({s, key}), convert({s, value}));
			};

			auto function_type = state.new_usertype<function>("function_");

			function_type[sol::meta_function::call] = [](const function& function, const sol::this_state s, sol::variadic_args va)
			{
				arguments arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				const auto values = function.call(arguments);
				std::vector<sol::lua_value> returns;

				for (const auto& value : values)
				{
					returns.push_back(convert(s, value));
				}

				return sol::as_returns(returns);
			};

			state["luiglobals"] = table((*::game::hks::lua_state)->globals.v.table);
			state["CoD"] = state["luiglobals"]["CoD"];
			state["LUI"] = state["luiglobals"]["LUI"];
			state["Engine"] = state["luiglobals"]["Engine"];
			state["Game"] = state["luiglobals"]["Game"];
		}
	}

	context::context(std::string folder)
		: folder_(std::move(folder))
		  , scheduler_(state_)
	{
		this->state_.open_libraries(sol::lib::base,
		                            sol::lib::package,
		                            sol::lib::io,
		                            sol::lib::string,
		                            sol::lib::os,
		                            sol::lib::math,
		                            sol::lib::table);

		this->state_["include"] = [this](const std::string& file)
		{
			this->load_script(file);
		};

		sol::function old_require = this->state_["require"];
		auto base_path = utils::string::replace(this->folder_, "/", ".") + ".";
		this->state_["require"] = [base_path, old_require](const std::string& path)
		{
			return old_require(base_path + path);
		};

		this->state_["scriptdir"] = [this]()
		{
			return this->folder_;
		};

		setup_types(this->state_, this->scheduler_);

		printf("Loading ui script '%s'\n", this->folder_.data());
		this->load_script("__init__");
	}

	context::~context()
	{
		this->state_.collect_garbage();
		this->scheduler_.clear();
		this->state_ = {};
	}

	void context::run_frame()
	{
		this->scheduler_.run_frame();
		this->state_.collect_garbage();
	}

	void context::load_script(const std::string& script)
	{
		if (!this->loaded_scripts_.emplace(script).second)
		{
			return;
		}

		const auto file = (std::filesystem::path{this->folder_} / (script + ".lua")).generic_string();
		handle_error(this->state_.safe_script_file(file, &sol::script_pass_on_error));
	}
}
