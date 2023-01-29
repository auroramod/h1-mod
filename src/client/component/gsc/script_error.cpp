#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "script_error.hpp"

#include "component/scripting.hpp"

#include <utils/hook.hpp>

namespace gsc
{
	namespace
	{
		utils::hook::detour scr_emit_function_hook;

		std::uint32_t current_filename = 0;

		std::string unknown_function_error;

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
					"while processing function '{}' in script '{}':\nunknown script '{}'", 
					pos.first, pos.second, scripting::current_file
				);
			}
			else
			{
				unknown_function_error = std::format("unknown script '{}'", scripting::current_file);
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

		void unknown_function_stub(const char* code_pos)
		{
			get_unknown_function_error(code_pos);
			game::Com_Error(game::ERR_DROP, "script link error\n%s",
				unknown_function_error.data());
		}

		std::uint32_t find_variable_stub(std::uint32_t parent_id, std::uint32_t thread_name)
		{
			const auto res = game::FindVariable(parent_id, thread_name);
			if (!res)
			{
				get_unknown_function_error(thread_name);
				game::Com_Error(game::ERR_DROP, "script link error\n%s",
					unknown_function_error.data());
			}
			return res;
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

			utils::hook::call(SELECT_VALUE(0x3BD626_b, 0x504606_b), unknown_function_stub); // CompileError (LinkFile)
			utils::hook::call(SELECT_VALUE(0x3BD672_b, 0x504652_b), unknown_function_stub); // ^
			utils::hook::call(SELECT_VALUE(0x3BD75A_b, 0x50473A_b), find_variable_stub);	// Scr_EmitFunction

			safe_func<0xBA7A0>(); // fix vlobby cac crash
		}

		void pre_destroy() override
		{
			scr_emit_function_hook.clear();
		}
	};
}

REGISTER_COMPONENT(gsc::error)
