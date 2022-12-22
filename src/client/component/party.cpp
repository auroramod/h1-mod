#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "party.hpp"
#include "console.hpp"
#include "command.hpp"
#include "network.hpp"
#include "scheduler.hpp"
#include "server_list.hpp"
#include "download.hpp"
#include "fastfiles.hpp"

#include "game/game.hpp"
#include "game/ui_scripting/execution.hpp"

#include "steam/steam.hpp"

#include <utils/properties.hpp>
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

		struct usermap_file
		{
			std::string extension;
			std::string name;
			bool optional;
		};

		std::vector<usermap_file> usermap_files =
		{
			{".ff", "usermaphash", false},
			{"_load.ff", "usermaploadhash", true},
			{".arena", "usermaparenahash", true},
		};

		struct
		{
			game::netadr_s host{};
			utils::info_string info_string{};
		} saved_info_response;

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

			if (game::VirtualLobby_Loaded())
			{
				// exit from virtuallobby
				utils::hook::invoke<void>(0x13C9C0_b, 1);
			}

			if (!fastfiles::is_stock_map(mapname))
			{
				fastfiles::set_usermap(mapname);
			}

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

		void cl_disconnect_stub(int show_main_menu) // possibly bool
		{
			party::clear_sv_motd();
			if (!game::VirtualLobby_Loaded())
			{
				fastfiles::clear_usermap();
			}
			cl_disconnect_hook.invoke<void>(show_main_menu);
		}

		std::unordered_map<std::string, std::string> hash_cache;

		std::string get_file_hash(const std::string& file)
		{
			if (!utils::io::file_exists(file))
			{
				return {};
			}

			const auto iter = hash_cache.find(file);
			if (iter != hash_cache.end())
			{
				return iter->second;
			}

			const auto data = utils::io::read_file(file);
			const auto sha = utils::cryptography::sha1::compute(data, true);
			hash_cache[file] = sha;
			return sha;
		}

		std::string get_usermap_file_path(const std::string& mapname, const std::string& extension)
		{
			return utils::string::va("usermaps\\%s\\%s%s", mapname.data(), mapname.data(), extension.data());
		}

		void check_download_map(const utils::info_string& info, std::vector<download::file_t>& files)
		{
			const auto mapname = info.get("mapname");
			if (fastfiles::is_stock_map(mapname))
			{
				return;
			}

			if (mapname.contains('.') || mapname.contains("::"))
			{
				throw std::runtime_error(utils::string::va("Invalid server mapname value %s\n", mapname.data()));
			}

			const auto check_file = [&](const std::string& ext, const std::string& name, bool optional)
			{
				const std::string filename = utils::string::va("usermaps/%s/%s%s", mapname.data(), mapname.data(), ext.data());
				const auto source_hash = info.get(name);
				if (source_hash.empty())
				{
					if (!optional)
					{
						throw std::runtime_error(utils::string::va("Server %s is empty", name.data()));
					}

					return;
				}

				const auto hash = get_file_hash(filename);
				if (hash != source_hash)
				{
					files.emplace_back(filename, source_hash);
					return;
				}
			};

			for (const auto& [ext, name, opt] : usermap_files)
			{
				check_file(ext, name, opt);
			}
		}

		bool check_download_mod(const utils::info_string& info, std::vector<download::file_t>& files)
		{
			static const auto fs_game = game::Dvar_FindVar("fs_game");
			const auto client_fs_game = utils::string::to_lower(fs_game->current.string);
			const auto server_fs_game = utils::string::to_lower(info.get("fs_game"));

			if (server_fs_game.empty() && client_fs_game.empty())
			{
				return false;
			}

			if (server_fs_game.empty() && !client_fs_game.empty())
			{
				game::Dvar_SetFromStringByNameFromSource("fs_game", "",
					game::DVAR_SOURCE_INTERNAL);
				return true;
			}

			if (!server_fs_game.starts_with("mods/") || server_fs_game.contains('.') || server_fs_game.contains("::"))
			{
				throw std::runtime_error(utils::string::va("Invalid server fs_game value %s\n", server_fs_game.data()));
			}

			const auto source_hash = info.get("modHash");
			if (source_hash.empty())
			{
				throw std::runtime_error("Connection failed: Server mod hash is empty.");
			}

			const auto mod_path = server_fs_game + "/mod.ff";
			auto has_to_download = !utils::io::file_exists(mod_path);

			if (!has_to_download)
			{
				const auto data = utils::io::read_file(mod_path);
				const auto hash = utils::cryptography::sha1::compute(data, true);

				has_to_download = source_hash != hash;
			}

			if (has_to_download)
			{
				files.emplace_back(mod_path, source_hash);
				return false;
			}
			else if (client_fs_game != server_fs_game)
			{
				game::Dvar_SetFromStringByNameFromSource("fs_game", server_fs_game.data(),
					game::DVAR_SOURCE_INTERNAL);
				return true;
			}

			return false;
		}

		void close_joining_popups()
		{
			if (game::Menu_IsMenuOpenAndVisible(0, "popup_acceptinginvite"))
			{
				command::execute("lui_close popup_acceptinginvite", false);
			}
			if (game::Menu_IsMenuOpenAndVisible(0, "generic_waiting_popup_"))
			{
				command::execute("lui_close generic_waiting_popup_", false);
			}
		}

		std::string get_whitelist_json_path()
		{
			return (utils::properties::get_appdata_path() / "whitelist.json").generic_string();
		}

		nlohmann::json get_whitelist_json_object()
		{
			std::string data;
			if (!utils::io::read_file(get_whitelist_json_path(), &data))
			{
				return nullptr;
			}

			nlohmann::json obj;
			try
			{
				obj = nlohmann::json::parse(data.data());
			}
			catch (const nlohmann::json::parse_error& ex)
			{
				menu_error(utils::string::va("%s\n", ex.what()));
				return nullptr;
			}

			return obj;
		}

		std::string target_ip_to_string(const game::netadr_s& target)
		{
			return utils::string::va("%i.%i.%i.%i",
				static_cast<int>(saved_info_response.host.ip[0]),
				static_cast<int>(saved_info_response.host.ip[1]),
				static_cast<int>(saved_info_response.host.ip[2]),
				static_cast<int>(saved_info_response.host.ip[3]));
		}

		bool download_files(const game::netadr_s& target, const utils::info_string& info, bool allow_download);

		void user_download_response(bool response)
		{
			if (!response)
			{
				return;
			}

			nlohmann::json obj = get_whitelist_json_object();
			if (obj == nullptr)
			{
				obj = {};
			}

			obj.push_back(target_ip_to_string(saved_info_response.host));

			utils::io::write_file(get_whitelist_json_path(), obj.dump(4));

			download_files(saved_info_response.host, saved_info_response.info_string, true);
		}

		bool should_user_confirm(const game::netadr_s& target, const utils::info_string& info)
		{
			nlohmann::json obj = get_whitelist_json_object();
			if (obj != nullptr)
			{
				const auto target_ip = target_ip_to_string(target);
				for (const auto& [key, value] : obj.items())
				{
					if (value.is_string() && value.get<std::string>() == target_ip)
					{
						return false;
					}
				}
			}

			const auto LUI = ui_scripting::get_globals().get("LUI").as<ui_scripting::table>();
			const auto yes_no_popup_func = LUI.get("yesnopopup").as<ui_scripting::function>();

			close_joining_popups();

			const ui_scripting::table data_table{};
			data_table.set("title", game::UI_SafeTranslateString("MENU_NOTICE"));
			data_table.set("text", std::format("Would you like to install required 3rd-party content for this server? (from {})", info.get("sv_wwwBaseUrl")));
			data_table.set("callback", user_download_response);

			yes_no_popup_func(data_table);

			return true;
		}

		bool needs_vid_restart = false;

		bool download_files(const game::netadr_s& target, const utils::info_string& info, bool allow_download)
		{
			try
			{
				std::vector<download::file_t> files{};

				const auto needs_restart = check_download_mod(info, files);
				needs_vid_restart = needs_vid_restart || needs_restart;
				check_download_map(info, files);

				if (files.size() > 0)
				{
					if (!allow_download && should_user_confirm(target, info))
					{
						return true;
					}

					download::stop_download();
					download::start_download(target, info, files);
					return true;
				}
				else if (needs_restart || needs_vid_restart)
				{
					command::execute("vid_restart");
					needs_vid_restart = false;
					scheduler::once([=]()
					{
						connect(target);
					}, scheduler::pipeline::main);
					return true;
				}
			}
			catch (const std::exception& e)
			{
				menu_error(e.what());
				return true;
			}

			return false;
		}

		void set_new_map(const char* mapname, const char* gametype, game::msg_t* msg)
		{
			if (game::SV_Loaded() || fastfiles::is_stock_map(mapname))
			{
				utils::hook::invoke<void>(0x13AAD0_b, mapname, gametype);
				return;
			}

			fastfiles::set_usermap(mapname);

			for (const auto& [ext, key, opt] : usermap_files)
			{
				char buffer[0x100] = {0};
				const std::string source_hash = game::MSG_ReadStringLine(msg,
					buffer, static_cast<unsigned int>(sizeof(buffer)));

				const auto path = get_usermap_file_path(mapname, ext);
				const auto hash = get_file_hash(path);

				if ((!source_hash.empty() && hash != source_hash) || (source_hash.empty() && !opt))
				{
					command::execute("disconnect");
					scheduler::once([]
					{
						connect(connect_state.host);
					}, scheduler::pipeline::main);
					return;
				}
			}

			utils::hook::invoke<void>(0x13AAD0_b, mapname, gametype);
		}

		void loading_new_map_cl_stub(utils::hook::assembler& a)
		{
			a.pushad64();
			a.mov(r8, rdi);
			a.call_aligned(set_new_map);
			a.popad64();

			a.mov(al, 1);
			a.jmp(0x12FCAA_b);
		}

		std::string current_sv_mapname;

		void sv_spawn_server_stub(const char* map, void* a2, void* a3, void* a4, void* a5)
		{
			if (!fastfiles::is_stock_map(map))
			{
				fastfiles::set_usermap(map);
			}

			hash_cache.clear();
			current_sv_mapname = map;
			utils::hook::invoke<void>(0x54BBB0_b, map, a2, a3, a4, a5);
		}

		utils::hook::detour net_out_of_band_print_hook;
		void net_out_of_band_print_stub(game::netsrc_t sock, game::netadr_s* addr, const char* data)
		{
			if (!std::strstr(data, "loadingnewmap"))
			{
				return net_out_of_band_print_hook.invoke<void>(sock, addr, data);
			}
			
			std::string buffer{};
			const auto line = [&](const std::string& data_)
			{
				buffer.append(data_);
				buffer.append("\n");
			};

			const auto* sv_gametype = game::Dvar_FindVar("g_gametype");
			line("loadingnewmap");
			line(current_sv_mapname);
			line(sv_gametype->current.string);

			const auto add_hash = [&](const std::string extension)
			{
				const auto filename = get_usermap_file_path(current_sv_mapname, extension);
				const auto hash = get_file_hash(filename);
				line(hash);
			};

			const auto is_usermap = fastfiles::usermap_exists(current_sv_mapname);
			for (const auto& [ext, key, opt] : usermap_files)
			{
				if (is_usermap)
				{
					add_hash(ext);
				}
				else
				{
					line("");
				}
			}

			net_out_of_band_print_hook.invoke<void>(sock, addr, buffer.data());
		}
	}

	void menu_error(const std::string& error)
	{
		console::error("%s\n", error.data());

		close_joining_popups();

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

			// add usermaphash to loadingnewmap command
			utils::hook::jump(0x12FA68_b, utils::hook::assemble(loading_new_map_cl_stub), true);
			utils::hook::call(0x54CC98_b, sv_spawn_server_stub);
			net_out_of_band_print_hook.create(game::NET_OutOfBandPrint, net_out_of_band_print_stub);

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
				console::info("%s -> %i: %s\n", name, client_num, message.data());
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
				console::info("%i: %s\n", client_num, message.data());
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
				console::info("%s: %s\n", name, message.data());
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
				console::info("%s\n", message.data());
			});

			network::on("getInfo", [](const game::netadr_s& target, const std::string& data)
			{
				const auto mapname = get_dvar_string("mapname");

				utils::info_string info;
				info.set("challenge", data);
				info.set("gamename", "H1");
				info.set("hostname", get_dvar_string("sv_hostname"));
				info.set("gametype", get_dvar_string("g_gametype"));
				info.set("sv_motd", get_dvar_string("sv_motd"));
				info.set("xuid", utils::string::va("%llX", steam::SteamUser()->GetSteamID().bits));
				info.set("mapname", mapname);
				info.set("isPrivate", get_dvar_string("g_password").empty() ? "0" : "1");
				info.set("clients", utils::string::va("%i", get_client_count()));
				info.set("bots", utils::string::va("%i", get_bot_count()));
				info.set("sv_maxclients", utils::string::va("%i", *game::mp::svs_numclients));
				info.set("protocol", utils::string::va("%i", PROTOCOL));
				info.set("playmode", utils::string::va("%i", game::Com_GetCurrentCoDPlayMode()));
				info.set("sv_running", utils::string::va("%i", get_dvar_bool("sv_running") && !game::VirtualLobby_Loaded()));
				info.set("dedicated", utils::string::va("%i", get_dvar_bool("dedicated")));
				info.set("sv_wwwBaseUrl", get_dvar_string("sv_wwwBaseUrl"));

				if (!fastfiles::is_stock_map(mapname))
				{
					const auto add_hash = [&](const std::string& extension, const std::string& name)
					{
						const auto path = get_usermap_file_path(mapname, extension);
						const auto hash = get_file_hash(path);
						info.set(name, hash);
					};

					for (const auto& [ext, name, opt] : usermap_files)
					{
						add_hash(ext, name);
					}
				}

				const auto fs_game = get_dvar_string("fs_game");
				info.set("fs_game", fs_game);

				if (!fs_game.empty())
				{
					const auto hash = get_file_hash(utils::string::va("%s/mod.ff", fs_game.data()));
					info.set("modHash", hash);
				}

				network::send(target, "infoResponse", info.build(), '\n');
			});

			network::on("infoResponse", [](const game::netadr_s& target, const std::string& data)
			{
				const utils::info_string info(data);
				server_list::handle_info_response(target, info);

				if (connect_state.host != target)
				{
					return;
				}

				saved_info_response = {};
				saved_info_response.host = target;
				saved_info_response.info_string = info;

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

				if (download_files(target, info, false))
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