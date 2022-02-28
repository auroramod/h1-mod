#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "network.hpp"
#include "party.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/cryptography.hpp>

namespace bots
{
	namespace
	{
		bool can_add()
		{
			if (party::get_client_count() < *game::mp::svs_numclients)
			{
				return true;
			}
			return false;
		}

		// TODO: when scripting comes, fix this to use better notifies
		void bot_team_join(const int entity_num)
		{
			scheduler::once([entity_num]()
			{
				game::SV_ExecuteClientCommand(&game::mp::svs_clients[entity_num],
				                              utils::string::va("lui 68 2 %i", *game::mp::sv_serverId_value),
				                              false);

				// scheduler the select class call
				scheduler::once([entity_num]()
				{
					game::SV_ExecuteClientCommand(&game::mp::svs_clients[entity_num],
					                              utils::string::va("lui 5 %i %i", (rand() % 5) + 10,
					                                                *game::mp::sv_serverId_value), false);
				}, scheduler::pipeline::server, 1s);
			}, scheduler::pipeline::server, 1s);
		}

		void spawn_bot(const int entity_num)
		{
			game::SV_SpawnTestClient(&game::mp::g_entities[entity_num]);
			if (game::Com_GetCurrentCoDPlayMode() == game::CODPLAYMODE_CORE)
			{
				bot_team_join(entity_num);
			}
		}

		void add_bot()
		{
			if (!can_add())
			{
				return;
			}

			// SV_BotGetRandomName
			const auto* const bot_name = game::SV_BotGetRandomName();
			auto* bot_ent = game::SV_AddBot(bot_name);
			if (bot_ent)
			{
				spawn_bot(bot_ent->s.entityNum);
			}
			else if (can_add()) // workaround since first bot won't ever spawn
			{
				add_bot();
			}
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

			command::add("spawnBot", [](const command::params& params)
			{
				if (!game::SV_Loaded() || game::VirtualLobby_Loaded()) return;

				auto num_bots = 1;
				if (params.size() == 2)
				{
					num_bots = atoi(params.get(1));
				}

				for (auto i = 0; i < (num_bots > *game::mp::svs_numclients ? *game::mp::svs_numclients : num_bots); i++)
				{
					scheduler::once(add_bot, scheduler::pipeline::server, 100ms * i);
				}
			});
		}
	};
}

REGISTER_COMPONENT(bots::component)