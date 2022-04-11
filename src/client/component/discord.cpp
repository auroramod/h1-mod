#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"
#include "network.hpp"
#include "party.hpp"
#include "materials.hpp"

#include "ui_scripting.hpp"
#include "game/ui_scripting/execution.hpp"

#include <utils/string.hpp>
#include <utils/cryptography.hpp>
#include <utils/http.hpp>

#include <discord_rpc.h>

#define DEFAULT_AVATAR "discord_default_avatar"
#define AVATAR "discord_avatar_%s"

#define DEFAULT_AVATAR_URL "https://cdn.discordapp.com/embed/avatars/0.png"
#define AVATAR_URL "https://cdn.discordapp.com/avatars/%s/%s.png?size=128"

#include "discord.hpp"

namespace discord
{
	namespace
	{
		DiscordRichPresence discord_presence;

		void update_discord()
		{
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

					char clean_hostname[0x80] = {0};
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
						const auto server_net_info = party::get_state_host();
						const auto server_ip_port = utils::string::va("%i.%i.%i.%i:%i",
							server_net_info.ip[0], 
							server_net_info.ip[1], 
							server_net_info.ip[2], 
							server_net_info.ip[3],
							ntohs(server_net_info.port)
						);

						char party_id[0x80] = {0};
						const auto server_ip_port_hash = utils::cryptography::sha1::compute(server_ip_port, true);
						strcpy_s(party_id, 0x80, server_ip_port_hash.data());

						discord_presence.partyId = party_id;
						discord_presence.joinSecret = server_ip_port;
						discord_presence.partyPrivacy = DISCORD_PARTY_PUBLIC;
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

		void download_user_avatar(const std::string& id, const std::string& avatar)
		{
			const auto data = utils::http::get_data(
				utils::string::va(AVATAR_URL, id.data(), avatar.data()));
			if (data.has_value())
			{
				materials::add(utils::string::va(AVATAR, id.data()), data.value());
			}
		}

		bool has_default_avatar = false;
		void download_default_avatar()
		{
			const auto data = utils::http::get_data(DEFAULT_AVATAR_URL);
			if (data.has_value())
			{
				has_default_avatar = true;
				materials::add(DEFAULT_AVATAR, data.value());
			}
		}
	}

	std::string get_avatar_material(const std::string& id)
	{
		const auto avatar_name = utils::string::va(AVATAR, id.data());
		if (materials::exists(avatar_name))
		{
			return avatar_name;
		}

		if (has_default_avatar)
		{
			return DEFAULT_AVATAR;
		}

		return "black";
	}

	void respond(const std::string& id, int reply)
	{
		scheduler::once([=]()
		{
			Discord_Respond(id.data(), reply);
		}, scheduler::pipeline::async);
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
			handlers.joinGame = join_game;
			handlers.spectateGame = nullptr;
			handlers.joinRequest = join_request;

			Discord_Initialize("947125042930667530", &handlers, 1, nullptr);

			scheduler::once(download_default_avatar, scheduler::pipeline::async);

			scheduler::once([]()
			{
				scheduler::once(update_discord, scheduler::pipeline::async);
				scheduler::loop(update_discord, scheduler::pipeline::async, 5s);
				scheduler::loop(Discord_RunCallbacks, scheduler::pipeline::async, 1s);
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

		static void join_game(const char* join_secret)
		{
			console::info("Discord: Join game called with join secret: %s\n", join_secret);

			std::string secret = join_secret;
			scheduler::once([=]()
			{
				game::netadr_s target{};
				if (game::NET_StringToAdr(secret.data(), &target))
				{
					console::info("Discord: Connecting to server: %s\n", secret.data());
					party::connect(target);
				}
			}, scheduler::pipeline::main);
		}

		static void join_request(const DiscordUser* request)
		{
			console::info("Discord: join_request from %s (%s)\n", request->username, request->userId);

			if (game::Com_InFrontend() || !ui_scripting::lui_running())
			{
				Discord_Respond(request->userId, DISCORD_REPLY_IGNORE);
				return;
			}

			std::string user_id = request->userId;
			std::string avatar = request->avatar;
			std::string discriminator = request->discriminator;
			std::string username = request->username;

			scheduler::once([=]()
			{
				const ui_scripting::table request_table{};
				request_table.set("avatar", avatar);
				request_table.set("discriminator", discriminator);
				request_table.set("userid", user_id);
				request_table.set("username", username);

				ui_scripting::notify("discord_join_request",
				{
					{"request", request_table}
				});
			}, scheduler::pipeline::lui);

			if (!materials::exists(utils::string::va(AVATAR, user_id.data())))
			{
				download_user_avatar(user_id, avatar);
			}
		}
	};
}

REGISTER_COMPONENT(discord::component)