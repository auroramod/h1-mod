#pragma once

namespace demonware
{
	class bdProfiles final : public service
	{
	public:
		bdProfiles();

	private:
		void unk3(service_server* server, byte_buffer* buffer) const;
	};
}
