#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdEventLog::bdEventLog() : service(67, "bdEventLog")
	{
		this->register_task(6, &bdEventLog::unk6);
	}

	void bdEventLog::unk6(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
