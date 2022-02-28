#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "network.hpp"
#include "console.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace network
{
	SOCKET sock;

	namespace
	{

		std::unordered_map<std::string, callback>& get_callbacks()
		{
			static std::unordered_map<std::string, callback> callbacks{};
			return callbacks;
		}

		bool handle_command(game::netadr_s* address, const char* command, game::msg_t* message)
		{
			const auto cmd_string = utils::string::to_lower(command);
			auto& callbacks = get_callbacks();
			const auto handler = callbacks.find(cmd_string);
			if (handler == callbacks.end())
			{
				return false;
			}

			const auto offset = cmd_string.size() + 5;
			const std::string_view data(message->data + offset, message->cursize - offset);

			handler->second(*address, data);
#ifdef DEBUG
			console::info("[Network] Handling command %s\n", cmd_string.data());
#endif
			return true;
		}

		void handle_command_stub(utils::hook::assembler& a)
		{
			const auto return_unhandled = a.newLabel();

			a.pushad64();

			a.mov(r8, rdi); // message
			a.mov(rdx, rbx); // command
			a.mov(rcx, rsi); // netaddr

			a.call_aligned(handle_command);

			a.test(al, al);
			a.jz(return_unhandled);

			// Command handled
			a.popad64();
			a.mov(al, 1);
			a.jmp(0x140252AF8); // H1MP64(1.4)

			a.bind(return_unhandled);
			a.popad64();
			a.jmp(0x14025234C); // H1MP64(1.4)
		}

		int net_compare_base_address(const game::netadr_s* a1, const game::netadr_s* a2)
		{
			if (a1->type == a2->type)
			{
				switch (a1->type)
				{
				case game::netadrtype_t::NA_BOT:
				case game::netadrtype_t::NA_LOOPBACK:
					return a1->port == a2->port;

				case game::netadrtype_t::NA_IP:
					return !memcmp(a1->ip, a2->ip, 4);
				case game::netadrtype_t::NA_BROADCAST:
					return true;
				default:
					break;
				}
			}

			return false;
		}

		int net_compare_address(const game::netadr_s* a1, const game::netadr_s* a2)
		{
			return net_compare_base_address(a1, a2) && a1->port == a2->port;
		}

		void reconnect_migratated_client(void*, game::netadr_s* from, const int, const int, const char*,
			const char*, bool)
		{
			// This happens when a client tries to rejoin after being recently disconnected, OR by a duplicated guid
			// We don't want this to do anything. It decides to crash seemingly randomly
			// Rather than try and let the player in, just tell them they are a duplicate player and reject connection
			game::NET_OutOfBandPrint(game::NS_SERVER, from, "error\nYou are already connected to the server.");
		}
	SOCKET create_socket(const char* net_interface, int port, int protocol)
	{
		sockaddr_in address{};

		if (net_interface && net_interface != "localhost"s)
		{
			// Sys_StringToSockaddr
			utils::hook::invoke<void>(0x1404F6580, net_interface, &address);
		}

		address.sin_family = AF_INET;
		address.sin_port = ntohs(port);

		const auto sock = ::socket(AF_INET, SOCK_DGRAM, protocol);

		u_long arg = 1;
		ioctlsocket(sock, FIONBIO, &arg);
		char optval[4] = {1};
		setsockopt(sock, 0xFFFF, 32, optval, 4);

		if (bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != -1)
		{
			return sock;
		}

		closesocket(sock);
		return 0;
	}

	void on(const std::string& command, const callback& callback)
	{
		get_callbacks()[utils::string::to_lower(command)] = callback;
	}

	int dw_send_to_stub(const int size, const char* src, game::netadr_s* a3)
	{
		sockaddr s = {};
		game::NetadrToSockadr(a3, &s);
		return sendto(sock, src, size, 0, &s, 16) >= 0;
	}

	void send(const game::netadr_s& address, const std::string& command, const std::string& data, const char separator)
	{
		std::string packet = "\xFF\xFF\xFF\xFF";
		packet.append(command);
		packet.push_back(separator);
		packet.append(data);

#ifdef DEBUG
		console::info("[Network] Sending command %s\n", command.data());
#endif

		send_data(address, packet);
	}

	void send_data(const game::netadr_s& address, const std::string& data)
	{
		auto size = static_cast<int>(data.size());
		if (address.type == game::NA_LOOPBACK)
		{
			// TODO: Fix this for loopback
			if (size > 1280)
			{
				console::error("Packet was too long. Truncated!\n");
				size = 1280;
			}

			game::NET_SendLoopPacket(game::NS_CLIENT1, size, data.data(), &address);
		}
		else
		{
			game::Sys_SendPacket(size, data.data(), &address);
		}
	}

	bool are_addresses_equal(const game::netadr_s& a, const game::netadr_s& b)
	{
		return net_compare_address(&a, &b);
	}

	const char* net_adr_to_string(const game::netadr_s& a)
	{
		if (a.type == game::netadrtype_t::NA_LOOPBACK)
		{
			return "loopback";
		}

		if (a.type == game::netadrtype_t::NA_BOT)
		{
			return "bot";
		}

		if (a.type == game::netadrtype_t::NA_IP || a.type == game::netadrtype_t::NA_BROADCAST)
		{
			if (a.port)
			{
				return utils::string::va("%u.%u.%u.%u:%u", a.ip[0], a.ip[1], a.ip[2], a.ip[3], htons(a.port));
			}

			return utils::string::va("%u.%u.%u.%u", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
		}

		return "bad";
	}

	game::dvar_t* register_netport_stub(const char* dvarName, int value, int min, int max, unsigned int flags,
		const char* description)
	{
		game::dvar_t* dvar;
		dvar = dvars::register_int("net_port", 27016, 0, 0xFFFFu, game::DVAR_FLAG_LATCHED);

		// read net_port from command line
		command::read_startup_variable("net_port");

		return dvar;
	}

	utils::hook::detour bind_socket_hook;

	SOCKET bind_socket_stub(const char* net_interface, u_short port, int protocol)
	{
#ifdef DEBUG
		printf("[Socket] Attempting to create socket\n");
#endif

		sock = socket(2, 2, protocol);
		u_long argp;
		char optval;
		struct sockaddr name;

		memset(&name, 0, sizeof(name));
		name.sa_family = 2;

		if (sock == -1)
		{
#ifdef DEBUG
			printf("[Socket] Error creating socket\n");
#endif
			WSAGetLastError();
			return 0;
		}

		argp = 1;
		optval = 1;
		if (ioctlsocket(sock, -2147195266, &argp) == -1 || setsockopt(sock, 0xFFFF, 32, &optval, 4) == -1)
			return 0;

		*(WORD*)name.sa_data = ntohs(port);

		if (bind(sock, &name, 16) != -1)
		{
#ifdef DEBUG
			printf("[Socket] Socket binded!\n");
#endif
			return sock;
		}

#ifdef DEBUG
		printf("[Socket] Closing socket\n");
#endif
		closesocket(sock);
		return 0;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			{
				if (game::environment::is_sp())
				{
					return;
				}

				// creating our own variable for socket use
				bind_socket_hook.create(0x140512B40, bind_socket_stub);

				// redirect dw_sendto to raw socket
				//utils::hook::jump(0x1404D850A, reinterpret_cast<void*>(0x1404D849A));
				utils::hook::call(0x140513467, dw_send_to_stub); // H1MP64(1.4)
				utils::hook::jump(game::Sys_SendPacket, dw_send_to_stub); // H1MP64(1.4)

				// intercept command handling
				utils::hook::jump(0x140252327, utils::hook::assemble(handle_command_stub), true); // H1MP64(1.4)

				// handle xuid without secure connection
				utils::hook::nop(0x140486AAF, 6); // H1MP64(1.4)

				utils::hook::jump(0x140424F20, net_compare_address); // H1MP64(1.4)
				utils::hook::jump(0x140424F70, net_compare_base_address); // H1MP64(1.4)

				// don't establish secure conenction
				utils::hook::set<uint8_t>(0x14027EA4D, 0xEB); // H1MP64(1.4)
				utils::hook::set<uint8_t>(0x14027EB1E, 0xEB); // H1MP64(1.4)
				utils::hook::set<uint8_t>(0x14027EF8D, 0xEB); // H1MP64(1.4)
				utils::hook::set<uint8_t>(0x14025081F, 0xEB); // H1MP64(1.4)

				// ignore unregistered connection
				utils::hook::jump(0x140480F46, 0x140480EE5); // H1MP64(1.4)
				utils::hook::set<uint8_t>(0x140480F3B, 0xEB); // H1MP64(1.4)

				// disable xuid verification
				utils::hook::set<uint8_t>(0x14005B62D, 0xEB); // H1MP64(1.4)
				utils::hook::set<uint8_t>(0x14005B649, 0xEB); // H1MP64(1.4)

				// disable xuid verification
				utils::hook::nop(0x14048382C, 2);
				utils::hook::set<uint8_t>(0x140483889, 0xEB); // H1MP64(1.4)

				// ignore configstring mismatch
				utils::hook::set<uint8_t>(0x1402591C9, 0xEB); // H1MP64(1.4)

				// ignore dw handle in SV_PacketEvent
				utils::hook::set<uint8_t>(0x1404898E2, 0xEB);
				utils::hook::call(0x1404898D6, &net_compare_address); // H1MP64(1.4)

				// ignore dw handle in SV_FindClientByAddress
				utils::hook::set<uint8_t>(0x140488EFD, 0xEB);
				utils::hook::call(0x140488EF1, &net_compare_address); // H1MP64(1.4)

				// ignore dw handle in SV_DirectConnect
				utils::hook::set<uint8_t>(0x140480C58, 0xEB);
				utils::hook::set<uint8_t>(0x140480E6F, 0xEB);
				utils::hook::call(0x140480C4B, &net_compare_address);
				utils::hook::call(0x140480E62, &net_compare_address);

				// increase cl_maxpackets
				dvars::override::register_int("cl_maxpackets", 1000, 1, 1000, game::DVAR_FLAG_SAVED, true);

				// increase snaps
				dvars::override::register_int("sv_remote_client_snapshot_msec", 33, 33, 100, game::DVAR_FLAG_NONE, true);

				// ignore impure client
				utils::hook::jump(0x140481B58, reinterpret_cast<void*>(0x140481BEE)); // H1MP64(1.4)

				// don't send checksum
				utils::hook::set<uint8_t>(0x140513433, 0); // H1MP64(1.4) mov: r8d, edi ; LEN
				utils::hook::set<uint8_t>(0x14051345A, 0); // H1MP64(1.4)

				// don't read checksum
				utils::hook::set(0x1404F6620, 0xC301B0); // H1MP64(1.4)

				// don't try to reconnect client
				utils::hook::call(0x140480DFF, reconnect_migratated_client); // H1MP64(1.4)
				utils::hook::nop(0x140480DDB, 4); // H1MP64(1.4) this crashes when reconnecting for some reason

				// allow server owner to modify net_port before the socket bind
				utils::hook::call(0x140512BE5, register_netport_stub); // H1MP64(1.4)
				utils::hook::call(0x140512D20, register_netport_stub); // H1MP64(1.4)

				// increase allowed packet size
				const auto max_packet_size = 0x20000;
				utils::hook::set<int>(0x1404255F1, max_packet_size); // H1MP64(1.4)
				utils::hook::set<int>(0x140425630, max_packet_size); // H1MP64(1.4)
				utils::hook::set<int>(0x140425522, max_packet_size); // H1MP64(1.4)
				utils::hook::set<int>(0x140425545, max_packet_size); // H1MP64(1.4)

				// ignore built in "print" oob command and add in our own
				utils::hook::set<uint8_t>(0x14025280E, 0xEB); // H1MP64(1.4)
				on("print", [](const game::netadr_s&, const std::string_view& data)
				{
					const std::string message{data};
					console::info(message.data());
				});

				// Use our own socket since the game's socket doesn't work with non localhost addresses
				// why? no idea
				utils::hook::jump(0x140512B40, create_socket);
			}
		}
	};
}

REGISTER_COMPONENT(network::component)
