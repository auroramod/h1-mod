#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdRichPresence::bdRichPresence() : service(68, "bdRichPresence")
	{
		this->register_task(1, &bdRichPresence::unk1);
		this->register_task(2, &bdRichPresence::unk2);
	}

	void bdRichPresence::unk1(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdRichPresence::unk2(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
