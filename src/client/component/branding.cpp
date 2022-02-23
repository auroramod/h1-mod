#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "localized_strings.hpp"
#include "scheduler.hpp"
#include "command.hpp"
#include "version.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

// fonts/default.otf, fonts/defaultBold.otf, fonts/fira_mono_regular.ttf, fonts/fira_mono_bold.ttf

namespace branding
{
	namespace
	{
		utils::hook::detour ui_get_formatted_build_number_hook;

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
				localized_strings::override("LUA_MENU_MULTIPLAYER_CAPS", "H1-Mod: MULTIPLAYER\n");
				localized_strings::override("MENU_MULTIPLAYER_CAPS", "H1-Mod: MULTIPLAYER");
			}

			dvars::override::Dvar_SetString("version", utils::string::va("H1-Mod %s", VERSION));

			ui_get_formatted_build_number_hook.create(
				SELECT_VALUE(0x1403B1C40, 0x1404E74C0), ui_get_formatted_build_number_stub);

			scheduler::loop([]()
			{

				const auto x = 4;
				const auto y = 4;
				const auto scale = 1.0f;
				float color[4] = {0.666f, 0.666f, 0.666f, 0.666f};

				const auto* text = "H1-Mod: " VERSION;

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
