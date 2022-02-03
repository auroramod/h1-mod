#pragma once
#include "game/game.hpp"

namespace network
{
	using callback = std::function<void(const game::netadr_s&, const std::string_view&)>;

	void on(const std::string& command, const callback& callback);
	void send(const game::netadr_s& address, const std::string& command, const std::string& data = {}, char separator = ' ');
	void send_data(const game::netadr_s& address, const std::string& data);

	bool are_addresses_equal(const game::netadr_s& a, const game::netadr_s& b);

	const char* net_adr_to_string(const game::netadr_s& a);
}

inline bool operator==(const game::netadr_s& a, const game::netadr_s& b)
{
	return network::are_addresses_equal(a, b); //
}

inline bool operator!=(const game::netadr_s& a, const game::netadr_s& b)
{
	return !(a == b); //
}

namespace std
{
	template <>
	struct equal_to<game::netadr_s>
	{
		using result_type = bool;

		bool operator()(const game::netadr_s& lhs, const game::netadr_s& rhs) const
		{
			return network::are_addresses_equal(lhs, rhs);
		}
	};

	template <>
	struct hash<game::netadr_s>
	{
		size_t operator()(const game::netadr_s& x) const noexcept
		{
			return hash<uint32_t>()(*reinterpret_cast<const uint32_t*>(&x.ip[0])) ^ hash<uint16_t>()(x.port);
		}
	};
}
