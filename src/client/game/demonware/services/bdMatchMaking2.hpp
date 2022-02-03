#pragma once

namespace demonware
{
	class bdMatchMaking2 final : public service
	{
	public:
		bdMatchMaking2();

	private:
		void unk1(service_server* server, byte_buffer* buffer) const;
		void unk2(service_server* server, byte_buffer* buffer) const;
		void unk3(service_server* server, byte_buffer* buffer) const;
		void unk5(service_server* server, byte_buffer* buffer) const;
		void unk16(service_server* server, byte_buffer* buffer) const;
	};
}
