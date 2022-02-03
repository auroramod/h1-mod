#include <std_include.hpp>
#include "../services.hpp"

namespace demonware
{
	bdDML::bdDML() : service(27, "bdDML")
	{
		this->register_task(2, &bdDML::get_user_raw_data);
	}

	void bdDML::get_user_raw_data(service_server* server, byte_buffer* /*buffer*/) const
	{
		auto result = new bdDMLRawData;
		result->country_code = "US";
		result->country_code = "'Murica";
		result->region = "New York";
		result->city = "New York";
		result->latitude = 0;
		result->longitude = 0;

		result->asn = 0x2119;
		result->timezone = "+01:00";

		auto reply = server->create_reply(this->task_id());
		reply->add(result);
		reply->send();
	}
}
