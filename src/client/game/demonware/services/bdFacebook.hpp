#pragma once

namespace demonware
{
	class bdFacebook final : public service
	{
	public:
		bdFacebook();

	private:
		void unk1(service_server* server, byte_buffer* buffer) const;
		void unk3(service_server* server, byte_buffer* buffer) const;
		void unk7(service_server* server, byte_buffer* buffer) const;
		void unk8(service_server* server, byte_buffer* buffer) const;
	};
}
