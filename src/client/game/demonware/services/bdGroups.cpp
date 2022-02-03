#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdGroups::bdGroups() : service(28, "bdGroup")
	{
		this->register_task(1, &bdGroups::set_groups);
		this->register_task(4, &bdGroups::unk4);
	}

	void bdGroups::set_groups(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdGroups::unk4(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
