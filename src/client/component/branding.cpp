#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "localized_strings.hpp"
#include "scheduler.hpp"
#include "command.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace branding
{
	game::dvar_t* dvar_draw2d;

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{

			if (game::environment::is_dedi())
			{
				return;
			}

			if (game::environment::is_mp)
			{
				localized_strings::override("LUA_MENU_MULTIPLAYER_CAPS", "H1-Mod: MULTIPLAYER\n");
				localized_strings::override("MENU_MULTIPLAYER_CAPS", "H1-Mod: MULTIPLAYER");
				localized_strings::override("PLATFORM_UI_HEADER_PLAY_MP_CAPS", "H1-ONLINE");
			}

			scheduler::loop([]()
				{
					if (!dvar_draw2d)
					{
						dvar_draw2d = game::Dvar_FindVar("cg_draw2d");
					}

					if (dvar_draw2d && !dvar_draw2d->current.enabled)
					{
						return;
					}

					const auto x = 4;
					const auto y = 4;
					const auto scale = 1.0f;
					float color[4] = { 1.0f, 0.5f, 0.0f, 1.0f };

					const auto* text = "H1-Mod 1.4";

					auto* font = game::R_RegisterFont("fonts/fira_mono_bold.ttf", 20);

					if (!font) return;

					game::R_AddCmdDrawText(text, 0x7FFFFFFF, font, static_cast<float>(x),
						y + static_cast<float>(font->pixelHeight) * scale,
						scale, scale, 0.0f, color, 0);

				}, scheduler::pipeline::renderer);
		}
	};
}

REGISTER_COMPONENT(branding::component)


// fonts/default.otf, fonts/defaultBold.otf, fonts/fira_mono_regular.ttf, fonts/fira_mono_bold.ttf
