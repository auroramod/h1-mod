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
			utils::hook::inject(0x0F6F61_b, reinterpret_cast<void*>(0x2E6F588_b));
			utils::hook::inject(0x18A980_b, reinterpret_cast<void*>(0x2E6F588_b));
			utils::hook::call(0x33EDEC_b, ui_get_font_handle_stub);

			// move chat position on the screen above menu splashes
			dvars::override::register_vec2("cg_hudChatPosition", 5, 200, 0, 640, game::DVAR_FLAG_SAVED);
			dvars::override::register_int("cg_chatHeight", 5, 0, 8, game::DVAR_FLAG_SAVED);
		}
	};
}

REGISTER_COMPONENT(chat::component)
