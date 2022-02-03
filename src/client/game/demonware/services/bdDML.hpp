#pragma once

namespace demonware
{
	class bdDML final : public service
	{
	public:
		bdDML();

	private:
		void get_user_raw_data(service_server* server, byte_buffer* buffer) const;
	};
}
