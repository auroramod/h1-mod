#pragma once
#include "loader/component_loader.hpp"

class thread_names final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void set_thread_names();
};
