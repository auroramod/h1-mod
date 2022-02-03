#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdCounters::bdCounters() : service(23, "bdCounters")
	{
		this->register_task(1, &bdCounters::unk1);
		this->register_task(2, &bdCounters::unk2);
	}

	void bdCounters::unk1(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdCounters::unk2(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
