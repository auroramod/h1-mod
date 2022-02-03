#pragma once

namespace demonware
{
	class bdMarketing final : public service
	{
	public:
		bdMarketing();

	private:
		void unk2(service_server* server, byte_buffer* buffer) const;
		void unk3(service_server* server, byte_buffer* buffer) const;
	};
}
