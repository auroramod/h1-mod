#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "game/scripting/entity.hpp"
#include "game/scripting/functions.hpp"
#include "game/scripting/event.hpp"
#include "game/scripting/lua/engine.hpp"
#include "game/scripting/execution.hpp"

#include "console.hpp"
#include "gsc.hpp"
#include "scheduler.hpp"
#include "scripting.hpp"

#include <xsk/gsc/types.hpp>
#include <xsk/resolver.hpp>
#include <xsk/utils/compression.hpp>

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace scripting
{
	std::unordered_map<int, std::unordered_map<std::string, int>> fields_table;
	std::unordered_map<std::string, std::unordered_map<std::string, const char*>> script_function_table;
	std::unordered_map<std::string, std::vector<std::pair<std::string, const char*>>> script_function_table_sort;
	utils::concurrency::container<shared_table_t> shared_table;

	std::string current_file;

	namespace
	{
		utils::hook::detour vm_notify_hook;
		utils::hook::detour vm_execute_hook;
		utils::hook::detour scr_load_level_hook;
		utils::hook::detour g_shutdown_game_hook;

		utils::hook::detour scr_add_class_field_hook;

		utils::hook::detour scr_set_thread_position_hook;
		utils::hook::detour process_script_hook;

		utils::hook::detour sl_get_canonical_string_hook;

		utils::hook::detour db_find_xasset_header_hook;

		std::string current_scriptfile;
		unsigned int current_file_id{};

		game::dvar_t* g_dump_scripts;

		std::vector<std::function<void(bool)>> shutdown_callbacks;

		std::unordered_map<unsigned int, std::string> canonical_string_table;

		void vm_notify_stub(const unsigned int notify_list_owner_id, const game::scr_string_t string_value,
			game::VariableValue* top)
		{
			if (!game::VirtualLobby_Loaded())
			{
				const auto* string = game::SL_ConvertToString(string_value);
				if (string)
				{
					event e;
					e.name = string;
					e.entity = notify_list_owner_id;

					for (auto* value = top; value->type != game::SCRIPT_END; --value)
					{
						e.arguments.emplace_back(*value);
					}

					lua::engine::notify(e);
				}
			}

			vm_notify_hook.invoke<void>(notify_list_owner_id, string_value, top);
		}

		unsigned int vm_execute_stub()
		{
			if (!lua::engine::is_running())
			{
				lua::engine::start();
			}

			return vm_execute_hook.invoke<unsigned int>();
		}

		void scr_load_level_stub()
		{
			scr_load_level_hook.invoke<void>();
			if (!game::VirtualLobby_Loaded())
			{
				lua::engine::start();
			}
		}

		void g_shutdown_game_stub(const int free_scripts)
		{
			if (free_scripts)
			{
				script_function_table_sort.clear();
				script_function_table.clear();
				canonical_string_table.clear();
			}

			for (const auto& callback : shutdown_callbacks)
			{
				callback(free_scripts);
			}

			scripting::notify(*game::levelEntityId, "shutdownGame_called", {1});
			lua::engine::stop();
			return g_shutdown_game_hook.invoke<void>(free_scripts);
		}

		void scr_add_class_field_stub(unsigned int classnum, game::scr_string_t name, unsigned int canonical_string, unsigned int offset)
		{
			const auto name_str = game::SL_ConvertToString(name);

			if (fields_table[classnum].find(name_str) == fields_table[classnum].end())
			{
				fields_table[classnum][name_str] = offset;
			}

			scr_add_class_field_hook.invoke<void>(classnum, name, canonical_string, offset);
		}

		void process_script_stub(const char* filename)
		{
			current_scriptfile = filename;
			
			const auto file_id = atoi(filename);
			if (file_id)
			{
				current_file_id = static_cast<std::uint16_t>(file_id);
			}
			else
			{
				current_file_id = 0;
				current_file = filename;
			}

			process_script_hook.invoke<void>(filename);
		}

		std::vector<std::string> get_token_names(unsigned int id)
		{
			auto result = scripting::find_token(id);

			if (canonical_string_table.find(id) != canonical_string_table.end())
			{
				result.push_back(canonical_string_table[id]);
			}

			return result;
		}

		std::string get_token_single(unsigned int id)
		{
			if (canonical_string_table.find(id) != canonical_string_table.end())
			{
				return canonical_string_table[id];
			}

			return xsk::gsc::h1::resolver::token_name(static_cast<std::uint16_t>(id));
		}

		void add_function_sort(unsigned int id, const char* pos)
		{
			std::string filename = current_file;
			if (current_file_id)
			{
				filename = get_token_single(current_file_id);
			}

			if (script_function_table_sort.find(filename) == script_function_table_sort.end())
			{
				const auto script = gsc::find_script(game::ASSET_TYPE_SCRIPTFILE, current_scriptfile.data(), false);
				if (script)
				{
					const auto end = &script->bytecode[script->bytecodeLen];
					script_function_table_sort[filename].emplace_back("__end__", end);
				}
			}

			const auto name = get_token_single(id);
			auto& itr = script_function_table_sort[filename];
			itr.insert(itr.end() - 1, { name, pos });
		}

		void add_function(const std::string& file, unsigned int id, const char* pos)
		{
			const auto function_names = get_token_names(id);
			for (const auto& name : function_names)
			{
				script_function_table[file][name] = pos;
			}
		}

		void scr_set_thread_position_stub(unsigned int thread_name, const char* code_pos)
		{
			add_function_sort(thread_name, code_pos);

			if (current_file_id)
			{
				const auto names = get_token_names(current_file_id);
				for (const auto& name : names)
				{
					add_function(name, thread_name, code_pos);
				}
			}
			else
			{
				add_function(current_file, thread_name, code_pos);
			}

			scr_set_thread_position_hook.invoke<void>(thread_name, code_pos);
		}

		unsigned int sl_get_canonical_string_stub(const char* str)
		{
			const auto result = sl_get_canonical_string_hook.invoke<unsigned int>(str);
			canonical_string_table[result] = str;
			return result;
		}
	}

	void on_shutdown(const std::function<void(bool)>& callback)
	{
		shutdown_callbacks.push_back(callback);
	}

	std::optional<std::string> get_canonical_string(const unsigned int id)
	{
		if (canonical_string_table.find(id) == canonical_string_table.end())
		{
			return {};
		}

		return {canonical_string_table[id]};
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			vm_notify_hook.create(SELECT_VALUE(0x3CD500_b, 0x514560_b), vm_notify_stub);

			scr_add_class_field_hook.create(SELECT_VALUE(0x3C3CE0_b, 0x50AE20_b), scr_add_class_field_stub);

			scr_set_thread_position_hook.create(SELECT_VALUE(0x3BD890_b, 0x504870_b), scr_set_thread_position_stub);
			process_script_hook.create(SELECT_VALUE(0x3C7200_b, 0x50E340_b), process_script_stub);
			sl_get_canonical_string_hook.create(game::SL_GetCanonicalString, sl_get_canonical_string_stub);

			if (!game::environment::is_sp())
			{
				scr_load_level_hook.create(0x450FC0_b, scr_load_level_stub);
			}
			else
			{
				vm_execute_hook.create(0x3CA080_b, vm_execute_stub);
			}

			g_shutdown_game_hook.create(SELECT_VALUE(0x2A5130_b, 0x422F30_b), g_shutdown_game_stub);

			scheduler::loop([]()
			{
				lua::engine::run_frame();
			}, scheduler::pipeline::server);
		}
	};
}

REGISTER_COMPONENT(scripting::component)
