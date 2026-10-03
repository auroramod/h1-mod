#pragma once
#include "loader/component_loader.hpp"

class thirdperson final : public component_interface
{
public:
	void post_unpack() override;

private:
	static __int64 update_thirdperson_stub();
	static void offset_thirdperson_view_internal_stub(int local_client_num, float angle, float range, int a4, int a5, int a6, int a7);
};
