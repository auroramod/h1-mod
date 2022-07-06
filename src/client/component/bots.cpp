#include <std_include.hpp>
#include "loader/component_loader.hpp"

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

namespace bots
{
	namespace
	{
		bool can_add()
		{
			return party::get_client_count() < *game::mp::svs_numclients
				&& game::SV_Loaded() && !game::VirtualLobby_Loaded();
		}

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

			const auto* const bot_name = game::SV_BotGetRandomName();
			const auto* bot_ent = game::SV_AddBot(bot_name);

			if (bot_ent)
			{
				spawn_bot(bot_ent->s.number);
			}
			else
			{
				scheduler::once([]()
				{
					add_bot();
				}, scheduler::pipeline::server, 100ms);
			}
		}

		utils::hook::detour get_bot_name_hook;
		std::vector<std::string> bot_names{};

		void load_bot_data()
		{
			static const char* bots_json = "h1-mod/bots.json";

			std::string bots_content;

			if (!utils::io::read_file(bots_json, &bots_content))
			{
				console::error(utils::string::va("%s was not found.\n", bots_json));
				return;
			}

			rapidjson::Document bots;
			bots.Parse(bots_content.data());

			// must be in a ["name1, "name2", "name3"] format
			if (!bots.IsArray())
			{
				console::error("bots.json must be a array of names.\n");
				return;
			}

			const auto names = bots.GetArray();
			for (const auto& name : names)
			{
				bot_names.emplace_back(std::string(name.GetString()));
			}
		}

		static size_t bot_id = 0;

		const char* get_random_bot_name()
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

			get_bot_name_hook.create(game::SV_BotGetRandomName, get_random_bot_name);

			command::add("spawnBot", [](const command::params& params)
			{
				if (!can_add())
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
					scheduler::once(add_bot, scheduler::pipeline::server, 100ms * i);
				}
			});

			// Clear bot names and reset ID on game shutdown to allow new names to be added without restarting
			scripting::on_shutdown([]
			{
				bot_names.clear();
				bot_id = 0;
			});
		}
	};
}

REGISTER_COMPONENT(bots::component)
