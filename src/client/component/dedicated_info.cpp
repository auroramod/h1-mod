#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/string.hpp>

namespace dedicated_info
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			scheduler::loop([]
			{
				const auto sv_running = game::Dvar_FindVar("sv_running");
				if (!sv_running || !sv_running->current.enabled || (*game::mp::svs_clients) == nullptr)
				{
					SetConsoleTitle("H1-Mod Dedicated Server");
					return;
				}

				const auto sv_hostname = game::Dvar_FindVar("sv_hostname");
				const auto sv_maxclients = game::Dvar_FindVar("sv_maxclients");
				const auto mapname = game::Dvar_FindVar("mapname");

				auto bot_count = 0;
				auto client_count = 0;

				const auto svs_clients = *game::mp::svs_clients;

				for (auto i = 0; i < *game::mp::svs_numclients; i++)
				{
					const auto client = svs_clients[i];
					auto* self = &game::mp::g_entities[i];

					if (client.header.state >= 1 && self && self->client)
					{
						client_count++;
						if (game::SV_BotIsBot(i))
						{
							++bot_count;
						}
					}
				}

				std::string cleaned_hostname;
				cleaned_hostname.resize(static_cast<int>(strlen(sv_hostname->current.string) + 1));

				utils::string::strip(sv_hostname->current.string, cleaned_hostname.data(),
				static_cast<int>(strlen(sv_hostname->current.string)) + 1);

				SetConsoleTitle(utils::string::va("%s on %s [%d/%d] (%d)", cleaned_hostname.data(),
					mapname->current.string, client_count,
					sv_maxclients->current.integer, bot_count)
				);
			}, scheduler::pipeline::main, 1s);
		}
	};
}

REGISTER_COMPONENT(dedicated_info::component)