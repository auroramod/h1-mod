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
			if (!font)
			{
				return;
			}

#ifdef DEBUG
			const auto text = "h1-mod: " VERSION " (" __DATE__ " " __TIME__ ")";
#else
			const auto text = "h1-mod: " VERSION;
#endif

			const auto placement = game::ScrPlace_GetViewPlacement();
			float text_color[4] = {0.6f, 0.6f, 0.6f, 0.6f};

			game::rectDef_s rect{};
			rect.x = 0;
			rect.y = 0;
			rect.w = 500;
			rect.horzAlign = 0;
			rect.vertAlign = 0;

			game::rectDef_s text_rect{};

			game::UI_DrawWrappedText(placement, text, &rect, font, -102.5f, 10.f, 0.17f, text_color, 0, 0, &text_rect, 0);
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