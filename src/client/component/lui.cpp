#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>

namespace lui
{
	namespace
	{
		uint64_t event_count{};
		uint64_t obituary_count{};

		bool begin_game_message_event_stub(int a1, const char* name, void* a3)
		{
			if (event_count > 30)
			{
				return false;
			}
			else
			{
				event_count++;
			}

			return utils::hook::invoke<bool>(0x2655A0_b, a1, name, a3);
		}

		void cg_entity_event_stub(void* a1, void* a2, unsigned int event_type, void* a4)
		{
			if (event_type == 140 && obituary_count++ >= 20)
			{
				return;
			}

			utils::hook::invoke<void>(0xF9400_b, a1, a2, event_type, a4);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_mp())
			{
				// Patch game message overflow
				utils::hook::call(0x266E6B_b, begin_game_message_event_stub);
				utils::hook::call(0xEAC1C_b, cg_entity_event_stub);

				scheduler::loop([]()
				{
					if (event_count > 0)
					{
						event_count--;
					}
				}, scheduler::pipeline::lui, 50ms);

				scheduler::loop([]()
				{
					obituary_count = 0;
				}, scheduler::pipeline::lui, 0ms);
			}

			// Increase max extra LUI memory
			/*const auto max_memory = 0x900000 * 2;
			utils::hook::set<uint32_t>(0x278E61_b - 4, max_memory);
			utils::hook::set<uint32_t>(0x27A2C5_b - 4, max_memory);
			utils::hook::set<uint32_t>(0x27A993_b - 4, max_memory);
			utils::hook::set<uint32_t>(0x27AB3A_b - 4, max_memory);
			utils::hook::set<uint32_t>(0x27AB35_b - 4, max_memory);
			utils::hook::set<uint32_t>(0x27C002_b - 4, max_memory);*/

			// Increase max extra frontend memory
			/*const auto max_frontend_memory = 0x180000 * 2;
			utils::hook::set<uint32_t>(0x278EA6_b - 4, max_frontend_memory);
			utils::hook::set<uint32_t>(0x278F01_b - 4, max_frontend_memory);
			utils::hook::set<uint32_t>(0x27A2D4_b - 4, max_frontend_memory);
			utils::hook::set<uint32_t>(0x27A2E3_b - 4, max_frontend_memory);
			utils::hook::set<uint32_t>(0x27F9E9_b - 4, max_frontend_memory);
			utils::hook::set<uint32_t>(0x27FA84_b - 4, max_frontend_memory);*/

			command::add("lui_open", [](const command::params& params)
			{
				if (params.size() <= 1)
				{
					console::info("usage: lui_open <name>\n");
					return;
				}

				game::LUI_OpenMenu(0, params[1], 0, 0, 0);
			});

			command::add("lui_close", [](const command::params& params)
			{
				if (params.size() <= 1)
				{
					console::info("usage: lui_close <name>\n");
					return;
				}

				game::LUI_LeaveMenuByName(0, params[1], 0, *game::hks::lua_state);
			});

			command::add("lui_open_popup", [](const command::params& params)
			{
				if (params.size() <= 1)
				{
					console::info("usage: lui_open_popup <name>\n");
					return;
				}

				game::LUI_OpenMenu(0, params[1], 1, 0, 0);
			});

			command::add("runMenuScript", [](const command::params& params)
			{
				const auto args_str = params.join(1);
				const auto* args = args_str.data();
				game::UI_RunMenuScript(0, &args);
			});
		}
	};
}

REGISTER_COMPONENT(lui::component)
