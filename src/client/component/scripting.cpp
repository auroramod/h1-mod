#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include <utils/hook.hpp>

#include "game/scripting/entity.hpp"
#include "game/scripting/functions.hpp"
#include "game/scripting/event.hpp"
#include "game/scripting/lua/engine.hpp"
#include "game/scripting/execution.hpp"

#include "scheduler.hpp"
#include "scripting.hpp"

namespace scripting
{
	std::unordered_map<int, std::unordered_map<std::string, int>> fields_table;
	std::unordered_map<std::string, std::unordered_map<std::string, const char*>> script_function_table;

	namespace
	{
		utils::hook::detour vm_notify_hook;
		utils::hook::detour scr_load_level_hook;
		utils::hook::detour g_shutdown_game_hook;

		utils::hook::detour scr_add_class_field_hook;

		utils::hook::detour scr_set_thread_position_hook;
		utils::hook::detour process_script_hook;

		std::string current_file;

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

					if (e.name == "entitydeleted")
					{
						scripting::clear_entity_fields(e.entity);
					}

					lua::engine::notify(e);
				}
			}

			vm_notify_hook.invoke<void>(notify_list_owner_id, string_value, top);
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
			lua::engine::stop();
			return g_shutdown_game_hook.invoke<void>(free_scripts);
		}

		void scr_add_class_field_stub(unsigned int classnum, game::scr_string_t _name, unsigned int canonicalString, unsigned int offset)
		{
			const auto name = game::SL_ConvertToString(_name);

			if (fields_table[classnum].find(name) == fields_table[classnum].end())
			{
				fields_table[classnum][name] = offset;
			}

			scr_add_class_field_hook.invoke<void>(classnum, _name, canonicalString, offset);
		}

		void process_script_stub(const char* filename)
		{
			const auto file_id = atoi(filename);
			if (file_id)
			{
				current_file = scripting::find_token(file_id);
			}
			else
			{
				current_file = filename;
			}

			process_script_hook.invoke<void>(filename);
		}

		void scr_set_thread_position_stub(unsigned int threadName, const char* codePos)
		{
			const auto function_name = scripting::find_token(threadName);
			script_function_table[current_file][function_name] = codePos;
			scr_set_thread_position_hook.invoke<void>(threadName, codePos);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			vm_notify_hook.create(SELECT_VALUE(0x140379A00, 0x1404479F0), vm_notify_stub);

			scr_add_class_field_hook.create(SELECT_VALUE(0x140370370, 0x14043E2C0), scr_add_class_field_stub);

			scr_set_thread_position_hook.create(SELECT_VALUE(0x14036A180, 0x140437D10), scr_set_thread_position_stub);
			process_script_hook.create(SELECT_VALUE(0x1403737E0, 0x1404417E0), process_script_stub);

			scr_load_level_hook.create(0x1403727C0, scr_load_level_stub);
			g_shutdown_game_hook.create(0x140345A60, g_shutdown_game_stub);

			scheduler::loop([]()
			{
				lua::engine::run_frame();
			}, scheduler::pipeline::server);
		}
	};
}

REGISTER_COMPONENT(scripting::component)