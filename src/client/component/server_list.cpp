#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "fastfiles.hpp"
#include "localized_strings.hpp"
#include "network.hpp"
#include "party.hpp"
#include "scheduler.hpp"
#include "server_list.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"
#include "game/ui_scripting/execution.hpp"

#include <utils/cryptography.hpp>
#include <utils/string.hpp>
#include <utils/hook.hpp>

namespace server_list
{
	namespace
	{
		const int server_limit = 100;

		struct server_info
		{
			// gotta add more to this
			int clients;
			int max_clients;
			int bots;
			int ping;
			std::string host_name;
			std::string map_name;
			std::string game_type;
			std::string mod_name;
			game::CodPlayMode play_mode;
			char in_game;
			game::netadr_s address;
			bool is_private;
		};

		struct
		{
			game::netadr_s address{};
			volatile bool requesting = false;
			std::unordered_map<game::netadr_s, int> queued_servers{};
		} master_state;

		std::mutex mutex;
		std::vector<server_info> servers;

		size_t server_list_page = 0;
		volatile bool update_server_list = false;
		std::chrono::high_resolution_clock::time_point last_scroll{};

		game::dvar_t* master_server_ip;
		game::dvar_t* master_server_port;

		size_t get_page_count()
		{
			const auto count = servers.size() / server_limit;
			return count + (servers.size() % server_limit > 0);
		}

		size_t get_page_base_index()
		{
			return server_list_page * server_limit;
		}

		void refresh_server_list()
		{
			{
				std::lock_guard<std::mutex> _(mutex);
				servers.clear();
				master_state.queued_servers.clear();
				server_list_page = 0;
			}

			party::reset_server_connection_state();

			if (get_master_server(master_state.address))
			{
				master_state.requesting = true;

				network::send(master_state.address, "getservers", utils::string::va("H1 %i full empty", PROTOCOL));
			}
		}

		void join_server(int, int, const int index)
		{
			std::lock_guard<std::mutex> _(mutex);

			const auto i = static_cast<size_t>(index) + get_page_base_index();
			if (i < servers.size())
			{
				static auto last_index = ~0ull;
				if (last_index != i)
				{
					last_index = i;
				}
				else
				{
					console::info("Connecting to (%d - %zu): %s\n", index, i, servers[i].host_name.data());
					party::connect(servers[i].address);
				}
			}
		}

		void trigger_refresh()
		{
			update_server_list = true;
		}

		int ui_feeder_count()
		{
			std::lock_guard<std::mutex> _(mutex);
			const auto count = static_cast<int>(servers.size());
			const auto index = get_page_base_index();
			const auto diff = count - index;
			return diff > server_limit ? server_limit : static_cast<int>(diff);
		}

		const char* ui_feeder_item_text(const int index, const int column)
		{
			std::lock_guard<std::mutex> _(mutex);

			const auto i = get_page_base_index() + index;

			if (i >= servers.size())
			{
				return "";
			}

			switch (column)
			{
			case 0:
				return servers[i].host_name.empty() ? "" : servers[i].host_name.data();
			case 1:
			{
				const auto& map_name = servers[i].map_name;
				if (map_name.empty())
				{
					return "Unknown";
				}

				auto map_display_name = game::UI_GetMapDisplayName(map_name.data());
				if (!fastfiles::exists(map_name, false))
				{
					map_display_name = utils::string::va("^1%s", map_display_name);
				}
				return map_display_name;
			}
			case 2:
			{
				const auto client_count = servers[i].clients - servers[i].bots;
				return utils::string::va("%d/%d [%d]", client_count, servers[i].max_clients,
					servers[i].clients);
			}
			case 3:
				return servers[i].game_type.empty() ? "" : servers[i].game_type.data();
			case 4:
			{
				const auto ping = servers[i].ping ? servers[i].ping : 999;
				if (ping < 75)
				{
					return utils::string::va("^2%d", ping);
				}
				else if (ping < 150)
				{
					return utils::string::va("^3%d", ping);
				}
				return utils::string::va("^1%d", ping);
			}
			case 5:
				return servers[i].is_private ? "1" : "0";
			case 6:
				return servers[i].mod_name.empty() ? "" : servers[i].mod_name.data();
			default:
				return "";
			}
		}

		void sort_serverlist()
		{
			std::stable_sort(servers.begin(), servers.end(), [](const server_info& a, const server_info& b)
			{
				const auto a_players = a.clients - a.bots;
				const auto b_players = b.clients - b.bots;

				if (a_players == b_players)
				{
					if (a.clients == b.clients)
					{
						return a.ping < b.ping;
					}

					return a.clients > b.clients;
				}

				return a_players > b_players;
			});
		}

		void insert_server(server_info&& server)
		{
			std::lock_guard<std::mutex> _(mutex);
			servers.emplace_back(std::move(server));
			sort_serverlist();
			trigger_refresh();
		}

		void do_frame_work()
		{
			auto& queue = master_state.queued_servers;
			if (queue.empty())
			{
				return;
			}

			std::lock_guard<std::mutex> _(mutex);

			size_t queried_servers = 0;
			const size_t query_limit = 3;

			for (auto i = queue.begin(); i != queue.end();)
			{
				if (i->second)
				{
					const auto now = game::Sys_Milliseconds();
					if (now - i->second > 10'000)
					{
						i = queue.erase(i);
						continue;
					}
				}
				else if (queried_servers++ < query_limit)
				{
					i->second = game::Sys_Milliseconds();
					network::send(i->first, "getInfo", utils::cryptography::random::get_challenge());
				}

				++i;
			}
		}

		bool is_server_list_open()
		{
			return game::Menu_IsMenuOpenAndVisible(0, "menu_systemlink_join");
		}

		bool is_scrolling_disabled()
		{
			return update_server_list || (std::chrono::high_resolution_clock::now() - last_scroll) < 500ms;
		}

		bool scroll_down()
		{
			if (!is_server_list_open())
			{
				return false;
			}

			if (!is_scrolling_disabled() && server_list_page + 1 < get_page_count())
			{
				last_scroll = std::chrono::high_resolution_clock::now();
				++server_list_page;
				trigger_refresh();
			}

			return true;
		}

		bool scroll_up()
		{
			if (!is_server_list_open())
			{
				return false;
			}

			if (!is_scrolling_disabled() && server_list_page > 0)
			{
				last_scroll = std::chrono::high_resolution_clock::now();
				--server_list_page;
				trigger_refresh();
			}

			return true;
		}

		utils::hook::detour lui_open_menu_hook;

		void lui_open_menu_stub(int controllerIndex, const char* menuName, int isPopup, int isModal, unsigned int isExclusive)
		{
#ifdef DEBUG
			console::info("[LUI] %s\n", menuName);
#endif

			if (!strcmp(menuName, "menu_systemlink_join"))
			{
				refresh_server_list();
			}

			lui_open_menu_hook.invoke<void>(controllerIndex, menuName, isPopup, isModal, isExclusive);
		}

		void check_refresh()
		{
			if (update_server_list)
			{
				update_server_list = false;
				ui_scripting::notify("updateGameList", {});
			}
		}
	}

	bool sl_key_event(const int key, const int down)
	{
		if (down)
		{
			if (key == game::keyNum_t::K_MWHEELUP)
			{
				return !scroll_up();
			}

			if (key == game::keyNum_t::K_MWHEELDOWN)
			{
				return !scroll_down();
			}
		}

		return true;
	}

	bool get_master_server(game::netadr_s& address)
	{
		return game::NET_StringToAdr(utils::string::va("%s:%s",
			master_server_ip->current.string, master_server_port->current.string), &address);
	}

	void handle_info_response(const game::netadr_s& address, const utils::info_string& info)
	{
		// Don't show servers that aren't using the same protocol!
		const auto protocol = std::atoi(info.get("protocol").data());
		if (protocol != PROTOCOL)
		{
			return;
		}

		// Don't show servers that aren't dedicated!
		const auto dedicated = std::atoi(info.get("dedicated").data());
		if (!dedicated)
		{
			return;
		}

		// Don't show servers that aren't running!
		const auto sv_running = std::atoi(info.get("sv_running").data());
		if (!sv_running)
		{
			return;
		}

		// Only handle servers of the same playmode!
		const auto playmode = game::CodPlayMode(std::atoi(info.get("playmode").data()));
		if (game::Com_GetCurrentCoDPlayMode() != playmode)
		{
			return;
		}

		if (info.get("gamename") != "H1")
		{
			return;
		}

		int start_time{};
		const auto now = game::Sys_Milliseconds();

		{
			std::lock_guard<std::mutex> _(mutex);
			const auto entry = master_state.queued_servers.find(address);

			if (entry == master_state.queued_servers.end() || !entry->second)
			{
				return;
			}

			start_time = entry->second;
			master_state.queued_servers.erase(entry);
		}

		server_info server{};
		server.address = address;
		server.host_name = info.get("hostname");
		server.map_name = info.get("mapname");
		server.game_type = game::UI_GetGameTypeDisplayName(info.get("gametype").data());
		server.mod_name = info.get("fs_game");
		server.play_mode = playmode;
		server.clients = atoi(info.get("clients").data());
		server.max_clients = atoi(info.get("sv_maxclients").data());
		server.bots = atoi(info.get("bots").data());
		server.ping = std::min(now - start_time, 999);
		server.is_private = atoi(info.get("isPrivate").data()) == 1;

		server.in_game = 1;

		insert_server(std::move(server));
	}

	int get_player_count()
	{
		std::lock_guard<std::mutex> _(mutex);
		auto count = 0;
		for (const auto& server : servers)
		{
			count += server.clients - server.bots;
		}
		return count;
	}

	int get_server_count()
	{
		std::lock_guard<std::mutex> _(mutex);
		return static_cast<int>(servers.size());
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_sp())
			{
				scheduler::once([]()
				{
					// add dvars to change destination master server ip/port
					master_server_ip = dvars::register_string("masterServerIP", "h1.auroramod.dev", game::DVAR_FLAG_NONE,
						"IP of the destination master server to connect to");
					master_server_port = dvars::register_string("masterServerPort", "20810", game::DVAR_FLAG_NONE,
						"Port of the destination master server to connect to");
				}, scheduler::pipeline::main);
			}

			if (!game::environment::is_mp())
			{
				return;
			}

			// hook LUI_OpenMenu to refresh server list for system link menu
			lui_open_menu_hook.create(game::LUI_OpenMenu, lui_open_menu_stub);

			// replace UI_RunMenuScript call in LUI_CoD_LuaCall_RefreshServerList to our refresh_servers
			utils::hook::jump(0x28E049_b, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.pushad64();
				a.call_aligned(refresh_server_list);
				a.popad64();

				a.xor_(eax, eax);
				a.mov(rbx, qword_ptr(rsp, 0x38));
				a.add(rsp, 0x20);
				a.pop(rdi);
				a.ret();
			}), true);

			utils::hook::jump(0x28E557_b, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.mov(r8d, edi);
				a.mov(ecx, eax);
				a.mov(ebx, eax);

				a.pushad64();
				a.call_aligned(join_server);
				a.popad64();

				a.jmp(0x28E563_b);
			}), true);

			utils::hook::nop(0x28E57D_b, 5);

			// do feeder stuff
			utils::hook::jump(0x28E117_b, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.mov(ecx, eax);

				a.pushad64();
				a.call_aligned(ui_feeder_count);
				a.movd(xmm0, eax);
				a.popad64();

				a.mov(rax, qword_ptr(rbx, 0x48));
				a.cvtdq2ps(xmm0, xmm0);
				a.jmp(0x28E12B_b);
			}), true);

			utils::hook::jump(0x28E331_b, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.push(rax);
				a.pushad64();
				a.mov(rcx, r9); // index
				a.mov(rdx, qword_ptr(rsp, 0x88 + 0x20)); // column
				a.call_aligned(ui_feeder_item_text);
				a.mov(qword_ptr(rsp, 0x80), rax);
				a.popad64();
				a.pop(rax);

				a.mov(rsi, qword_ptr(rsp, 0x90));
				a.mov(rdi, rax);
				a.jmp(0x28E341_b);
			}), true);

			scheduler::loop(do_frame_work, scheduler::pipeline::main);
			scheduler::loop(check_refresh, scheduler::pipeline::lui, 10ms);

			network::on("getServersResponse", [](const game::netadr_s& target, const std::string& data)
			{
				{
					std::lock_guard<std::mutex> _(mutex);
					if (!master_state.requesting || master_state.address != target)
					{
						return;
					}

					master_state.requesting = false;

					std::optional<size_t> start{};
					for (std::size_t i = 0; i + 6 < data.size(); ++i)
					{
						if (data[i + 6] == '\\')
						{
							start.emplace(i);
							break;
						}
					}

					if (!start.has_value())
					{
						return;
					}

					for (auto i = start.value(); i + 6 < data.size(); i += 7)
					{
						if (data[i + 6] != '\\')
						{
							break;
						}

						game::netadr_s address{};
						address.type = game::NA_IP;
						address.localNetID = game::NS_CLIENT1;
						std::memcpy(&address.ip[0], data.data() + i + 0, 4);
						std::memcpy(&address.port, data.data() + i + 4, 2);

						master_state.queued_servers[address] = 0;
					}
				}
			});
		}
	};
}

REGISTER_COMPONENT(server_list::component)
