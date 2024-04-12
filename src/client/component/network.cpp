#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "network.hpp"
#include "console.hpp"
#include "dvars.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace network
{
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
			const auto offset = cmd_string.size() + 5;
			if (message->cursize < offset || handler == callbacks.end())
			{
				return false;
			}

			const std::string data(message->data + offset, message->cursize - offset);

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
			a.jmp(0x12FCAA_b);

			a.bind(return_unhandled);
			a.popad64();
			a.jmp(0x12F3AC_b);
		}

		int net_compare_base_address(const game::netadr_s* a, const game::netadr_s* b)
		{
			if (a->type == b->type)
			{
				switch (a->type)
				{
				case game::netadrtype_t::NA_BOT:
				case game::netadrtype_t::NA_LOOPBACK:
					return a->port == b->port;

				case game::netadrtype_t::NA_IP:
					return !memcmp(a->ip, b->ip, 4);
				case game::netadrtype_t::NA_BROADCAST:
					return true;
				default:
					break;
				}
			}

			return false;
		}

		int net_compare_address(const game::netadr_s* a, const game::netadr_s* b)
		{
			return net_compare_base_address(a, b) && a->port == b->port;
		}

		void reconnect_migrated_client(void*, game::netadr_s* from, const int, const int, const char*,
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
				utils::hook::invoke<void>(0x59E810_b, net_interface, &address);
			}

			address.sin_family = AF_INET;
			address.sin_port = ntohs(static_cast<short>(port));

			const auto sock = socket(AF_INET, SOCK_DGRAM, protocol);

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

		void* memmove_stub(void* dst, void* src, size_t size)
		{
			return std::memmove(dst, src, std::min(size, 1262ull));
		}
	}

	void on(const std::string& command, const callback& callback)
	{
		get_callbacks()[utils::string::to_lower(command)] = callback;
	}

	int dw_send_to_stub(const int size, const char* src, game::netadr_s* to)
	{
		sockaddr s = {};
		game::NetadrToSockadr(to, &s);
		return sendto(*game::query_socket, src, size, 0, &s, 16) >= 0;
	}

	void send(const game::netadr_s& address, const std::string& command, const std::string& data, const char separator)
	{
		std::string packet = "\xFF\xFF\xFF\xFF";
		packet.append(command);
		packet.push_back(separator);
		packet.append(data);

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
		dvar = dvars::register_int("net_port", 27016, 0, 0xFFFFu, game::DVAR_FLAG_LATCHED, "Network port");

		// read net_port from command line
		command::read_startup_variable("net_port");

		return dvar;
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

				// redirect dw_sendto to raw socket
				utils::hook::jump(0x5EEC90_b, dw_send_to_stub);
				utils::hook::jump(game::Sys_SendPacket, dw_send_to_stub);

				// intercept command handling
				utils::hook::jump(0x12F387_b, utils::hook::assemble(handle_command_stub), true);

				// handle xuid without secure connection
				utils::hook::nop(0x554222_b, 6);

				utils::hook::jump(0x4F1800_b, net_compare_address);
				utils::hook::jump(0x4F1850_b, net_compare_address);

				// don't establish secure conenction
				utils::hook::set<uint8_t>(0x358C8D_b, 0xEB);
				utils::hook::set<uint8_t>(0x358D5E_b, 0xEB);
				utils::hook::set<uint8_t>(0x3591CD_b, 0xEB);
				utils::hook::set<uint8_t>(0x12CD0F_b, 0xEB);

				// ignore unregistered connection
				utils::hook::jump(0x54E2D1_b, 0x54E270_b, true);
				utils::hook::set<uint8_t>(0x54E2C6_b, 0xEB);

				// disable xuid verification
				utils::hook::set<uint8_t>(0x728BF_b, 0xEB);

				// disable xuid verification
				utils::hook::nop(0x5509D9_b, 2);
				utils::hook::set<uint8_t>(0x550A36_b, 0xEB);

				// ignore configstring mismatch
				utils::hook::set<uint8_t>(0x341261_b, 0xEB);

				// ignore dw handle in SV_PacketEvent
				utils::hook::set<uint8_t>(0x1CBC22_b, 0xEB);

				// ignore dw handle in SV_FindClientByAddress
				utils::hook::set<uint8_t>(0x1CB24D_b, 0xEB);

				// ignore dw handle in SV_DirectConnect
				utils::hook::set<uint8_t>(0x54DFE8_b, 0xEB);
				utils::hook::set<uint8_t>(0x54E1FD_b, 0xEB);

				// increase cl_maxpackets
				dvars::override::register_int("cl_maxpackets", 1000, 1, 1000, game::DVAR_FLAG_SAVED);

				// increase snaps
				dvars::override::register_int("sv_remote_client_snapshot_msec", 33, 33, 100, game::DVAR_FLAG_NONE);

				// ignore impure client
				utils::hook::jump(0x54EDD3_b, 0x54EE69_b);

				// don't send checksum
				utils::hook::set<uint8_t>(0x59E628_b, 0);

				// don't read checksum
				utils::hook::set(0x59E8B0_b, 0xC301B0);

				// don't try to reconnect client
				utils::hook::jump(0x54D220_b, reconnect_migrated_client);
				utils::hook::nop(0x54E168_b, 4); // this crashes when reconnecting for some reason

				// allow server owner to modify net_port before the socket bind
				utils::hook::call(0x5BD032_b, register_netport_stub);
				utils::hook::call(0x5BD3F0_b, register_netport_stub);

				// increase allowed packet size
				const auto max_packet_size = 0x20000;
				utils::hook::set<int>(0x4F1ED1_b, max_packet_size);
				utils::hook::set<int>(0x4F1F10_b, max_packet_size);
				utils::hook::set<int>(0x4F1E02_b, max_packet_size);
				utils::hook::set<int>(0x4F1E25_b, max_packet_size);

				// ignore built in "print" oob command and add in our own
				utils::hook::set<std::uint8_t>(0x12F817_b, 0xEB);

				// Use our own socket since the game's socket doesn't work with non localhost addresses
				// why? no idea
				utils::hook::jump(0x5BD210_b, create_socket);

				// patch buffer overflow
				utils::hook::call(0x4F19A7_b, memmove_stub); // NET_DeferPacketToClient
			}
		}
	};
}

REGISTER_COMPONENT(network::component)
