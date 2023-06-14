#include <std_include.hpp>

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

		const int timeout = 7; // seconds

		std::optional<utils::http::result> motd_content;
		std::optional<utils::http::result> featured_content;
		std::optional<utils::http::result> featured2_content;

		auto get_motd = [&motd_content]()
		{
			motd_content = utils::http::get_data("https://h1.gg/data/motd.json", {}, {}, {}, timeout);
		};
		auto get_featured = [&featured_content]()
		{
			featured_content = utils::http::get_data("https://h1.gg/data/featured.json", {}, {}, {}, timeout);
		};
		auto get_featured2 = [&featured2_content]()
		{
			featured2_content = utils::http::get_data("https://h1.gg/data/featured2.json", {}, {}, {}, timeout);
		};

		std::thread get_motd_thread(get_motd);
		std::thread get_featured_thread(get_featured);
		std::thread get_featured2_thread(get_featured2);

		get_motd_thread.join();
		get_featured_thread.join();
		get_featured2_thread.join();

		if (motd_content.has_value())
		{
			const auto motd = new bdMarketingMessage;
			motd->m_messageID = 1;
			motd->m_languageCode = "en-US";
			motd->m_content = motd_content.value().buffer.data();
			motd->m_metadata = "{}";
			reply->add(motd);
		}

		if (featured_content.has_value())
		{
			const auto featured = new bdMarketingMessage;
			featured->m_messageID = 2;
			featured->m_languageCode = "en-US";
			featured->m_content = featured_content.value().buffer.data();
			featured->m_metadata = "{}";
			reply->add(featured);
		}

		if (featured2_content.has_value())
		{
			const auto featured2 = new bdMarketingMessage;
			featured2->m_messageID = 3;
			featured2->m_languageCode = "en-US";
			featured2->m_content = featured2_content.value().buffer.data();
			featured2->m_metadata = "{}";
			reply->add(featured2);
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
