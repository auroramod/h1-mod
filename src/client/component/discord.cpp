#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "command.hpp"
#include "discord.hpp"
#include "materials.hpp"
#include "party.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/ui_scripting/execution.hpp"

#include <utils/string.hpp>
#include <utils/cryptography.hpp>
#include <utils/http.hpp>

#include <discord_rpc.h>

#define DEFAULT_AVATAR "discord_default_avatar"
#define AVATAR "discord_avatar_%s"

#define DEFAULT_AVATAR_URL "https://cdn.discordapp.com/embed/avatars/0.png"
#define AVATAR_URL "https://cdn.discordapp.com/avatars/%s/%s.png?size=128"

namespace discord
{
	namespace
	{
		struct discord_presence_state_t
		{
			int start_timestamp;
			int party_size;
			int party_max;
		};

		struct discord_presence_strings_t
		{
			std::string state;
			std::string details;
			std::string small_image_key;
			std::string small_image_text;
			std::string large_image_key;
			std::string large_image_text;
			std::string party_id;
			std::string join_secret;
		};

		DiscordRichPresence discord_presence{};
		discord_presence_strings_t discord_strings;

		std::mutex avatar_map_mutex;
		std::unordered_map<std::string, game::Material*> avatar_material_map;
		game::Material* default_avatar_material{};

		void update_discord_frontend()
		{
			discord_presence.details = SELECT_VALUE("Singleplayer", "Multiplayer");
			discord_presence.startTimestamp = 0;

			static const auto in_firing_range = game::Dvar_FindVar("virtualLobbyInFiringRange");
			if (in_firing_range != nullptr && in_firing_range->current.enabled == 1)
			{
				discord_presence.state = "Firing Range";
				discord_presence.largeImageKey = "mp_vlobby_room";
			}
			else
			{
				discord_presence.state = "Main Menu";
				discord_presence.largeImageKey = SELECT_VALUE("menu_singleplayer", "menu_multiplayer");
			}

			Discord_UpdatePresence(&discord_presence);
		}

		void update_discord_ingame()
		{
			static const auto mapname_dvar = game::Dvar_FindVar("mapname");
			auto mapname = mapname_dvar->current.string;

			discord_strings.large_image_key = mapname;

			const auto presence_key = utils::string::va("PRESENCE_%s%s", SELECT_VALUE("SP_", ""), mapname);
			if (game::DB_XAssetExists(game::ASSET_TYPE_LOCALIZE, presence_key) && 
				!game::DB_IsXAssetDefault(game::ASSET_TYPE_LOCALIZE, presence_key))
			{
				mapname = game::UI_SafeTranslateString(presence_key);
			}

			if (game::environment::is_mp())
			{
				static const auto gametype_dvar = game::Dvar_FindVar("g_gametype");
				static const auto max_clients_dvar = game::Dvar_FindVar("sv_maxclients");
				static const auto hostname_dvar = game::Dvar_FindVar("sv_hostname");

				const auto gametype_display_name = game::UI_GetGameTypeDisplayName(gametype_dvar->current.string);
				const auto gametype = utils::string::strip(gametype_display_name);

				discord_strings.details = std::format("{} on {}", gametype, mapname);

				const auto client_state = *game::mp::client_state;
				if (client_state != nullptr)
				{
					discord_presence.partySize = client_state->num_players;
				}

				if (game::SV_Loaded())
				{
					discord_strings.state = "Private Match";
					discord_presence.partyMax = max_clients_dvar->current.integer;
					discord_presence.partyPrivacy = DISCORD_PARTY_PRIVATE;
				}
				else
				{
					discord_strings.state = utils::string::strip(hostname_dvar->current.string);

					const auto server_connection_state = party::get_server_connection_state();
					const auto server_ip_port = std::format("{}.{}.{}.{}:{}",
						static_cast<int>(server_connection_state.host.ip[0]),
						static_cast<int>(server_connection_state.host.ip[1]),
						static_cast<int>(server_connection_state.host.ip[2]),
						static_cast<int>(server_connection_state.host.ip[3]),
						static_cast<int>(ntohs(server_connection_state.host.port))
					);

					discord_strings.party_id = utils::cryptography::sha1::compute(server_ip_port, true).substr(0, 8);
					discord_presence.partyMax = server_connection_state.max_clients;
					discord_presence.partyPrivacy = DISCORD_PARTY_PUBLIC;
					discord_strings.join_secret = server_ip_port;
				}

				auto server_discord_info = party::get_server_discord_info();
				if (server_discord_info.has_value())
				{
					discord_strings.small_image_key = server_discord_info->image;
					discord_strings.small_image_text = server_discord_info->image_text;
				}
			}
			else if (game::environment::is_sp())
			{
				discord_strings.details = mapname;
			}

			if (discord_presence.startTimestamp == 0)
			{
				discord_presence.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
					std::chrono::system_clock::now().time_since_epoch()).count();
			}

			discord_presence.state = discord_strings.state.data();
			discord_presence.details = discord_strings.details.data();
			discord_presence.smallImageKey = discord_strings.small_image_key.data();
			discord_presence.smallImageText = discord_strings.small_image_text.data();
			discord_presence.largeImageKey = discord_strings.large_image_key.data();
			discord_presence.largeImageText = discord_strings.large_image_text.data();
			discord_presence.partyId = discord_strings.party_id.data();
			discord_presence.joinSecret = discord_strings.join_secret.data();

			Discord_UpdatePresence(&discord_presence);
		}

		void update_discord()
		{
			const auto saved_time = discord_presence.startTimestamp;
			discord_presence = {};
			discord_presence.startTimestamp = saved_time;

			if (!game::CL_IsCgameInitialized() || game::VirtualLobby_Loaded())
			{
				update_discord_frontend();
			}
			else
			{
				update_discord_ingame();
			}
		}

		game::Material* create_avatar_material(const std::string& name, const std::string& data)
		{
			const auto material = materials::create_material(name);
			try
			{
				if (!materials::setup_material_image(material, data))
				{
					materials::free_material(material);
					return nullptr;
				}

				{
					std::lock_guard _0(avatar_map_mutex);
					avatar_material_map.insert(std::make_pair(name, material));
				}

				return material;
			}
			catch (const std::exception& e)
			{
				materials::free_material(material);
				console::error("Failed to load user avatar image: %s\n", e.what());
			}

			return nullptr;
		}

		void download_user_avatar(const std::string& id, const std::string& avatar)
		{
			const auto data = utils::http::get_data(
				utils::string::va(AVATAR_URL, id.data(), avatar.data()));
			if (!data.has_value())
			{
				return;
			}

			const auto& value = data.value();
			if (value.code != CURLE_OK)
			{
				return;
			}

			const auto name = utils::string::va(AVATAR, id.data());
			create_avatar_material(name, value.buffer);
		}

		void download_default_avatar()
		{
			const auto data = utils::http::get_data(DEFAULT_AVATAR_URL);
			if (!data.has_value())
			{
				return;
			}

			const auto& value = data.value();
			if (value.code != CURLE_OK)
			{
				return;
			}

			default_avatar_material = create_avatar_material(DEFAULT_AVATAR, value.buffer);
		}

		void ready(const DiscordUser* request)
		{
			DiscordRichPresence presence{};
			presence.instance = 1;
			presence.state = "";
			console::info("Discord: Ready on %s (%s)\n", request->username, request->userId);
			Discord_UpdatePresence(&presence);
		}

		void errored(const int error_code, const char* message)
		{
			console::error("Discord: %s (%i)\n", message, error_code);
		}

		void join_game(const char* join_secret)
		{
			console::debug("Discord: join_game called with secret '%s'\n", join_secret);

			scheduler::once([=]
			{
				game::netadr_s target{};
				if (game::NET_StringToAdr(join_secret, &target))
				{
					console::info("Discord: Connecting to server '%s'\n", join_secret);
					party::connect(target);
				}
			}, scheduler::pipeline::main);
		}

		void join_request(const DiscordUser* request)
		{
			console::debug("Discord: Join request from %s (%s)\n", request->username, request->userId);

			if (game::Com_InFrontend() || !ui_scripting::lui_running())
			{
				Discord_Respond(request->userId, DISCORD_REPLY_IGNORE);
				return;
			}

			static std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> last_requests;

			const std::string user_id = request->userId;
			const std::string avatar = request->avatar;
			const std::string discriminator = request->discriminator;
			const std::string username = request->username;

			const auto now = std::chrono::high_resolution_clock::now();
			auto iter = last_requests.find(user_id);
			if (iter != last_requests.end())
			{
				if ((now - iter->second) < 15s)
				{
					return;
				}
				else
				{
					iter->second = now;
				}
			}
			else
			{
				last_requests.insert(std::make_pair(user_id, now));
			}

			scheduler::once([=]
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

			const auto material_name = utils::string::va(AVATAR, user_id.data());
			if (!avatar.empty() && !avatar_material_map.contains(material_name))
			{
				download_user_avatar(user_id, avatar);
			}
		}

		void set_default_bindings()
		{
			const auto set_binding = [](const std::string& command, const game::keyNum_t key)
			{
				const auto binding = game::Key_GetBindingForCmd(command.data());
				for (auto i = 0; i < 256; i++)
				{
					if (game::playerKeys[0].keys[i].binding == binding)
					{
						return;
					}
				}

				if (game::playerKeys[0].keys[key].binding == 0)
				{
					game::Key_SetBinding(0, key, binding);
				}
			};

			set_binding("discord_accept", game::K_F1);
			set_binding("discord_deny", game::K_F2);
		}
	}

	game::Material* get_avatar_material(const std::string& id)
	{
		const auto material_name = utils::string::va(AVATAR, id.data());
		const auto iter = avatar_material_map.find(material_name);
		if (iter == avatar_material_map.end())
		{
			return default_avatar_material;
		}

		return iter->second;
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
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			DiscordEventHandlers handlers{};
			handlers.ready = ready;
			handlers.errored = errored;
			handlers.disconnected = errored;
			handlers.spectateGame = nullptr;

			if (game::environment::is_mp())
			{
				handlers.joinGame = join_game;
				handlers.joinRequest = join_request;
			}
			else
			{
				handlers.joinGame = nullptr;
				handlers.joinRequest = nullptr;
			}

			Discord_Initialize("947125042930667530", &handlers, 1, nullptr);

			if (game::environment::is_mp())
			{
				scheduler::on_game_initialized([]
				{
					scheduler::once(download_default_avatar, scheduler::async);
					set_default_bindings();
				}, scheduler::main);
			}

			scheduler::loop(Discord_RunCallbacks, scheduler::async, 500ms);
			scheduler::loop(update_discord, scheduler::async, 5s);

			initialized_ = true;

			command::add("discord_accept", []()
			{
				ui_scripting::notify("discord_response", {{"accept", true}});
			});

			command::add("discord_deny", []()
			{
				ui_scripting::notify("discord_response", {{"accept", false}});
			});
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
	};
}

REGISTER_COMPONENT(discord::component)
