#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "dvars.hpp"
#include "localized_strings.hpp"
#include "scheduler.hpp"
#include "version.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

// fonts/default.otf, fonts/defaultBold.otf, fonts/fira_mono_regular.ttf, fonts/fira_mono_bold.ttf

namespace branding
{
	namespace
	{
		game::dvar_t* branding = nullptr;

		utils::hook::detour ui_get_formatted_build_number_hook;
		const char* ui_get_formatted_build_number_stub()
		{
			const auto build_num = ui_get_formatted_build_number_hook.invoke<const char*>();
			return utils::string::va("%s (%s)", VERSION, build_num);
		}

		void draw_branding()
		{
			if (branding == nullptr || !branding->current.enabled)
			{
				return;
			}

			const auto font = game::R_RegisterFont("fonts/fira_mono_bold.ttf", 15);
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
			static float text_color[4] = { 0.860f, 0.459f, 0.925f, 0.400f };

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
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			scheduler::once([]()
			{
				branding = dvars::register_bool("branding", true, game::DVAR_FLAG_SAVED, "Show brainding in the top left corner");
			}, scheduler::renderer);
			scheduler::loop(draw_branding, scheduler::renderer);

			ui_get_formatted_build_number_hook.create(
				SELECT_VALUE(0x406EC0_b, 0x1DF300_b), ui_get_formatted_build_number_stub);
		}
	};
}

REGISTER_COMPONENT(branding::component)