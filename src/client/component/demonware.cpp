#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>
#include <utils/thread.hpp>

#include "game/game.hpp"
#include "game/demonware/servers/lobby_server.hpp"
#include "game/demonware/servers/auth3_server.hpp"
#include "game/demonware/servers/stun_server.hpp"
#include "game/demonware/servers/umbrella_server.hpp"
#include "game/demonware/server_registry.hpp"
#include <game/dvars.hpp>

#define TCP_BLOCKING true
#define UDP_BLOCKING false

namespace demonware
{
	namespace
	{
		volatile bool exit_server;
		std::thread server_thread;
		utils::concurrency::container<std::unordered_map<SOCKET, bool>> blocking_sockets;
		utils::concurrency::container<std::unordered_map<SOCKET, tcp_server*>> socket_map;
		server_registry<tcp_server> tcp_servers;
		server_registry<udp_server> udp_servers;

		tcp_server* find_server(const SOCKET socket)
		{
			return socket_map.access<tcp_server*>([&](const std::unordered_map<SOCKET, tcp_server*>& map) -> tcp_server*
				{
					const auto entry = map.find(socket);
					if (entry == map.end())
					{
						return nullptr;
					}

					return entry->second;
				});
		}

		bool socket_link(const SOCKET socket, const uint32_t address)
		{
			auto* server = tcp_servers.find(address);
			if (!server)
			{
				return false;
			}

			socket_map.access([&](std::unordered_map<SOCKET, tcp_server*>& map)
				{
					map[socket] = server;
				});

			return true;
		}

		void socket_unlink(const SOCKET socket)
		{
			socket_map.access([&](std::unordered_map<SOCKET, tcp_server*>& map)
				{
					const auto entry = map.find(socket);
					if (entry != map.end())
					{
						map.erase(entry);
					}
				});
		}

		bool is_socket_blocking(const SOCKET socket, const bool def)
		{
			return blocking_sockets.access<bool>([&](std::unordered_map<SOCKET, bool>& map)
				{
					const auto entry = map.find(socket);
					if (entry == map.end())
					{
						return def;
					}

					return entry->second;
				});
		}

		void remove_blocking_socket(const SOCKET socket)
		{
			blocking_sockets.access([&](std::unordered_map<SOCKET, bool>& map)
				{
					const auto entry = map.find(socket);
					if (entry != map.end())
					{
						map.erase(entry);
					}
				});
		}

		void add_blocking_socket(const SOCKET socket, const bool block)
		{
			blocking_sockets.access([&](std::unordered_map<SOCKET, bool>& map)
				{
					map[socket] = block;
				});
		}

		void server_main()
		{
			exit_server = false;

			while (!exit_server)
			{
				tcp_servers.frame();
				udp_servers.frame();
				std::this_thread::sleep_for(50ms);
			}
		}

		namespace io
		{
			hostent* gethostbyname_stub(const char* name)
			{
#ifdef DEBUG
				printf("[ network ]: [gethostbyname]: \"%s\"\n", name);
#endif

				base_server* server = tcp_servers.find(name);
				if (!server)
				{
					server = udp_servers.find(name);
				}

				if (!server)
				{
#pragma warning(push)
#pragma warning(disable: 4996)
					return gethostbyname(name);
#pragma warning(pop)
				}

				static thread_local in_addr address{};
				address.s_addr = server->get_address();

				static thread_local in_addr* addr_list[2]{};
				addr_list[0] = &address;
				addr_list[1] = nullptr;

				static thread_local hostent host{};
				host.h_name = const_cast<char*>(name);
				host.h_aliases = nullptr;
				host.h_addrtype = AF_INET;
				host.h_length = sizeof(in_addr);
				host.h_addr_list = reinterpret_cast<char**>(addr_list);

				return &host;
			}

			int connect_stub(const SOCKET s, const struct sockaddr* addr, const int len)
			{
				if (len == sizeof(sockaddr_in))
				{
					const auto* in_addr = reinterpret_cast<const sockaddr_in*>(addr);
					if (socket_link(s, in_addr->sin_addr.s_addr)) return 0;
				}

				return connect(s, addr, len);
			}

			int closesocket_stub(const SOCKET s)
			{
				remove_blocking_socket(s);
				socket_unlink(s);

				return closesocket(s);
			}

			int send_stub(const SOCKET s, const char* buf, const int len, const int flags)
			{
				auto* server = find_server(s);

				if (server)
				{
					server->handle_input(buf, len);
					return len;
				}

				return send(s, buf, len, flags);
			}

			int recv_stub(const SOCKET s, char* buf, const int len, const int flags)
			{
				auto* server = find_server(s);

				if (server)
				{
					if (server->pending_data())
					{
						return static_cast<int>(server->handle_output(buf, len));
					}
					else
					{
						WSASetLastError(WSAEWOULDBLOCK);
						return -1;
					}
				}

				return recv(s, buf, len, flags);
			}

			int sendto_stub(const SOCKET s, const char* buf, const int len, const int flags, const sockaddr* to,
				const int tolen)
			{
				const auto* in_addr = reinterpret_cast<const sockaddr_in*>(to);
				auto* server = udp_servers.find(in_addr->sin_addr.s_addr);

				if (server)
				{
					server->handle_input(buf, len, { s, to, tolen });
					return len;
				}

				return sendto(s, buf, len, flags, to, tolen);
			}

			int recvfrom_stub(const SOCKET s, char* buf, const int len, const int flags, struct sockaddr* from,
				int* fromlen)
			{
				// Not supported yet
				if (is_socket_blocking(s, UDP_BLOCKING))
				{
					return recvfrom(s, buf, len, flags, from, fromlen);
				}

				size_t result = 0;
				udp_servers.for_each([&](udp_server& server)
					{
						if (server.pending_data(s))
						{
							result = server.handle_output(
								s, buf, static_cast<size_t>(len), from, fromlen);
						}
					});

				if (result)
				{
					return static_cast<int>(result);
				}

				return recvfrom(s, buf, len, flags, from, fromlen);
			}

			int select_stub(const int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
				struct timeval* timeout)
			{
				if (exit_server)
				{
					return select(nfds, readfds, writefds, exceptfds, timeout);
				}

				auto result = 0;
				std::vector<SOCKET> read_sockets;
				std::vector<SOCKET> write_sockets;

				socket_map.access([&](std::unordered_map<SOCKET, tcp_server*>& sockets)
					{
						for (auto& s : sockets)
						{
							if (readfds)
							{
								if (FD_ISSET(s.first, readfds))
								{
									if (s.second->pending_data())
									{
										read_sockets.push_back(s.first);
										FD_CLR(s.first, readfds);
									}
								}
							}

							if (writefds)
							{
								if (FD_ISSET(s.first, writefds))
								{
									write_sockets.push_back(s.first);
									FD_CLR(s.first, writefds);
								}
							}

							if (exceptfds)
							{
								if (FD_ISSET(s.first, exceptfds))
								{
									FD_CLR(s.first, exceptfds);
								}
							}
						}
					});

				if ((!readfds || readfds->fd_count == 0) && (!writefds || writefds->fd_count == 0))
				{
					timeout->tv_sec = 0;
					timeout->tv_usec = 0;
				}

				result = select(nfds, readfds, writefds, exceptfds, timeout);
				if (result < 0) result = 0;

				for (const auto& socket : read_sockets)
				{
					if (readfds)
					{
						FD_SET(socket, readfds);
						result++;
					}
				}

				for (const auto& socket : write_sockets)
				{
					if (writefds)
					{
						FD_SET(socket, writefds);
						result++;
					}
				}

				return result;
			}

			int ioctlsocket_stub(const SOCKET s, const long cmd, u_long* argp)
			{
				if (static_cast<unsigned long>(cmd) == (FIONBIO))
				{
					add_blocking_socket(s, *argp == 0);
				}

				return ioctlsocket(s, cmd, argp);
			}

			BOOL internet_get_connected_state_stub(LPDWORD, DWORD)
			{
				// Allow offline play
				return TRUE;
			}
		}

		void bd_logger_stub(const char* const function, const char* const msg, ...)
		{
			game::dvar_t* enabled;

			enabled = dvars::register_bool("bd_logger_enabled", false, game::DVAR_FLAG_SAVED, true);


			//game::Dvar_RegisterBool("bd_logger_enabled", false, game::DVAR_FLAG_SAVED, "bdLogger")
			if (!enabled->current.enabled)
			{
				return;
			}

			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);
			printf("%s: %s\n", function, buffer);

			va_end(ap);
		}
	}

	class component final : public component_interface
	{
	public:
		component()
		{
			udp_servers.create<stun_server>("phoenix.stun.us.demonware.net");
			udp_servers.create<stun_server>("phoenix.stun.eu.demonware.net");
			udp_servers.create<stun_server>("phoenix.stun.jp.demonware.net");
			udp_servers.create<stun_server>("phoenix.stun.au.demonware.net");

			udp_servers.create<stun_server>("stun.us.demonware.net");
			udp_servers.create<stun_server>("stun.eu.demonware.net");
			udp_servers.create<stun_server>("stun.jp.demonware.net");
			udp_servers.create<stun_server>("stun.au.demonware.net");

			tcp_servers.create<auth3_server>("mwr-pc-steam-auth3.prod.demonware.net");
			tcp_servers.create<lobby_server>("mwr-pc-steam-lobby.prod.demonware.net");
			tcp_servers.create<umbrella_server>("prod.umbrella.demonware.net");
		}

		void post_load() override
		{
			server_thread = utils::thread::create_named_thread("Demonware", server_main);
		}

		void* load_import(const std::string& library, const std::string& function) override
		{
			if (library == "WS2_32.dll")
			{
				if (function == "#3") return io::closesocket_stub;
				if (function == "#4") return io::connect_stub;
				if (function == "#10") return io::ioctlsocket_stub;
				if (function == "#16") return io::recv_stub;
				if (function == "#17") return io::recvfrom_stub;
				if (function == "#18") return io::select_stub;
				if (function == "#19") return io::send_stub;
				if (function == "#20") return io::sendto_stub;
				if (function == "#52") return io::gethostbyname_stub;
			}

			if (function == "InternetGetConnectedState")
			{
				return io::internet_get_connected_state_stub;
			}

			return nullptr;
		}

		void post_unpack() override
		{
			utils::hook::jump(SELECT_VALUE(0, 0x1407400B0), bd_logger_stub); // H1MP64(1.4)

	   //singleplayer not supported so far.
			if (game::environment::is_sp())
			{
				utils::hook::set<uint8_t>(0x1405632E0, 0xC3); // bdAuthSteam
				utils::hook::set<uint8_t>(0x1402DF2C0, 0xC3); // dwNet
				return;
			}

			utils::hook::set<uint8_t>(0x140715039, 0x0); // CURLOPT_SSL_VERIFYPEER H1MP64(1.4)
			utils::hook::set<uint8_t>(0x140715025, 0xAF); // CURLOPT_SSL_VERIFYHOST H1MP64(1.4)
			utils::hook::set<uint8_t>(0x14095433C, 0x0); // HTTPS -> HTTP              [MWR OK][S1X: 0x14088D0E8]

			//HTTPS -> HTTP
			utils::hook::inject(0x14006DDA9, "http://prod.umbrella.demonware.net/v1.0/"); // --->  [H1MP1.4 - S1X: 0x14003852E]
			utils::hook::inject(0x14006E11C, "http://prod.umbrella.demonware.net/v1.0/"); // --->  [H1MP1.4 - S1X: 0x14003884F]
			utils::hook::inject(0x14006E2FB, "http://prod.umbrella.demonware.net/v1.0/"); // --->  [H1MP1.4 - S1X: 0x140038A07]

			utils::hook::set<uint8_t>(0x14047F290, 0xC3); // SV_SendMatchData H1MP64(1.4)
			utils::hook::set<uint8_t>(0x140598990, 0xC3); // Live_CheckForFullDisconnect H1MP64(1.4)
		}

		void pre_destroy() override
		{
			exit_server = true;
			if (server_thread.joinable())
			{
				server_thread.join();
			}
		}
	};
}

REGISTER_COMPONENT(demonware::component)