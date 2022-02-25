#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdUNK95::bdUNK95() : service(95, "bdUNK95")
	{
		this->register_task(1, &bdUNK95::unk1);
	}

	void bdUNK95::unk1(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
