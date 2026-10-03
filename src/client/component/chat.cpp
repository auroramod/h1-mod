#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "chat.hpp"

#include "localized_strings.hpp"
#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

void chat::post_unpack()
{
	if (!game::environment::is_mp())
	{
		return;
	}

	// use better font
	utils::hook::call(0x1400AA831, ui_get_font_handle_stub);
	utils::hook::call(0x14024800C, ui_get_font_handle_stub);
	utils::hook::call(0x14024F573, ui_get_font_handle_stub);

	// move chat position on the screen above menu splashes
	dvars_component::override::register_vec2("cg_hudChatPosition", 5, 200, 0, 640, game::DVAR_ARCHIVE);
	dvars_component::override::register_int("cg_chatHeight", 5, 0, 8, game::DVAR_ARCHIVE);
}

game::Font_s* chat::ui_get_font_handle_stub()
{
	return game::R_RegisterFont("fonts/defaultBold.otf", 24);
}

REGISTER_COMPONENT(chat)
