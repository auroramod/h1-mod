#pragma once
#include "loader/component_loader.hpp"

class stats final : public component_interface
{
public:
	void post_unpack() override;

private:
	static int is_item_unlocked_stub(int a1, void* a2, void* a3, void* a4, int a5, void* a6);
	static int is_item_unlocked();
	static int is_item_unlocked_stub2(void* a1, void* a2);
};
