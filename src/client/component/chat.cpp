#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "localized_strings.hpp"
#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace chat
{
	namespace
	{
		game::Font_s* ui_get_font_handle_stub()
		{
			return game::R_RegisterFont("fonts/defaultBold.otf", 24);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
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
			dvars::override::register_vec2("cg_hudChatPosition", 5, 200, 0, 640, game::DVAR_ARCHIVE);
			dvars::override::register_int("cg_chatHeight", 5, 0, 8, game::DVAR_ARCHIVE);
		}
	};
}

REGISTER_COMPONENT(chat::component)
