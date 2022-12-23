#include <std_include.hpp>
#include "value_conversion.hpp"
#include "game/scripting/functions.hpp"
#include "game/scripting/execution.hpp"
#include "component/logfile.hpp"

namespace scripting::lua
{
	namespace
	{
		struct array_value
		{
			int index;
			script_value value;
		};

		sol::lua_value entity_to_array(lua_State* state, unsigned int id)
		{
			auto table = sol::table::create(state);
			auto metatable = sol::table::create(state);

			std::unordered_map<std::string, array_value> values;

			const auto offset = 0xFA00 * (id & 3);

			auto current = game::scr_VarGlob->objectVariableChildren[id].firstChild;
			auto idx = 1;

			for (auto i = offset + current; current; i = offset + current)
			{
				const auto var = game::scr_VarGlob->childVariableValue[i];

				if (var.type == game::VAR_UNDEFINED)
				{
					current = var.nextSibling;
					continue;
				}

				const auto string_value = (game::scr_string_t)((unsigned __int8)var.name_lo + (var.k.keys.name_hi << 8));
				const auto* str = game::SL_ConvertToString(string_value);

				std::string key = string_value < 0x40000 && str
					? str
					: std::to_string(idx++);

				game::VariableValue variable{};
				variable.type = var.type;
				variable.u = var.u.u;

				array_value value;
				value.index = i;
				value.value = variable;

				values[key] = value;

				current = var.nextSibling;
			}

			table["getkeys"] = [values]()
			{
				std::vector<std::string> _keys;

				for (const auto& entry : values)
				{
					_keys.push_back(entry.first);
				}

				return _keys;
			};

			metatable[sol::meta_function::new_index] = [values](const sol::table t, const sol::this_state s,
				const sol::lua_value& key_value, const sol::lua_value& value)
			{
				const auto key = key_value.is<int>()
					? std::to_string(key_value.as<int>())
					: key_value.as<std::string>();

				if (values.find(key) == values.end())
				{
					return;
				}

				const auto i = values.at(key).index;
				const auto variable = &game::scr_VarGlob->childVariableValue[i];

				const auto new_variable = convert({s, value}).get_raw();

				game::AddRefToValue(new_variable.type, new_variable.u);
				game::RemoveRefToValue(variable->type, variable->u.u);

				variable->type = (char)new_variable.type;
				variable->u.u = new_variable.u;
			};

			metatable[sol::meta_function::index] = [values](const sol::table t, const sol::this_state s,
				const sol::lua_value& key_value)
			{
				const auto key = key_value.is<int>()
					? std::to_string(key_value.as<int>())
					: key_value.as<std::string>();

				if (values.find(key) == values.end())
				{
					return sol::lua_value{s, sol::lua_nil};
				}

				return convert(s, values.at(key).value);
			};

			metatable[sol::meta_function::length] = [values]()
			{
				return values.size();
			};

			table[sol::metatable_key] = metatable;

			return {state, table};
		}

		game::VariableValue convert_function(sol::lua_value value)
		{
			const auto function = value.as<sol::protected_function>();
			const auto index = reinterpret_cast<char*>(logfile::get_hook_count() + 1);
			logfile::set_lua_hook(index, function);

			game::VariableValue func{};
			func.type = game::VAR_FUNCTION;
			func.u.codePosValue = index;

			return func;
		}

		sol::lua_value convert_function(lua_State* state, const char* pos)
		{
			return sol::overload(
				[pos](const entity& entity, const sol::this_state s, sol::variadic_args va)
				{
					std::vector<script_value> arguments{};

					for (auto arg : va)
					{
						arguments.push_back(convert({s, arg}));
					}

					return convert(s, exec_ent_thread(entity, pos, arguments));
				},
				[pos](const sol::this_state s, sol::variadic_args va)
				{
					std::vector<script_value> arguments{};

					for (auto arg : va)
					{
						arguments.push_back(convert({s, arg}));
					}

					return convert(s, exec_ent_thread(*game::levelEntityId, pos, arguments));
				}
			);
		}
	}

	sol::lua_value entity_to_struct(lua_State* state, unsigned int parent_id)
	{
		auto table = sol::table::create(state);
		auto metatable = sol::table::create(state);

		metatable[sol::meta_function::new_index] = [parent_id](const sol::table t, const sol::this_state s,
			const sol::lua_value& field, const sol::lua_value& value)
		{
			const auto new_variable = convert({s, value});
			if (field.is<unsigned int>())
			{
				scripting::set_object_variable(parent_id, field.as<unsigned int>(), new_variable);
			}
			else if (field.is<std::string>())
			{
				scripting::set_object_variable(parent_id, field.as<std::string>(), new_variable);
			}
		};

		metatable[sol::meta_function::index] = [parent_id](const sol::table t, const sol::this_state s,
			const sol::lua_value& field)
		{
			if (field.is<unsigned int>())
			{
				return convert(s, scripting::get_object_variable(parent_id, field.as<unsigned int>()));
			}
			else if (field.is<std::string>())
			{
				return convert(s, scripting::get_object_variable(parent_id, field.as<std::string>()));
			}

			return sol::lua_value{s, sol::lua_nil};
		};

		table[sol::metatable_key] = metatable;

		return {state, table};
	}

	script_value convert(const sol::lua_value& value)
	{
		if (value.is<int>())
		{
			return {value.as<int>()};
		}

		if (value.is<unsigned int>())
		{
			return {value.as<unsigned int>()};
		}

		if (value.is<bool>())
		{
			return {value.as<bool>()};
		}

		if (value.is<double>())
		{
			return {value.as<double>()};
		}

		if (value.is<float>())
		{
			return {value.as<float>()};
		}

		if (value.is<std::string>())
		{
			return {value.as<std::string>()};
		}

		if (value.is<entity>())
		{
			return {value.as<entity>()};
		}

		if (value.is<vector>())
		{
			return {value.as<vector>()};
		}

		if (value.is<sol::protected_function>())
		{
			return convert_function(value);
		}

		return {};
	}

	sol::lua_value convert(lua_State* state, const script_value& value)
	{
		if (value.is<int>())
		{
			return {state, value.as<int>()};
		}

		if (value.is<float>())
		{
			return {state, value.as<float>()};
		}

		if (value.is<std::string>())
		{
			return {state, value.as<std::string>()};
		}
		
		if (value.is<std::map<std::string, script_value>>())
		{
			return entity_to_struct(state, value.get_raw().u.uintValue);
		}

		if (value.is<scripting::array>())
		{
			return entity_to_array(state, value.get_raw().u.uintValue);
		}

		if (value.is<function>())
		{
			return convert_function(state, value.get_raw().u.codePosValue);
		}

		if (value.is<entity>())
		{
			return {state, value.as<entity>()};
		}

		if (value.is<vector>())
		{
			return {state, value.as<vector>()};
		}

		return {state, sol::lua_nil};
	}
}
