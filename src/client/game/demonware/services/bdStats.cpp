#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdStats::bdStats() : service(4, "bdStats")
	{
		this->register_task(1, &bdStats::unk1);
		this->register_task(3, &bdStats::unk3); // leaderboards
		this->register_task(4, &bdStats::unk4);
		this->register_task(8, &bdStats::unk8);
		this->register_task(11, &bdStats::unk11);
	}

	void bdStats::unk1(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdStats::unk3(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdStats::unk4(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdStats::unk8(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdStats::unk11(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
