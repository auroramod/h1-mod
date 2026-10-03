#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class chat final : public component_interface
{
public:
	void post_unpack() override;

private:
	static game::Font_s* ui_get_font_handle_stub();
};
