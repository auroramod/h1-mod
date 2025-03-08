#include <std_include.hpp>

#include "component/motd.hpp"
#include "component/scheduler.hpp"

#include <utils/http.hpp>
#include "../services.hpp"

namespace demonware
{
	bdMarketingComms::bdMarketingComms() : service(104, "bdMarketingComms")
	{
		this->register_task(1, &bdMarketingComms::getMessages);
		this->register_task(4, &bdMarketingComms::reportFullMessagesViewed);
		this->register_task(5, &bdMarketingComms::getMessages_);
	}

	void bdMarketingComms::getMessages(service_server* server, byte_buffer* /*buffer*/) const
	{
		auto reply = server->create_reply(this->task_id());

		int message_id = 1;
		const auto featured_content = motd::get_featured_content();
		for (const auto& [key, value] : featured_content)
		{
			const auto marketing_message = new bdMarketingMessage;
			marketing_message->m_messageID = message_id;
			marketing_message->m_languageCode = "en-US";
			marketing_message->m_content = value.buffer;
			marketing_message->m_metadata = "{}";
			reply->add(marketing_message);
			++message_id;
		}

		reply->send();
	}

	void bdMarketingComms::reportFullMessagesViewed(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}

	void bdMarketingComms::getMessages_(service_server* server, byte_buffer* /*buffer*/) const
	{
		// TODO:
		auto reply = server->create_reply(this->task_id());
		reply->send();
	}
}
