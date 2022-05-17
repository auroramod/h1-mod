#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "localized_strings.hpp"
#include "scheduler.hpp"
#include "command.hpp"
#include "version.hpp"

#include "game/game.hpp"
#include "dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

// fonts/default.otf, fonts/defaultBold.otf, fonts/fira_mono_regular.ttf, fonts/fira_mono_bold.ttf

namespace branding
{
	namespace
	{
		utils::hook::detour ui_get_formatted_build_number_hook;

		float color[4] = {0.666f, 0.666f, 0.666f, 0.666f};

		const char* ui_get_formatted_build_number_stub()
		{
			const auto* const build_num = ui_get_formatted_build_number_hook.invoke<const char*>();
			return utils::string::va("%s (%s)", VERSION, build_num);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			if (game::environment::is_mp())
			{
				localized_strings::override("LUA_MENU_MULTIPLAYER_CAPS", "H1-MOD: MULTIPLAYER\n");
				localized_strings::override("MENU_MULTIPLAYER_CAPS", "H1-MOD: MULTIPLAYER");
			}

			//dvars::override::set_string("version", utils::string::va("H1-Mod %s", VERSION));

			ui_get_formatted_build_number_hook.create(
				SELECT_VALUE(0x0, 0x1DF300_b), ui_get_formatted_build_number_stub);

			scheduler::loop([]()
			{
				const auto font = game::R_RegisterFont("fonts/fira_mono_bold.ttf", 20);
				if (font)
				{
					game::R_AddCmdDrawText("H1-Mod: " VERSION, 0x7FFFFFFF, font, 10.f,
					                       5.f + static_cast<float>(font->pixelHeight), 1.f, 1.f, 0.0f, color, 0);
				}
			}, scheduler::pipeline::renderer);
		}
	};
}

REGISTER_COMPONENT(branding::component)