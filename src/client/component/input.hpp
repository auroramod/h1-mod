#pragma once
#include "loader/component_loader.hpp"

class input final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void cl_char_event_stub(const int local_client_num, const int key);
	static void cl_key_event_stub(const int local_client_num, const int key, const int down, const unsigned int time);
	static void cl_mouse_move_stub(const int local_client_num, int x, int y);
};
