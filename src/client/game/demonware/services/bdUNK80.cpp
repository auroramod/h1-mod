#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdUNK80::bdUNK80() : service(80, "bdUNK80")
	{
		this->register_task(42, &bdUNK80::unk42); // COD POINTS purchase ?
		// this->register_task(43, &bdUNK80::unk43); COD POINTS purchase ?
		this->register_task(49, &bdUNK80::unk49);
		this->register_task(60, &bdUNK80::unk60);
		this->register_task(130, &bdUNK80::unk130);
		this->register_task(165, &bdUNK80::unk165);
		this->register_task(193, &bdUNK80::unk193);
		this->register_task(232, &bdUNK80::unk232);
	}

	void bdUNK80::unk42(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdUNK80::unk49(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdUNK80::unk60(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdUNK80::unk130(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdUNK80::unk165(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdUNK80::unk193(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdUNK80::unk232(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
