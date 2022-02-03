#pragma once

namespace demonware
{
	class bdCounters final : public service
	{
	public:
		bdCounters();

	private:
		void unk1(service_server* server, byte_buffer* buffer) const;
		void unk2(service_server* server, byte_buffer* buffer) const;
	};
}
