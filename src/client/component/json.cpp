#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"
#include "gsc.hpp"
#include "json.hpp"

#include <json.hpp>

namespace json
{
	namespace
	{
		nlohmann::json gsc_to_json(scripting::script_value _value);

		nlohmann::json entity_to_array(unsigned int id)
		{
			scripting::array array(id);
			nlohmann::json obj;

			auto string_indexed = -1;
			const auto keys = array.get_keys();
			for (auto i = 0; i < keys.size(); i++)
			{
				const auto is_int = keys[i].is<int>();
				const auto is_string = keys[i].is<std::string>();

				if (string_indexed == -1)
				{
					string_indexed = is_string;
				}

				if (!string_indexed && is_int)
				{
					const auto index = keys[i].as<int>();
					obj[index] = gsc_to_json(array[index]);
				}
				else if (string_indexed && is_string)
				{
					const auto key = keys[i].as<std::string>();
					obj.emplace(key, gsc_to_json(array[key]));
				}
			}

			return obj;
		}

		nlohmann::json vector_to_array(const float* value)
		{
			nlohmann::json obj;
			obj.push_back(value[0]);
			obj.push_back(value[1]);
			obj.push_back(value[2]);

			return obj;
		}

		nlohmann::json gsc_to_json(scripting::script_value _value)
		{
			const auto variable = _value.get_raw();
			const auto value = variable.u;
			const auto type = variable.type;

			switch (type)
			{
			case (game::SCRIPT_NONE):
				return {};
			case (game::SCRIPT_INTEGER):
				return value.intValue;
			case (game::SCRIPT_FLOAT):
				return value.floatValue;
			case (game::SCRIPT_STRING):
			case (game::SCRIPT_ISTRING):
				return game::SL_ConvertToString(static_cast<game::scr_string_t>(value.stringValue));
			case (game::SCRIPT_VECTOR):
				return vector_to_array(value.vectorValue);
			case (game::SCRIPT_OBJECT):
			{
				const auto object_type = game::scr_VarGlob->objectVariableValue[value.uintValue].w.type;

				switch (object_type)
				{
				case (game::SCRIPT_STRUCT):
					return "[struct]";
				case (game::SCRIPT_ARRAY):
					return entity_to_array(value.uintValue);
				default:
					return "[entity]";
				}
			}
			case (game::SCRIPT_FUNCTION):
				return "[function]";
			default:
				return "[unknown type]";
			};
		}

		scripting::script_value json_to_gsc(nlohmann::json obj)
		{
			const auto type = obj.type();

			switch (type)
			{
			case (nlohmann::detail::value_t::number_integer):
			case (nlohmann::detail::value_t::number_unsigned):
				return obj.get<int>();
			case (nlohmann::detail::value_t::number_float):
				return obj.get<float>();
			case (nlohmann::detail::value_t::string):
				return obj.get<std::string>();
			case (nlohmann::detail::value_t::array):
			{
				scripting::array array;

				for (const auto& [key, value] : obj.items())
				{
					array.push(json_to_gsc(value));
				}

				return array.get_raw();
			}
			case (nlohmann::detail::value_t::object):
			{
				scripting::array array;

				for (const auto& [key, value] : obj.items())
				{
					array[key] = json_to_gsc(value);
				}

				return array.get_raw();
			}
			}

			return {};
		}
	}

	std::string gsc_to_string(const scripting::script_value& value)
	{
		return gsc_to_json(value).dump();
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gsc::function::add("array", [](const gsc::function_args& args)
			{
				scripting::array array(args.get_raw());
				return array.get_raw();
			});

			gsc::function::add("map", [](const gsc::function_args& args)
			{
				scripting::array array;

				for (auto i = 0u; i < args.size(); i += 2)
				{
					if (i >= args.size() - 1)
					{
						continue;
					}

					const auto key = args[i].as<std::string>();
					array[key] = args[i + 1];
				}

				return array;
			});

			gsc::function::add("jsonparse", [](const gsc::function_args& args)
			{
				const auto json = args[0].as<std::string>();
				const auto obj = nlohmann::json::parse(json);
				return json_to_gsc(obj);
			});

			gsc::function::add("jsonserialize", [](const gsc::function_args& args)
			{
				const auto value = args[0];
				auto indent = -1;

				if (args.size() > 1)
				{
					indent = args[1].as<int>();
				}

				return gsc_to_json(value).dump(indent);
			});

			gsc::function::add("jsonprint", [](const gsc::function_args& args) -> scripting::script_value
			{
				std::string buffer;

				for (const auto arg : args.get_raw())
				{
					buffer.append(gsc_to_string(arg));
					buffer.append("\t");
				}

				printf("%s\n", buffer.data());
				return {};
			});
		}
	};
}

REGISTER_COMPONENT(json::component)
