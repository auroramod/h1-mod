#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "script_extension.hpp"
#include "script_error.hpp"

#include "component/scripting.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

using namespace utils::string;

namespace gsc
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

		void compile_error_stub(const char* code_pos, [[maybe_unused]] const char* msg)
		{
			get_unknown_function_error(code_pos);
			game::Com_Error(game::ERR_SCRIPT_DROP, "script link error\n%s", unknown_function_error.data());
		}
		
		std::uint32_t find_variable_stub(std::uint32_t parent_id, std::uint32_t thread_name)
		{
			const auto res = game::FindVariable(parent_id, thread_name);
			if (!res)
			{
				get_unknown_function_error(thread_name);
				game::Com_Error(game::ERR_SCRIPT_DROP, "script link error\n%s", unknown_function_error.data());
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

				scr_error(va("Type %s is not an object", var_typename[value->type]));
			}

			scr_error(va("Parameter %u does not exist", index + 1));
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

			scr_error(va("Parameter %u does not exist", index + 1));
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

				scr_error(va("Type %s is not a localized string", var_typename[value->type]));
			}

			scr_error(va("Parameter %u does not exist", index + 1));
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
					scr_error(va("Illegal localized string reference: %s must contain only alpha-numeric characters and underscores", token));
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

				scr_error(va("Type %s is not a vector", var_typename[value->type]));
			}

			scr_error(va("Parameter %u does not exist", index + 1));
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

				scr_error(va("Type %s is not an int", var_typename[value->type]));
			}

			scr_error(va("Parameter %u does not exist", index + 1));
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

				scr_error(va("Type %s is not a float", var_typename[value->type]));
			}

			scr_error(va("Parameter %u does not exist", index + 1));
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

				scr_error(va("Type %s is not an object", var_typename[(game::scr_VmPub->top - index)->type]));
			}

			scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		int scr_get_type(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				return (game::scr_VmPub->top - index)->type;
			}

			scr_error(va("Parameter %u does not exist", index + 1));
			return 0;
		}

		const char* scr_get_type_name(unsigned int index)
		{
			if (index < game::scr_VmPub->outparamcount)
			{
				return var_typename[(game::scr_VmPub->top - index)->type];
			}

			scr_error(va("Parameter %u does not exist", index + 1));
			return nullptr;
		}

		template <size_t rva>
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

			const auto ptr = rva + 0_b;
			hook.create(reinterpret_cast<void*>(ptr), stub);
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

	class error final : public component_interface
	{
	public:
		void post_unpack() override
		{
			scr_emit_function_hook.create(SELECT_VALUE(0x3BD680_b, 0x504660_b), &scr_emit_function_stub);

			utils::hook::call(SELECT_VALUE(0x3BD626_b, 0x504606_b), compile_error_stub); // CompileError (LinkFile)
			utils::hook::call(SELECT_VALUE(0x3BD672_b, 0x504652_b), compile_error_stub); // ^
			utils::hook::call(SELECT_VALUE(0x3BD75A_b, 0x50473A_b), find_variable_stub); // Scr_EmitFunction

			// Restore basic error messages for commonly used scr functions
			utils::hook::jump(SELECT_VALUE(0x3C89F0_b, 0x50F9E0_b), scr_get_object);
			utils::hook::jump(SELECT_VALUE(0x3C84C0_b, 0x50F560_b), scr_get_const_string);
			utils::hook::jump(SELECT_VALUE(0x3C8280_b, 0x50F320_b), scr_get_const_istring);
			utils::hook::jump(SELECT_VALUE(0x2D6950_b, 0x452EF0_b), scr_validate_localized_string_ref);
			utils::hook::jump(SELECT_VALUE(0x3C8F30_b, 0x50FF20_b), scr_get_vector);
			utils::hook::jump(SELECT_VALUE(0x3C8930_b, 0x50F920_b), scr_get_int);
			utils::hook::jump(SELECT_VALUE(0x3C87D0_b, 0x50F870_b), scr_get_float);

			utils::hook::jump(SELECT_VALUE(0x3C8C10_b, 0x50FC00_b), scr_get_pointer_type);
			utils::hook::jump(SELECT_VALUE(0x3C8DE0_b, 0x50FDD0_b), scr_get_type);
			utils::hook::jump(SELECT_VALUE(0x3C8E50_b, 0x50FE40_b), scr_get_type_name);

			if (!game::environment::is_sp())
			{
				safe_func<0xBA7A0>(); // fix vlobby cac crash
			}
		}

		void pre_destroy() override
		{
			scr_emit_function_hook.clear();
		}
	};
}

REGISTER_COMPONENT(gsc::error)
