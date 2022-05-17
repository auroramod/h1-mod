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
			int getaddrinfo_stub(const char* name, const char* service,
				const addrinfo* hints, addrinfo** res)
			{
#ifdef DEBUG
				printf("[ network ]: [getaddrinfo]: \"%s\" \"%s\"\n", name, service);
#endif

				base_server* server = tcp_servers.find(name);
				if (!server)
				{
					server = udp_servers.find(name);
				}

				if (!server)
				{
					return getaddrinfo(name, service, hints, res);
				}

				const auto address = utils::memory::get_allocator()->allocate<sockaddr>();
				const auto ai = utils::memory::get_allocator()->allocate<addrinfo>();

				auto in_addr = reinterpret_cast<sockaddr_in*>(address);
				in_addr->sin_addr.s_addr = server->get_address();
				in_addr->sin_family = AF_INET;

				ai->ai_family = AF_INET;
				ai->ai_socktype = SOCK_STREAM;
				ai->ai_addr = address;
				ai->ai_addrlen = sizeof(sockaddr);
				ai->ai_next = nullptr;
				ai->ai_flags = 0;
				ai->ai_protocol = 0;
				ai->ai_canonname = const_cast<char*>(name);

				*res = ai;

				return 0;
			}

			void freeaddrinfo_stub(addrinfo* ai)
			{
				if (!utils::memory::get_allocator()->find(ai))
				{
					return freeaddrinfo(ai);
				}

				utils::memory::get_allocator()->free(ai->ai_addr);
				utils::memory::get_allocator()->free(ai);
			}

			int getpeername_stub(const SOCKET s, sockaddr* addr, socklen_t* addrlen)
			{
				auto* server = find_server(s);

				if (server)
				{
					auto in_addr = reinterpret_cast<sockaddr_in*>(addr);
					in_addr->sin_addr.s_addr = server->get_address();
					in_addr->sin_family = AF_INET;
					*addrlen = sizeof(sockaddr);

					return 0;
				}

				return getpeername(s, addr, addrlen);
			}

			int getsockname_stub(const SOCKET s, sockaddr* addr, socklen_t* addrlen)
			{
				auto* server = find_server(s);

				if (server)
				{
					auto in_addr = reinterpret_cast<sockaddr_in*>(addr);
					in_addr->sin_addr.s_addr = server->get_address();
					in_addr->sin_family = AF_INET;
					*addrlen = sizeof(sockaddr);

					return 0;
				}

				return getsockname(s, addr, addrlen);
			}

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

		void bd_logger_stub()
		{
			//printf("logged\n");
		}

#ifdef DEBUG
		void a(unsigned int n)
		{
			printf("bdAuth: Auth task failed with HTTP code [%u]\n", n);
		}

		void b(unsigned int n)
		{
			printf("bdAuth: Decoded client ticket of unexpected size [%u]\n", n);
		}

		void c(unsigned int n)
		{
			printf("bdAuth: Decoded server ticket of unexpected size [%u]\n", n);
		}

		void d()
		{
			printf("bdAuth: Auth ticket magic number mismatch\n");
		}

		void e()
		{
			printf("bdAuth: Cross Authentication completed\n");
		}

		void f()
		{
			printf("bdAuth: Auth task reply contains invalid data / format\n");
		}

		void g(unsigned int n)
		{
			printf("bdAuth: Auth task returned with error code [%u]\n", n);
		}

		void h(unsigned int n)
		{
			printf("bdAuth: Invalid or No Task ID [%u] in Auth reply\n", n);
		}

		void i()
		{
			printf("bdAuth: Received reply from DemonWare Auth server\n");
		}

		void l()
		{
			printf("bdAuth: Unknown error\n");
		}
#endif

		utils::hook::detour kekw_hook;
		bool kekw_stub(__int64 a1, __int64 a2, __int64* a3)
		{
			// Checks X-Signature header or something
			utils::hook::set(0x7D4AB0_b, 0xC301B0);
			// Checks extended_data and extra_data in json object
			utils::hook::set(0x7D55C0_b, 0xC301B0);
			return kekw_hook.invoke<bool>(a1, a2, a3);
		}

		void* allocate_somewhere_near(uint8_t* base_address)
		{
			const size_t PAGE_SIZE = 0x1000;
			size_t offset = 0;
			while (true)
			{
				offset += PAGE_SIZE;
				auto res = VirtualAlloc(base_address - offset, PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				if (res)
				{
					std::memset(res, 0, PAGE_SIZE);
					return res;
				}
			}
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
				if (function == "#5") return io::getpeername_stub;
				if (function == "#6") return io::getsockname_stub;
				if (function == "#10") return io::ioctlsocket_stub;
				if (function == "#16") return io::recv_stub;
				if (function == "#17") return io::recvfrom_stub;
				if (function == "#18") return io::select_stub;
				if (function == "#19") return io::send_stub;
				if (function == "#20") return io::sendto_stub;
				if (function == "#52") return io::gethostbyname_stub;
				if (function == "getaddrinfo") return io::getaddrinfo_stub;
				if (function == "freeaddrinfo") return io::freeaddrinfo_stub;
			}

			if (function == "InternetGetConnectedState")
			{
				return io::internet_get_connected_state_stub;
			}

			return nullptr;
		}

		void post_unpack() override
		{
			/*
				mwr has upgraded some networking methods and the gethostbyname import from winsock library is no longer used
				gethostbyname has been replaced with getaddrinfo
				btw, still you can't get online..
			*/
			//utils::hook::jump(SELECT_VALUE(0, 0x7EBC20_b), bd_logger_stub);

			if (game::environment::is_sp())
			{
				//	utils::hook::set<uint8_t>(0x1405FCA00, 0xC3); // bdAuthSteam H1(1.4)
				//	utils::hook::set<uint8_t>(0x140333A00, 0xC3); // dwNet H1(1.4)
				return;
			}

			utils::hook::set<uint8_t>(0x7C0AD9_b, 0x0);  // CURLOPT_SSL_VERIFYPEER H1MP64(1.15)
			utils::hook::set<uint8_t>(0x7C0AC5_b, 0xAF); // CURLOPT_SSL_VERIFYHOST H1MP64(1.15)
			utils::hook::set<uint8_t>(0xA1327C_b, 0x0);  // HTTPS -> HTTP              [MWR OK][S1X: 0x14088D0E8]

			//HTTPS -> HTTP
			char* umbrella = (char*)allocate_somewhere_near((uint8_t*)game::base_address);
			std::memcpy(umbrella, "http://prod.umbrella.demonware.net/v1.0/", sizeof("http://prod.umbrella.demonware.net/v1.0/"));

			utils::hook::inject(0x8615F_b, umbrella);
			utils::hook::inject(0x8638C_b, umbrella);

			char* uno = (char*)allocate_somewhere_near((uint8_t*)game::base_address);
			std::memcpy(uno, "http://prod.uno.demonware.net/v1.0/", sizeof("http://prod.uno.demonware.net/v1.0/"));

			utils::hook::inject(0x86C56_b, uno);
			utils::hook::inject(0x86F96_b, uno);

			BYTE bytes[] = { 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x25, 0x73, 0x3A, 0x25, 0x64, 0x2F, 0x61, 0x75, 0x74, 0x68, 0x2F, 0x0 }; // KEKW
			std::memcpy((void*)0x9EDB08_b, bytes, sizeof(bytes)); //utils::hook::inject(0x140728170, "http://%s:%d/auth/"); :DDD

			//utils::hook::set<uint8_t>(0x14047F290, 0xC3); // SV_SendMatchData H1MP64(1.4)
			//utils::hook::set<uint8_t>(0x140598990, 0xC3); // Live_CheckForFullDisconnect H1MP64(1.4)

//#ifdef DEBUG
//			// yes
//			utils::hook::call(0x140727BEB, l);
//			utils::hook::call(0x140727AFC, i);
//			utils::hook::call(0x140727E49, h);
//			utils::hook::call(0x140727E30, g);
//			utils::hook::call(0x140727E37, f);
//			utils::hook::call(0x140727DF2, e);
//			utils::hook::call(0x140727DF9, d);
//			utils::hook::call(0x140727CFC, c);
//			utils::hook::call(0x140727C82, b);
//			utils::hook::call(0x140727E6A, a);
//#endif
			// Checks X-Signature header or something
			//utils::hook::set(0x7D4AB0_b, 0xC301B0);
			// Checks extended_data and extra_data in json object
			//utils::hook::set(0x7D55C0_b, 0xC301B0);
			// Update check
			//utils::hook::set(0x1403A5390, 0xC301B0);

			// Remove some while loop in demonware that freezes the rendering for a few secs at launch
			//utils::hook::nop(0x14057DBC5, 5);

			MessageBoxA(0, "TEST", "", 0);
			kekw_hook.create(0x7AC600_b, kekw_stub);
			MessageBoxA(0, "TEST2", "", 0);
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
