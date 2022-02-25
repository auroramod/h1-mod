#pragma once

namespace demonware
{
	class bdUNK95 final : public service
	{
	public:
		bdUNK95();

	private:
		void unk1(service_server* server, byte_buffer* buffer) const;
	};
}
