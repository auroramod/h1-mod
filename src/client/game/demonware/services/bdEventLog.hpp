#pragma once

namespace demonware
{
	class bdEventLog final : public service
	{
	public:
		bdEventLog();

	private:
		void unk6(service_server* server, byte_buffer* buffer) const;
	};
}
