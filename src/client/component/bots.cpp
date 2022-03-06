#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "network.hpp"
#include "party.hpp"

#include "game/game.hpp"
#include "game/scripting/execution.hpp"

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
			const game::scr_entref_t entref{static_cast<uint16_t>(entity_num), 0};
			scheduler::once([entref]()
			{
				scripting::notify(entref, "luinotifyserver", {"team_select", 2});
				scheduler::once([entref]()
				{
					auto* _class = utils::string::va("class%d", utils::cryptography::random::get_integer() % 5);
					scripting::notify(entref, "luinotifyserver", {"class_select", _class});
				}, scheduler::pipeline::server, 2s);
			}, scheduler::pipeline::server, 2s);
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

				num_bots = std::min(num_bots, *game::mp::svs_numclients);;

				for (auto i = 0; i < num_bots; i++)
				{
					scheduler::once(add_bot, scheduler::pipeline::server, 100ms * i);
				}
			});
		}
	};
}

REGISTER_COMPONENT(bots::component)