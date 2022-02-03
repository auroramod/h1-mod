#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdAnticheat::bdAnticheat() : service(38, "bdAnticheat")
	{
		this->register_task(2, &bdAnticheat::unk2);
		this->register_task(4, &bdAnticheat::report_console_details);
	}

	void bdAnticheat::unk2(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO: Read data as soon as needed
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdAnticheat::report_console_details(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO: Read data as soon as needed
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
