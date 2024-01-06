#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

#define DVAR_AS_OMNVAR_OVERRIDE(omnvar, default_val, max_int) dvars::register_int(omnvar, default_val, 0, max_int, game::DVAR_FLAG_REPLICATED, utils::string::va("Omnvar replacement for %s", omnvar));
#define DVAR_AS_OMNVAR_OVERRIDE_FLOAT(omnvar, default_val, max_float) dvars::register_float(omnvar, default_val, 0.0, max_float, game::DVAR_FLAG_REPLICATED, utils::string::va("Omnvar replacement for %s", omnvar));

namespace experimental
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// fix static model's lighting going black sometimes
			//dvars::override::register_int("r_smodelInstancedThreshold", 0, 0, 128, 0x0);

			// change minimum cap to -2000 instead of -1000 (culling issue)
			dvars::override::register_float("r_lodBiasRigid", 0, -2000, 0, game::DVAR_FLAG_SAVED);
		
			// use replicated dvars instead of omnvars for Momentum custom gamemode
			if (game::environment::is_mp())
			{
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_flag_count", 5, 5);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_ally_flag_count", 0, 5);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_touching_allies", 0, 18);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_touching_axis", 0, 18);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_capture_team", 0, 3);
				DVAR_AS_OMNVAR_OVERRIDE_FLOAT("ui_twar_capture_progress", 0.0, 100.0);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_momentum_bar_visible", 1, 1);
				DVAR_AS_OMNVAR_OVERRIDE_FLOAT("ui_twar_momentum_maxed_time", 20.0, 20.0);

				DVAR_AS_OMNVAR_OVERRIDE_FLOAT("ui_twar_momentum_allies", 0.0, 1.0);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_momentum_scale_allies", 3, 3);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_momentum_end_time_allies", 0, 100'000);

				DVAR_AS_OMNVAR_OVERRIDE_FLOAT("ui_twar_momentum_axis", 0.0, 1.0);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_momentum_scale_axis", 3, 3);
				DVAR_AS_OMNVAR_OVERRIDE("ui_twar_momentum_end_time_axis", 0, 100'000);
			}
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(experimental::component)
#endif