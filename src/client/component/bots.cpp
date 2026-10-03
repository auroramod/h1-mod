#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "bots.hpp"

#include "command.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "network.hpp"
#include "party.hpp"
#include "scripting.hpp"

#include "game/game.hpp"
#include "game/scripting/execution.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/cryptography.hpp>
#include <utils/io.hpp>

static utils::hook::detour get_bot_name_hook;
static std::vector<std::string> bot_names{};
static size_t bot_id = 0;

void bots::post_unpack()
{
	if (game::environment::is_sp())
	{
		return;
	}

	get_bot_name_hook.create(game::SV_BotGetRandomName, get_random_bot_name);

	command::add("spawnBot", [](const command::params& params)
	{
		if (!bots::can_add())
		{
			return;
		}

		auto num_bots = 1;
		if (params.size() == 2)
		{
			num_bots = atoi(params.get(1));
		}

		num_bots = std::min(num_bots, *game::mp::svs_numclients);

		for (auto i = 0; i < num_bots; i++)
		{
			scheduler::once(bots::add, scheduler::pipeline::server, 100ms * i);
		}
	});

	// Clear bot names and reset ID on game shutdown to allow new names to be added without restarting
	scripting::on_shutdown([](bool /*free_scripts*/, bool post_shutdown)
	{
		if (!post_shutdown)
		{
			bot_names.clear();
			bot_id = 0;
		}
	});
}

bool bots::can_add()
{
	return party::get_client_count() < *game::mp::svs_numclients
		&& game::SV_Loaded() && !game::VirtualLobby_Loaded();
}

void bots::join_team(const int entity_num)
{
	const game::scr_entref_t entref{static_cast<uint16_t>(entity_num), 0};
	scheduler::once([entref]
	{
		scripting::notify(entref, "luinotifyserver", {"team_select", 2});
		scheduler::once([entref]
		{
			auto* _class = utils::string::va("class%d", utils::cryptography::random::get_integer() % 5);
			scripting::notify(entref, "luinotifyserver", {"class_select", _class});
		}, scheduler::pipeline::server, 2s);
	}, scheduler::pipeline::server, 2s);
}

void bots::spawn(const int entity_num)
{
	game::SV_SpawnTestClient(&game::mp::g_entities[entity_num]);
	if (game::Com_GetCurrentCoDPlayMode() == game::CODPLAYMODE_CORE)
	{
		bots::join_team(entity_num);
	}
}

void bots::add()
{
	if (!can_add())
	{
		return;
	}

	const auto* const bot_name = game::SV_BotGetRandomName();

	if (const auto* bot_ent = game::SV_AddBot(bot_name))
	{
		bots::spawn(bot_ent->s.number);
	}
	else
	{
		scheduler::once([]
		{
			bots::add();
		}, scheduler::pipeline::server, 100ms);
	}
}

void bots::load_bot_data()
{
	static const char* bots_txt = "h1-mod/bots.txt";

	std::string bots_content;
	if (!utils::io::read_file(bots_txt, &bots_content))
	{
		return;
	}

	auto names = utils::string::split(bots_content, '\n');
	for (auto& name : names)
	{
		name = utils::string::replace(name, "\r", "");
		if (!name.empty())
		{
			bot_names.emplace_back(name);
		}
	}
}

const char* bots::get_random_bot_name()
{
	if (bot_names.empty())
	{
		load_bot_data();
	}

	// only use bot names once, no dupes in names
	if (!bot_names.empty() && bot_id < bot_names.size())
	{
		bot_id %= bot_names.size();
		const auto& entry = bot_names.at(bot_id++);
		return utils::string::va("%.*s", static_cast<int>(entry.size()), entry.data());
	}

	return get_bot_name_hook.invoke<const char*>();
}

REGISTER_COMPONENT(bots)
