#include <std_include.hpp>
#include "lui.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>

static uint64_t event_count{};
static uint64_t obituary_count{};

void lui::post_unpack()
{
	if (game::environment::is_dedi())
	{
		return;
	}

	if (game::environment::is_mp())
	{
		// Patch game message overflow
		utils::hook::call(0x14016324B, begin_game_message_event_stub);
		utils::hook::call(0x1400A124F, cg_entity_event_stub);

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

bool lui::begin_game_message_event_stub(int a1, const char* name, void* a3)
{
	if (event_count > 30)
	{
		return false;
	}
	else
	{
		event_count++;
	}

	return utils::hook::invoke<bool>(0x140161A00, a1, name, a3);
}

void lui::cg_entity_event_stub(void* a1, void* a2, unsigned int event_type, void* a4)
{
	if (event_type == 140 && obituary_count++ >= 20)
	{
		return;
	}

	utils::hook::invoke<void>(0x1400ACB60, a1, a2, event_type, a4);
}

REGISTER_COMPONENT(lui)
