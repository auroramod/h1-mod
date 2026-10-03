#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

class conststrings final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void render_window();
	static void ncs_window();
};
#endif
