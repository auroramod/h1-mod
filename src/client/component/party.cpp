#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "party.hpp"
#include "console.hpp"
#include "command.hpp"
#include "network.hpp"
#include "scheduler.hpp"
#include "server_list.hpp"
#include "download.hpp"

#include "game/game.hpp"

#include "steam/steam.hpp"

#include <utils/string.hpp>
#include <utils/info_string.hpp>
#include <utils/cryptography.hpp>
#include <utils/hook.hpp>
#include <utils/io.hpp>

namespace party
{
	namespace
	{
		struct
		{
			game::netadr_s host{};
			std::string challenge{};
			bool hostDefined{false};
		} connect_state;

		std::string sv_motd;
		int sv_maxclients;

		std::optional<std::string> mod_hash{};

		void perform_game_initialization()
		{
			command::execute("onlinegame 1", true);
			command::execute("xstartprivateparty", true);
			command::execute("xblive_privatematch 1", true);
			command::execute("startentitlements", true);
		}

		void connect_to_party(const game::netadr_s& target, const std::string& mapname, const std::string& gametype)
		{
			if (game::environment::is_sp())
			{
				return;
			}

			if (game::Live_SyncOnlineDataFlags(0) != 0)
			{
				// initialize the game after onlinedataflags is 32 (workaround)
				if (game::Live_SyncOnlineDataFlags(0) == 32)
				{
					scheduler::once([=]()
					{
						command::execute("xstartprivateparty", true);
						command::execute("disconnect", true); // 32 -> 0

						connect_to_party(target, mapname, gametype);
					}, scheduler::pipeline::main, 1s);
					return;
				}
				else
				{
					scheduler::once([=]()
					{
						connect_to_party(target, mapname, gametype);
					}, scheduler::pipeline::main, 1s);
					return;
				}
			}

			perform_game_initialization();

			// exit from virtuallobby
			utils::hook::invoke<void>(0x13C9C0_b, 1);

			// CL_ConnectFromParty
			char session_info[0x100] = {};
			utils::hook::invoke<void>(0x12DFF0_b, 0, session_info, &target, mapname.data(), gametype.data());
		}

		std::string get_dvar_string(const std::string& dvar)
		{
			auto* dvar_value = game::Dvar_FindVar(dvar.data());
			if (dvar_value && dvar_value->current.string)
			{
				return dvar_value->current.string;
			}

			return {};
		}

		int get_dvar_int(const std::string& dvar)
		{
			auto* dvar_value = game::Dvar_FindVar(dvar.data());
			if (dvar_value && dvar_value->current.integer)
			{
				return dvar_value->current.integer;
			}

			return -1;
		}

		bool get_dvar_bool(const std::string& dvar)
		{
			auto* dvar_value = game::Dvar_FindVar(dvar.data());
			if (dvar_value && dvar_value->current.enabled)
			{
				return dvar_value->current.enabled;
			}

			return false;
		}

		const char* get_didyouknow_stub(void* table, int row, int column)
		{
			if (party::sv_motd.empty())
			{
				return utils::hook::invoke<const char*>(0x5A0AC0_b, table, row, column);
			}
			return utils::string::va("%s", party::sv_motd.data());
		}

		void disconnect()
		{
			if (!game::VirtualLobby_Loaded())
			{
				if (game::CL_IsCgameInitialized())
				{
					// CL_AddReliableCommand
					utils::hook::invoke<void>(0x12B810_b, 0, "disconnect");
					// CL_WritePacket
					utils::hook::invoke<void>(0x13D490_b, 0);
				}
				// CL_Disconnect
				utils::hook::invoke<void>(0x12F080_b, 0);
			}
		}

		utils::hook::detour cl_disconnect_hook;

		void cl_disconnect_stub(int showMainMenu) // possibly bool
		{
			party::clear_sv_motd();
			cl_disconnect_hook.invoke<void>(showMainMenu);
		}

		bool download_mod(const game::netadr_s& target, const utils::info_string& info)
		{
			const auto server_fs_game = utils::string::to_lower(info.get("fs_game"));
			if (server_fs_game.empty())
			{
				return false;
			}

			if (!server_fs_game.starts_with("mods/") || server_fs_game.contains('.'))
			{
				menu_error(utils::string::va("Invalid server fs_game value %s\n", server_fs_game.data()));
				return true;
			}

			const auto source_hash = info.get("modHash");
			if (source_hash.empty())
			{
				menu_error("Connection failed: Server mod hash is empty.");
				return true;
			}

			static const auto fs_game = game::Dvar_FindVar("fs_game");
			const auto client_fs_game = utils::string::to_lower(fs_game->current.string);

			const auto mod_path = server_fs_game + "/mod.ff";
			auto has_to_download = !utils::io::file_exists(mod_path);
			if (!has_to_download)
			{
				const auto data = utils::io::read_file(mod_path);
				const auto hash = utils::cryptography::sha1::compute(data, true);

				has_to_download = source_hash != hash;
			}
			else
			{
				console::debug("Failed to find mod, downloading\n");
			}

			if (has_to_download)
			{
				console::debug("Starting download of mod '%s'\n", server_fs_game.data());

				download::stop_download();
				download::start_download(target, info);

				return true;
			}
			else if (client_fs_game != server_fs_game)
			{
				game::Dvar_SetFromStringByNameFromSource("fs_game", server_fs_game.data(), 
					game::DVAR_SOURCE_INTERNAL);
				command::execute("vid_restart");

				// set fs_game to the mod the server is on, "restart" game, and then (hopefully) reconnect
				scheduler::once([=]()
				{
					command::execute("lui_open_popup popup_acceptinginvite", false);
					// connecting too soon after vid_restart causes a crash ingame (awesome game)
					scheduler::once([=]()
					{
						connect(target);
					}, scheduler::pipeline::main, 5s);
				}, scheduler::pipeline::main);

				return true;
			}

			return false;
		}
	}

	void menu_error(const std::string& error)
	{
		console::error("%s\n", error.data());
		if (game::Menu_IsMenuOpenAndVisible(0, "popup_acceptinginvite"))
		{
			command::execute("lui_close popup_acceptinginvite", false);
		}

		utils::hook::invoke<void>(0x17D770_b, error.data(), "MENU_NOTICE"); // Com_SetLocalizedErrorMessage
		*reinterpret_cast<int*>(0x2ED2F78_b) = 1;
	}

	void clear_sv_motd()
	{
		party::sv_motd.clear();
	}

	int get_client_num_by_name(const std::string& name)
	{
		for (auto i = 0; !name.empty() && i < *game::mp::svs_numclients; ++i)
		{
			if (game::mp::g_entities[i].client)
			{
				char client_name[16] = {0};
				strncpy_s(client_name, game::mp::g_entities[i].client->name, sizeof(client_name));
				game::I_CleanStr(client_name);

				if (client_name == name)
				{
					return i;
				}
			}
		}
		return -1;
	}

	void reset_connect_state()
	{
		connect_state = {};
	}

	int get_client_count()
	{
		auto count = 0;
		const auto* svs_clients = *game::mp::svs_clients;
		if (svs_clients == nullptr)
		{
			return count;
		}

		for (auto i = 0; i < *game::mp::svs_numclients; ++i)
		{
			if (svs_clients[i].header.state >= 1)
			{
				++count;
			}
		}

		return count;
	}

	int get_bot_count()
	{
		auto count = 0;
		const auto* svs_clients = *game::mp::svs_clients;
		if (svs_clients == nullptr)
		{
			return count;
		}

		for (auto i = 0; i < *game::mp::svs_numclients; ++i)
		{
			if (svs_clients[i].header.state >= 1 &&
				game::SV_BotIsBot(i))
			{
				++count;
			}
		}

		return count;
	}

	void connect(const game::netadr_s& target)
	{
		if (game::environment::is_sp())
		{
			return;
		}

		command::execute("lui_open_popup popup_acceptinginvite", false);

		connect_state.host = target;
		connect_state.challenge = utils::cryptography::random::get_challenge();
		connect_state.hostDefined = true;

		network::send(target, "getInfo", connect_state.challenge);
	}

	game::netadr_s get_state_host()
	{
		return connect_state.host;
	}

	std::string get_state_challenge()
	{
		return connect_state.challenge;
	}

	void start_map(const std::string& mapname, bool dev)
	{
		if (game::Live_SyncOnlineDataFlags(0) > 32)
		{
			scheduler::once([=]()
			{
				start_map(mapname, dev);
			}, scheduler::pipeline::main, 1s);
		}
		else
		{
			if (!game::SV_MapExists(mapname.data()))
			{
				console::info("Map '%s' doesn't exist.\n", mapname.data());
				return;
			}

			auto* current_mapname = game::Dvar_FindVar("mapname");
			if (current_mapname && utils::string::to_lower(current_mapname->current.string) ==
				utils::string::to_lower(mapname) && (game::SV_Loaded() && !game::VirtualLobby_Loaded()))
			{
				console::info("Restarting map: %s\n", mapname.data());
				command::execute("map_restart", false);
				return;
			}

			if (!game::environment::is_dedi())
			{
				if (game::SV_Loaded())
				{
					const auto* args = "Leave";
					game::UI_RunMenuScript(0, &args);
				}

				perform_game_initialization();
			}

			console::info("Starting map: %s\n", mapname.data());
			auto* gametype = game::Dvar_FindVar("g_gametype");
			if (gametype && gametype->current.string)
			{
				command::execute(utils::string::va("ui_gametype %s", gametype->current.string), true);
			}
			command::execute(utils::string::va("ui_mapname %s", mapname.data()), true);

			/*auto* maxclients = game::Dvar_FindVar("sv_maxclients");
			if (maxclients)
			{
				command::execute(utils::string::va("ui_maxclients %i", maxclients->current.integer), true);
				command::execute(utils::string::va("party_maxplayers %i", maxclients->current.integer), true);
			}*/

			command::execute((dev ? "sv_cheats 1" : "sv_cheats 0"), true);

			const auto* args = "StartServer";
			game::UI_RunMenuScript(0, &args);
		}
	}

	int server_client_count()
	{
		return party::sv_maxclients;
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

			// detour CL_Disconnect to clear motd
			cl_disconnect_hook.create(0x12F080_b, cl_disconnect_stub);

			if (game::environment::is_mp())
			{
				// show custom drop reason
				utils::hook::nop(0x12EF4E_b, 13);
				utils::hook::jump(0x12EF4E_b, utils::hook::assemble([](utils::hook::assembler& a)
				{
					a.mov(rdx, rsi);
					a.mov(ecx, 2);
					a.jmp(0x12EF27_b);
				}), true);

				command::add("disconnect", disconnect);
			}

			// enable custom kick reason in GScr_KickPlayer
			utils::hook::set<uint8_t>(0xE423D_b, 0xEB);

			// allow custom didyouknow based on sv_motd
			utils::hook::call(0x1A8A3A_b, get_didyouknow_stub);

			command::add("map", [](const command::params& argument)
			{
				if (argument.size() != 2)
				{
					return;
				}

				start_map(argument[1], false);
			});

			command::add("devmap", [](const command::params& argument)
			{
				if (argument.size() != 2)
				{
					return;
				}

				party::start_map(argument[1], true);
			});

			command::add("map_restart", []()
			{
				if (!game::SV_Loaded() || game::VirtualLobby_Loaded())
				{
					return;
				}

				*reinterpret_cast<int*>(0xB7B8E60_b) = 1; // sv_map_restart
				*reinterpret_cast<int*>(0xB7B8E64_b) = 1; // sv_loadScripts
				*reinterpret_cast<int*>(0xB7B8E68_b) = 0; // sv_migrate

				utils::hook::invoke<void>(0x54BD50_b); // SV_CheckLoadGame
			});

			command::add("fast_restart", []()
			{
				if (game::SV_Loaded() && !game::VirtualLobby_Loaded())
				{
					game::SV_FastRestart(0);
				}
			});

			command::add("reconnect", [](const command::params& argument)
			{
				if (!connect_state.hostDefined)
				{
					console::info("Cannot connect to server.\n");
					return;
				}

				if (game::CL_IsCgameInitialized())
				{
					command::execute("disconnect");
					command::execute("reconnect");
				}
				else
				{
					connect(connect_state.host);
				}
			});

			command::add("connect", [](const command::params& argument)
			{
				if (argument.size() != 2)
				{
					return;
				}

				game::netadr_s target{};
				if (game::NET_StringToAdr(argument[1], &target))
				{
					connect(target);
				}
			});

			command::add("kickClient", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("usage: kickClient <num>, <reason>(optional)\n");
					return;
				}

				if (!game::SV_Loaded() || game::VirtualLobby_Loaded())
				{
					return;
				}

				std::string reason;
				if (params.size() > 2)
				{
					reason = params.join(2);
				}
				if (reason.empty())
				{
					reason = "EXE_PLAYERKICKED";
				}

				const auto client_num = atoi(params.get(1));
				if (client_num < 0 || client_num >= *game::mp::svs_numclients)
				{
					return;
				}

				scheduler::once([client_num, reason]()
				{
					game::SV_KickClientNum(client_num, reason.data());
				}, scheduler::pipeline::server);
			});

			command::add("kick", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("usage: kick <name>, <reason>(optional)\n");
					return;
				}

				if (!game::SV_Loaded() || game::VirtualLobby_Loaded())
				{
					return;
				}

				std::string reason;
				if (params.size() > 2)
				{
					reason = params.join(2);
				}
				if (reason.empty())
				{
					reason = "EXE_PLAYERKICKED";
				}

				const std::string name = params.get(1);
				if (name == "all"s)
				{
					for (auto i = 0; i < *game::mp::svs_numclients; ++i)
					{
						scheduler::once([i, reason]()
						{
							game::SV_KickClientNum(i, reason.data());
						}, scheduler::pipeline::server);
					}
					return;
				}

				const auto client_num = get_client_num_by_name(name);
				if (client_num < 0 || client_num >= *game::mp::svs_numclients)
				{
					return;
				}

				scheduler::once([client_num, reason]()
				{
					game::SV_KickClientNum(client_num, reason.data());
				}, scheduler::pipeline::server);
			});

			scheduler::once([]()
			{
				const auto hash = game::generateHashValue("sv_sayName");
				game::Dvar_RegisterString(hash, "sv_sayName", "console", game::DvarFlags::DVAR_FLAG_NONE);
			}, scheduler::pipeline::main);

			command::add("tell", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					return;
				}

				const auto client_num = atoi(params.get(1));
				const auto message = params.join(2);
				const auto* const name = game::Dvar_FindVar("sv_sayName")->current.string;

				game::SV_GameSendServerCommand(client_num, game::SV_CMD_CAN_IGNORE,
				                               utils::string::va("%c \"%s: %s\"", 84, name, message.data()));
				printf("%s -> %i: %s\n", name, client_num, message.data());
			});

			command::add("tellraw", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					return;
				}

				const auto client_num = atoi(params.get(1));
				const auto message = params.join(2);

				game::SV_GameSendServerCommand(client_num, game::SV_CMD_CAN_IGNORE,
				                               utils::string::va("%c \"%s\"", 84, message.data()));
				printf("%i: %s\n", client_num, message.data());
			});

			command::add("say", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto message = params.join(1);
				const auto* const name = game::Dvar_FindVar("sv_sayName")->current.string;

				game::SV_GameSendServerCommand(
					-1, game::SV_CMD_CAN_IGNORE, utils::string::va("%c \"%s: %s\"", 84, name, message.data()));
				printf("%s: %s\n", name, message.data());
			});

			command::add("sayraw", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto message = params.join(1);

				game::SV_GameSendServerCommand(-1, game::SV_CMD_CAN_IGNORE,
				                               utils::string::va("%c \"%s\"", 84, message.data()));
				printf("%s\n", message.data());
			});

			network::on("getInfo", [](const game::netadr_s& target, const std::string_view& data)
			{
				utils::info_string info{};
				info.set("challenge", std::string{data});
				info.set("gamename", "H1");
				info.set("hostname", get_dvar_string("sv_hostname"));
				info.set("gametype", get_dvar_string("g_gametype"));
				info.set("sv_motd", get_dvar_string("sv_motd"));
				info.set("xuid", utils::string::va("%llX", steam::SteamUser()->GetSteamID().bits));
				info.set("mapname", get_dvar_string("mapname"));
				info.set("isPrivate", get_dvar_string("g_password").empty() ? "0" : "1");
				info.set("clients", utils::string::va("%i", get_client_count()));
				info.set("bots", utils::string::va("%i", get_bot_count()));
				info.set("sv_maxclients", utils::string::va("%i", *game::mp::svs_numclients));
				info.set("protocol", utils::string::va("%i", PROTOCOL));
				info.set("playmode", utils::string::va("%i", game::Com_GetCurrentCoDPlayMode()));
				info.set("sv_running", utils::string::va("%i", get_dvar_bool("sv_running") && !game::VirtualLobby_Loaded()));
				info.set("dedicated", utils::string::va("%i", get_dvar_bool("dedicated")));
				info.set("sv_wwwBaseUrl", get_dvar_string("sv_wwwBaseUrl"));

				const auto fs_game = get_dvar_string("fs_game");
				info.set("fs_game", fs_game);

				if (mod_hash.has_value())
				{
					info.set("modHash", mod_hash.value());
				}
				else
				{
					const auto mod_path = fs_game + "/mod.ff";
					if (utils::io::file_exists(mod_path))
					{
						const auto mod_data = utils::io::read_file(mod_path);
						const auto sha = utils::cryptography::sha1::compute(mod_data, true);
						mod_hash = sha; // todo: clear this somewhere
						info.set("modHash", sha);
					}
				}

				network::send(target, "infoResponse", info.build(), '\n');
			});

			network::on("infoResponse", [](const game::netadr_s& target, const std::string_view& data)
			{
				const utils::info_string info{data};
				server_list::handle_info_response(target, info);

				if (connect_state.host != target)
				{
					return;
				}

				if (info.get("challenge") != connect_state.challenge)
				{
					menu_error("Connection failed: Invalid challenge.");
					return;
				}

				const auto gamename = info.get("gamename");
				if (gamename != "H1"s)
				{
					menu_error("Connection failed: Invalid gamename.");
					return;
				}

				const auto playmode = info.get("playmode");
				if (game::CodPlayMode(std::atoi(playmode.data())) != game::Com_GetCurrentCoDPlayMode())
				{
					menu_error("Connection failed: Invalid playmode.");
					return;
				}

				const auto sv_running = info.get("sv_running");
				if (!std::atoi(sv_running.data()))
				{
					menu_error("Connection failed: Server not running.");
					return;
				}

				const auto mapname = info.get("mapname");
				if (mapname.empty())
				{
					menu_error("Connection failed: Invalid map.");
					return;
				}

				const auto gametype = info.get("gametype");
				if (gametype.empty())
				{
					menu_error("Connection failed: Invalid gametype.");
					return;
				}

				if (download_mod(target, info))
				{
					return;
				}

				party::sv_motd = info.get("sv_motd");
				party::sv_maxclients = std::stoi(info.get("sv_maxclients"));

				connect_to_party(target, mapname, gametype);
			});
		}
	};
}

REGISTER_COMPONENT(party::component)