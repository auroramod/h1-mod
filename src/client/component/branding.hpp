#pragma once
#include "loader/component_loader.hpp"

class branding final : public component_interface
{
public:
	void post_unpack() override;

private:
	static const char* ui_get_formatted_build_number_stub();
	static void draw_branding();
};
