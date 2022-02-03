#include <std_include.hpp>
#include "stun_server.hpp"

#include "../byte_buffer.hpp"

namespace demonware
{
	void stun_server::handle(const endpoint_data& endpoint, const std::string& packet)
	{
		uint8_t type, version, padding;

		byte_buffer buffer(packet);
		buffer.set_use_data_types(false);
		buffer.read_byte(&type);
		buffer.read_byte(&version);
		buffer.read_byte(&padding);

		switch (type)
		{
		case 30:
			this->ip_discovery(endpoint);
			break;
		case 20:
			this->nat_discovery(endpoint);
			break;
		default:
			break;
		}
	}

	void stun_server::ip_discovery(const endpoint_data& endpoint)
	{
		const uint32_t ip = 0x0100007f;

		byte_buffer buffer;
		buffer.set_use_data_types(false);
		buffer.write_byte(31); // type
		buffer.write_byte(2); // version
		buffer.write_byte(0); // version
		buffer.write_uint32(ip); // external ip
		buffer.write_uint16(3074); // port

		this->send(endpoint, buffer.get_buffer());
	}

	void stun_server::nat_discovery(const endpoint_data& endpoint)
	{
		const uint32_t ip = 0x0100007f;

		byte_buffer buffer;
		buffer.set_use_data_types(false);
		buffer.write_byte(21); // type
		buffer.write_byte(2); // version
		buffer.write_byte(0); // version
		buffer.write_uint32(ip); // external ip
		buffer.write_uint16(3074); // port
		buffer.write_uint32(this->get_address()); // server ip
		buffer.write_uint16(3074); // server port

		this->send(endpoint, buffer.get_buffer());
	}
}
