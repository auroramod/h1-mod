#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "script_extension.hpp"
#include "script_error.hpp"

#include "component/command.hpp"
#include "component/scheduler.hpp"
#include "component/scripting.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

using namespace utils::string;

namespace script_error
{
	namespace
	{
		utils::hook::detour scr_emit_function_hook;

		std::uint32_t current_filename = 0;

		std::string unknown_function_error;

		std::array<const char*, 27> var_typename =
		{
			"undefined",
			"object",
			"string",
			"localized string",
			"vector",
			"float",
			"int",
			"codepos",
			"precodepos",
			"function",
			"builtin function",
			"builtin method",
			"stack",
			"animation",
			"pre animation",
			"thread",
			"thread",
			"thread",
			"thread",
			"struct",
			"removed entity",
			"entity",
			"array",
			"removed thread",
			"<free>",
			"thread list",
			"endon list",
		};

		template <size_t address>
		void safe_func()
		{
			static utils::hook::detour hook;
			static const auto stub = []()
			{
				__try
				{
					hook.invoke<void>();
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					game::Scr_ErrorInternal();
				}
			};

			hook.create(reinterpret_cast<void*>(address), stub);
		}

		void script_link_error()
		{
			if (game::environment::is_sp())
			{
				game::Com_Error(game::ERR_SCRIPT_DROP, "script link error\n%s", unknown_function_error.data());
				return;
			}

			// Com_Error causes problems when it runs ingame (SV_Shutdown -> archive time), disconnect first
			command::execute("disconnect");
			scheduler::once([]
			{
				game::Com_Error(game::ERR_SCRIPT_DROP, "script link error\n%s", unknown_function_error.data());
			});
		}

		void scr_emit_function_stub(std::uint32_t filename, std::uint32_t thread_name, char* code_pos)
		{
			current_filename = filename;
			scr_emit_function_hook.invoke<void>(filename, thread_name, code_pos);
		}

		std::string get_filename_name()
		{
			const auto filename_str = game::SL_ConvertToString(static_cast<game::scr_string_t>(current_filename));
			const auto id = std::atoi(filename_str);
			if (!id)
			{
				return filename_str;
			}

			return scripting::get_token(id);
		}

		void get_unknown_function_error(const char* code_pos)
		{
			const auto function = find_function(code_pos);
			if (function.has_value())
			{
				const auto& pos = function.value();
				unknown_function_error = std::format(
					"while processing function '{}' in script '{}':\nunknown script '{}' ({})", 
					pos.first, pos.second, scripting::current_file, scripting::current_file_id
				);
			}
			else
			{
				unknown_function_error = std::format("unknown script '{}' ({})", 
					scripting::current_file, scripting::current_file_id);
			}
		}

		void get_unknown_function_error(std::uint32_t thread_name)
		{
			const auto filename = get_filename_name();
			const auto name = scripting::get_token(thread_name);

			unknown_function_error = std::format(
				"while processing script '{}':\nunknown function '{}::{}'", 
				scripting::current_file, filename, name
			);
		}

		void compile_error_stub(const char* code_pos, const char* msg)
		{
			get_unknown_function_error(code_pos);
			script_link_error();
		}

		std::uint32_t find_variable_stub(std::uint32_t parent_id, std::uint32_t thread_name)
		{
			const auto res = game::FindVariable(parent_id, thread_name);
			if (!res)
			{
				get_unknown_function_error(thread_name);
				script_link_error();
			}
			return res;
		}

		unsigned int scr_get_object(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				auto* value = game::scr_VmPub->top - index;
				if (value->type == game::VAR_POINTER)
				{
					return value->u.pointerValue;
				}

				script_extension::scr_error(va("Type %s is not an object", var_typename[value->type]));
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		unsigned int scr_get_const_string(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				auto* value = game::scr_VmPub->top - index;
				if (game::Scr_CastString(value))
				{
					assert(value->type == game::VAR_STRING);
					return value->u.stringValue;
				}

				game::Scr_ErrorInternal();
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		unsigned int scr_get_const_istring(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				auto* value = game::scr_VmPub->top - index;
				if (value->type == game::VAR_ISTRING)
				{
					return value->u.stringValue;
				}

				script_extension::scr_error(va("Type %s is not a localized string", var_typename[value->type]));
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		void scr_validate_localized_string_ref(int parm_index, const char* token, int token_len)
		{
			assert(token);
			assert(token_len >= 0);

			if (token_len < 2)
			{
				return;
			}

			for (auto char_iter = 0; char_iter < token_len; ++char_iter)
			{
				if (!std::isalnum(static_cast<unsigned char>(token[char_iter])) && token[char_iter] != '_')
				{
					script_extension::scr_error(va("Illegal localized string reference: %s must contain only alpha-numeric characters and underscores", token));
				}
			}
		}

		void scr_get_vector(unsigned int index, float* vector_value)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				auto* value = game::scr_VmPub->top - index;
				if (value->type == game::VAR_VECTOR)
				{
					std::memcpy(vector_value, value->u.vectorValue, sizeof(std::float_t[3]));
					return;
				}

				script_extension::scr_error(va("Type %s is not a vector", var_typename[value->type]));
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
		}

		int scr_get_int(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				auto* value = game::scr_VmPub->top - index;
				if (value->type == game::VAR_INTEGER)
				{
					return value->u.intValue;
				}

				script_extension::scr_error(va("Type %s is not an int", var_typename[value->type]));
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		float scr_get_float(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				auto* value = game::scr_VmPub->top - index;
				if (value->type == game::VAR_FLOAT)
				{
					return value->u.floatValue;
				}

				if (value->type == game::VAR_INTEGER)
				{
					return static_cast<float>(value->u.intValue);
				}

				script_extension::scr_error(va("Type %s is not a float", var_typename[value->type]));
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return 0.0f;
		}

		int scr_get_pointer_type(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				if ((game::scr_VmPub->top - index)->type == game::VAR_POINTER)
				{
					return static_cast<int>(game::GetObjectType((game::scr_VmPub->top - index)->u.uintValue));
				}

				script_extension::scr_error(va("Type %s is not an object", var_typename[(game::scr_VmPub->top - index)->type]));
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		int scr_get_type(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				return (game::scr_VmPub->top - index)->type;
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		const char* scr_get_type_name(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				return var_typename[(game::scr_VmPub->top - index)->type];
			}

			script_extension::scr_error(va("Parameter %u does not exist", index + 1));
			return nullptr;
		}
	}

	std::optional<std::pair<std::string, std::string>> find_function(const char* pos)
	{
		for (const auto& file : scripting::script_function_table_sort)
		{
			for (auto i = file.second.begin(); i != file.second.end() && std::next(i) != file.second.end(); ++i)
			{
				const auto next = std::next(i);
				if (pos >= i->second && pos < next->second)
				{
					return {std::make_pair(i->first, file.first)};
				}
			}
		}

		return {};
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			scr_emit_function_hook.create(SELECT_VALUE(0x1403BD680, 0x140437B00), &scr_emit_function_stub);

			utils::hook::call(SELECT_VALUE(0x1403BD626, 0x140437AA6), compile_error_stub); // CompileError (LinkFile)
			utils::hook::call(SELECT_VALUE(0x1403BD672, 0x140437AF2), compile_error_stub); // ^
			utils::hook::call(SELECT_VALUE(0x1403BD75A, 0x140437BDA), find_variable_stub); // Scr_EmitFunction

			// Restore basic error messages for commonly used scr functions
			utils::hook::jump(SELECT_VALUE(0x1403C89F0, 0x140442E80), scr_get_object);
			utils::hook::jump(SELECT_VALUE(0x1403C84C0, 0x140442A00), scr_get_const_string);
			utils::hook::jump(SELECT_VALUE(0x1403C8280, 0x1404427C0), scr_get_const_istring);
			utils::hook::jump(SELECT_VALUE(0x1402D6950, 0x1403746F0), scr_validate_localized_string_ref);
			utils::hook::jump(SELECT_VALUE(0x1403C8F30, 0x1404433C0), scr_get_vector);
			utils::hook::jump(SELECT_VALUE(0x1403C8930, 0x140442DC0), scr_get_int);
			utils::hook::jump(SELECT_VALUE(0x1403C87D0, 0x140442D10), scr_get_float);

			utils::hook::jump(SELECT_VALUE(0x1403C8C10, 0x1404430A0), scr_get_pointer_type);
			utils::hook::jump(SELECT_VALUE(0x1403C8DE0, 0x140443270), scr_get_type);
			utils::hook::jump(SELECT_VALUE(0x1403C8E50, 0x1404432E0), scr_get_type_name);

			if (!game::environment::is_sp())
			{
				safe_func<0x140376DC0>(); // fix vlobby cac crash
			}
		}

		void pre_destroy() override
		{
			scr_emit_function_hook.clear();
		}
	};
}

REGISTER_COMPONENT(script_error::component)
