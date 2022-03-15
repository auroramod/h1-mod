#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"
#include "network.hpp"
#include "party.hpp"

#include <utils/string.hpp>
#include <utils/cryptography.hpp>

#include <discord_rpc.h>

namespace discord
{
	namespace
	{
		DiscordRichPresence discord_presence;

		void update_discord()
		{
			Discord_RunCallbacks();

			if (!game::CL_IsCgameInitialized() || game::VirtualLobby_Loaded())
			{
				discord_presence.details = game::environment::is_sp() ? "Singleplayer" : "Multiplayer";
				discord_presence.state = "Main Menu";

				auto firingRangeDvar = game::Dvar_FindVar("virtualLobbyInFiringRange");
				if (firingRangeDvar && firingRangeDvar->current.enabled == 1)
				{
					discord_presence.state = "Firing Range";
				}

				discord_presence.partySize = 0;
				discord_presence.partyMax = 0;
				discord_presence.startTimestamp = 0;
				discord_presence.largeImageKey = game::environment::is_sp() ? "menu_singleplayer" : "menu_multiplayer";
			
				// set to blank when in lobby
				discord_presence.matchSecret = "";
				discord_presence.joinSecret = "";
				discord_presence.partyId = "";
				discord_presence.state = "";
			}
			else
			{
				const auto map = game::Dvar_FindVar("mapname")->current.string;
				const auto mapname = game::UI_SafeTranslateString(utils::string::va("PRESENCE_%s%s", (game::environment::is_sp() ? "SP_" : ""), map));

				if (game::environment::is_mp())
				{
					const auto gametype = game::UI_GetGameTypeDisplayName(game::Dvar_FindVar("g_gametype")->current.string);

					discord_presence.details = utils::string::va("%s on %s", gametype, mapname);

					char clean_hostname[0x100] = {0};
					utils::string::strip(game::Dvar_FindVar("sv_hostname")->current.string, 
						clean_hostname, sizeof(clean_hostname));
					auto max_clients = party::server_client_count();

					// When true, we are in Private Match
					if (game::SV_Loaded())
					{
						strcpy_s(clean_hostname, "Private Match");
						max_clients = game::Dvar_FindVar("sv_maxclients")->current.integer;
						discord_presence.partyPrivacy = DISCORD_PARTY_PRIVATE;
					}
					else
					{
						discord_presence.partyPrivacy = DISCORD_PARTY_PUBLIC;

						// TODO: we need to make this a random string that represents the session ID
						// const auto sessionId = party::get_state_challenge();
						discord_presence.partyId = "PLACEHOLDER";

						const auto server_net_info = party::get_state_host();
						const auto server_ip_port = utils::string::va("%i.%i.%i.%i:%i",
							server_net_info.ip[0], server_net_info.ip[1], server_net_info.ip[2], server_net_info.ip[3],
							ntohs(server_net_info.port));

						discord_presence.joinSecret = server_ip_port;
					}

					discord_presence.partySize = *reinterpret_cast<int*>(0x1429864C4);
					discord_presence.partyMax = max_clients;
					discord_presence.state = clean_hostname;
					discord_presence.largeImageKey = map;
				}
				else if (game::environment::is_sp())
				{
					discord_presence.state = "";
					discord_presence.largeImageKey = map;
					discord_presence.details = mapname;
				}

				if (!discord_presence.startTimestamp)
				{
					discord_presence.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
						std::chrono::system_clock::now().time_since_epoch()).count();
				}
			}

			Discord_UpdatePresence(&discord_presence);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			DiscordEventHandlers handlers;
			ZeroMemory(&handlers, sizeof(handlers));
			handlers.ready = ready;
			handlers.errored = errored;
			handlers.disconnected = errored;
			handlers.joinGame = joinGame;
			handlers.spectateGame = nullptr;
			handlers.joinRequest = joinRequest;

			Discord_Initialize("947125042930667530", &handlers, 1, nullptr);

			scheduler::once([]()
			{
				scheduler::once(update_discord, scheduler::pipeline::async);
				scheduler::loop(update_discord, scheduler::pipeline::async, 5s);
			}, scheduler::pipeline::main);

			initialized_ = true;
		}

		void pre_destroy() override
		{
			if (!initialized_ || game::environment::is_dedi())
			{
				return;
			}

			Discord_Shutdown();
		}

	private:
		bool initialized_ = false;

		static void ready(const DiscordUser* request)
		{
			ZeroMemory(&discord_presence, sizeof(discord_presence));

			discord_presence.instance = 1;

			console::info("Discord: Ready on %s (%s)\n", request->username, request->userId);

			Discord_UpdatePresence(&discord_presence);
		}

		static void errored(const int error_code, const char* message)
		{
			console::error("Discord: Error (%i): %s\n", error_code, message);
		}

		static void joinGame(const char* joinSecret)
		{
			console::info("Discord: Join game called with join secret: %s\n", joinSecret);

			scheduler::once([joinSecret]()
			{
				game::netadr_s target{};
				if (game::NET_StringToAdr(joinSecret, &target))
				{
					console::info("Discord: Connecting to server: %s\n", joinSecret);
					party::connect(target);
				}
			}, scheduler::pipeline::main);
		}

		static void joinRequest(const DiscordUser* request)
		{
			console::info("Discord: joinRequest from %s (%s)\n", request->username, request->userId);
			// Discord_Respond(request->userId, DISCORD_REPLY_YES);
		}
	};
}

REGISTER_COMPONENT(discord::component)