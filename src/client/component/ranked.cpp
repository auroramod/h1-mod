#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"
#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace ranked
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			if (game::environment::is_mp())
			{
				dvars::override::register_bool("xblive_privatematch", false, game::DVAR_FLAG_REPLICATED);
			}

			if (game::environment::is_dedi() && !utils::flags::has_flag("unranked"))
			{
				dvars::override::register_bool("xblive_privatematch", false, game::DVAR_FLAG_REPLICATED | game::DVAR_FLAG_WRITE);

				// Skip some check in _menus.gsc
				dvars::register_bool("force_ranking", true, game::DVAR_FLAG_WRITE, "");
			}

			// Always run bots, even if xblive_privatematch is 0
			utils::hook::set(0x1401D9300, 0xC301B0); // BG_BotSystemEnabled
			utils::hook::set(0x1401D90D0, 0xC301B0); // BG_AISystemEnabled
			utils::hook::set(0x1401D92A0, 0xC301B0); // BG_BotFastFileEnabled
			utils::hook::set(0x1401D9400, 0xC301B0); // BG_BotsUsingTeamDifficulty
		}
	};
}

REGISTER_COMPONENT(ranked::component)
