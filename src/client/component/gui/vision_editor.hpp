#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

class vision_editor final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void dump_tweaks();
	static void render_window();
};
#endif
