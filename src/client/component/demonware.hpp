#pragma once
#include "loader/component_loader.hpp"

namespace demonware
{
	class tcp_server;
}

class demonware_component final : public component_interface
{
public:
	demonware_component();

	void post_load() override;
	void post_unpack() override;
	void pre_destroy() override;
	void* load_import(const std::string& library, const std::string& function) override;

private:
	static demonware::tcp_server* find_server(const SOCKET socket);
	static bool socket_link(const SOCKET socket, const uint32_t address);
	static void socket_unlink(const SOCKET socket);
	static bool is_socket_blocking(const SOCKET socket, const bool def);
	static void remove_blocking_socket(const SOCKET socket);
	static void add_blocking_socket(const SOCKET socket, const bool block);
	static void server_main();
	static int getaddrinfo_stub(const char* name, const char* service, const addrinfo* hints, addrinfo** res);
	static void freeaddrinfo_stub(addrinfo* ai);
	static int getpeername_stub(const SOCKET s, sockaddr* addr, socklen_t* addrlen);
	static int getsockname_stub(const SOCKET s, sockaddr* addr, socklen_t* addrlen);
	static hostent* gethostbyname_stub(const char* name);
	static int connect_stub(const SOCKET s, const struct sockaddr* addr, const int len);
	static int closesocket_stub(const SOCKET s);
	static int send_stub(const SOCKET s, const char* buf, const int len, const int flags);
	static int recv_stub(const SOCKET s, char* buf, const int len, const int flags);
	static int sendto_stub(const SOCKET s, const char* buf, const int len, const int flags, const sockaddr* to, const int tolen);
	static int recvfrom_stub(const SOCKET s, char* buf, const int len, const int flags, struct sockaddr* from, int* fromlen);
	static int select_stub(const int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
	static int ioctlsocket_stub(const SOCKET s, const long cmd, u_long* argp);
	static BOOL internet_get_connected_state_stub(LPDWORD, DWORD);
	static void bd_logger_stub();
	static void request_start_match_stub();
};
