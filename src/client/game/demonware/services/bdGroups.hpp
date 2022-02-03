#pragma once

namespace demonware
{
	class bdGroups final : public service
	{
	public:
		bdGroups();

	private:
		void set_groups(service_server* server, byte_buffer* buffer) const;
		void unk4(service_server* server, byte_buffer* buffer) const;
	};
}
