#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "dvars.hpp"
#include "localized_strings.hpp"
#include "scheduler.hpp"
#include "version.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

// fonts/default.otf, fonts/defaultBold.otf, fonts/fira_mono_regular.ttf, fonts/fira_mono_bold.ttf

namespace branding
{
	namespace
	{
		utils::hook::detour ui_get_formatted_build_number_hook;

		float color[4] = {0.39f, 0.9f, 0.4f, 0.9f};

		const char* ui_get_formatted_build_number_stub()
		{
			const auto* const build_num = ui_get_formatted_build_number_hook.invoke<const char*>();
			return utils::string::va("%s (%s)", VERSION, build_num);
		}

		void draw_branding()
		{
			const auto font = game::R_RegisterFont("fonts/fira_mono_bold.ttf", 22);
			if (font)
			{
#ifdef DEBUG
				game::R_AddCmdDrawText("h1-mod: " VERSION " (" __DATE__ " " __TIME__ ")", 
					0x7FFFFFFF, font, 10.f,
					5.f + static_cast<float>(font->pixelHeight), 
					1.f, 1.f, 0.0f, color, 0);
#else
				game::R_AddCmdDrawText("h1-mod",
					0x7FFFFFFF, font, 10.f,
					5.f + static_cast<float>(font->pixelHeight), 
					1.f, 1.f, 0.0f, color, 0);
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

			ui_get_formatted_build_number_hook.create(
				SELECT_VALUE(0x406EC0_b, 0x1DF300_b), ui_get_formatted_build_number_stub);
		}
	};
}

REGISTER_COMPONENT(branding::component)