#pragma once

namespace demonware
{
	class bdUNK80 final : public service
	{
	public:
		bdUNK80();

	private:
		void unk42(service_server* server, byte_buffer* buffer) const;
		void unk49(service_server* server, byte_buffer* buffer) const;
		void unk60(service_server* server, byte_buffer* buffer) const;
		void unk130(service_server* server, byte_buffer* buffer) const;
		void unk165(service_server* server, byte_buffer* buffer) const;
		void unk193(service_server* server, byte_buffer* buffer) const;
	};
}
