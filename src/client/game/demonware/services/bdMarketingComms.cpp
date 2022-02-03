#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdMarketingComms::bdMarketingComms() : service(104, "bdMarketingComms")
	{
		this->register_task(1, &bdMarketingComms::get_messages);
	}

	void bdMarketingComms::get_messages(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
