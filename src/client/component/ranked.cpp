#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

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
				dvars::override::register_bool("xblive_privatematch", false, game::DVAR_CODINFO);
			}

			if (game::environment::is_dedi())
			{
				dvars::override::register_bool("xblive_privatematch", false, game::DVAR_CODINFO | game::DVAR_ROM);
				dvars::register_bool("force_ranking", true, game::DVAR_ROM, ""); // Skip some check in _menus.gsc
			}

			// Always run bots, even if xblive_privatematch is 0
			utils::hook::set(0x1401D9300, 0xC301B0); // BG_BotSystemEnabled
			utils::hook::set(0x1401D90D0, 0xC301B0); // BG_AISystemEnabled
			utils::hook::set(0x1401D92A0, 0xC301B0); // BG_BotFastFileEnabled
			utils::hook::set(0x1401D9400, 0xC301B0); // BG_BotsUsingTeamDifficulty

			// Fix sessionteam always returning none (SV_ClientMP_HasAssignedTeam_Internal)
			utils::hook::set(0x140489280, 0xC300B0);
		}
	};
}

REGISTER_COMPONENT(ranked::component)
