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
		game::dvar_t* cg_showversion;
		utils::hook::detour ui_get_formatted_build_number_hook;

		float color[4] = {0.666f, 0.666f, 0.666f, 0.666f};

		const char* ui_get_formatted_build_number_stub()
		{
			const auto* const build_num = ui_get_formatted_build_number_hook.invoke<const char*>();
			return utils::string::va("%s (%s)", VERSION, build_num);
		}

		void draw_branding()
		{
			const auto font = game::R_RegisterFont("fonts/fira_mono_bold.ttf", 20);
			if (font && cg_showversion->current.enabled)
			{
#ifdef DEBUG
				game::R_AddCmdDrawText("H1-Mod: " VERSION " (" __DATE__ " " __TIME__ ")", 0x7FFFFFFF, font, 10.f,
					5.f + static_cast<float>(font->pixelHeight), 1.f, 1.f, 0.0f, color, 0);
#else
				game::R_AddCmdDrawText("H1-Mod: " VERSION, 0x7FFFFFFF, font, 10.f,
					5.f + static_cast<float>(font->pixelHeight), 1.f, 1.f, 0.0f, color, 0);
#endif
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			scheduler::loop(draw_branding, scheduler::pipeline::renderer);
		}

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

			cg_showversion = dvars::register_bool("cg_showversion", true, game::DvarFlags::DVAR_FLAG_SAVED, "Show/Hide H1-Mod Branding");

			ui_get_formatted_build_number_hook.create(
				SELECT_VALUE(0x406EC0_b, 0x1DF300_b), ui_get_formatted_build_number_stub);
		}
	};
}

REGISTER_COMPONENT(branding::component)