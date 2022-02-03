#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdProfiles::bdProfiles() : service(8, "bdProfiles")
	{
		this->register_task(3, &bdProfiles::unk3);
	}

	void bdProfiles::unk3(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
