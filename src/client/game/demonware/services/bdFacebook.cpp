#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdFacebook::bdFacebook() : service(36, "bdFacebook")
	{
		this->register_task(1, &bdFacebook::unk1);
		this->register_task(3, &bdFacebook::unk3);
		this->register_task(7, &bdFacebook::unk7);
		this->register_task(8, &bdFacebook::unk8);
	}

	void bdFacebook::unk1(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdFacebook::unk3(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdFacebook::unk7(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdFacebook::unk8(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
