#include <std_include.hpp>
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
		auto motd = new bdMarketingMessage;
		motd->m_messageID = 1;
		motd->m_languageCode = "en-US";
		motd->m_content = R"({
"title": "<h2>Message of the Day</h2>",
"content_short": "",
"h3_header": "<h3>^2Welcome to H1-Mod^7</h3>",
"content_long": "<body>Join our discord to stay tuned for latest updates and news: ^5https://discord.gg/RzzXu5EVnh^7<br/><br/>Visit ^2https://h1.gg^7 for further information!</body>",
"popup_action_title": "Press 'Esc' or 'Enter' to close",
"popup_image": "h1_motd_template_1_2"
})";
		motd->m_metadata = "{}";

		auto featured = new bdMarketingMessage;
		featured->m_messageID = 2;
		featured->m_languageCode = "en-US";
		featured->m_content = R"({
    "action_title": "v2.0.0",
    "image": "h1_motd_360x200_mp_crash",
    "title": "<h1>v2.0.0</h1>",
    "content_short": "",
    "h3_header": "<h3>CHANGELOG:</h3>",
    "content_long": "<body>- Added Wine/Proton compatibility<br/>- Added custom zones/maps/weapons support<br/>- Added usermap/mod downloading<br/>- Added GSC support<br/><br/>You can check the whole changelog via: ^2https://docs.h1.gg/changelog^7.</body>",
    "popup_action_title": "Press 'Esc' or 'Enter' to close",
    "popup_image": "h1_motd_620x580_mp_crash",
    "action": "popup"
})";
		featured->m_metadata = "{}";

		auto featured2 = new bdMarketingMessage;
		featured2->m_messageID = 3;
		featured2->m_languageCode = "en-US";
		featured2->m_content = R"({
    "action_title": "Custom Depot System",
    "image": "h1_motd_dlc_360x200_ch_mtx6_kits",
    "title": "<h2>Custom Depot System</h2>",
    "content_short": "",
    "h3_header": "<h3>^3No COD Points!^7</h3>",
    "content_long": "<body>We've added a new depot system to the game<br/><br/>Which you can now collect credits/parts from playing to be used with depots!</body>",
    "popup_action_title": "Press 'Esc' or 'Enter' to close",
	"popup_action": "depot",
    "popup_image": "h1_motd_dlc_620x580_ch_mtx6_kits",
    "action": "popup"
})";
		featured2->m_metadata = "{}";

		auto reply = server->create_reply(this->task_id());
		reply->add(motd);
		reply->add(featured);
		reply->add(featured2);
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
