#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace slowmotion
{
	namespace
	{
		void scr_cmd_set_slow_motion()
		{
			if (game::Scr_GetNumParam() < 1)
			{
				return;
			}

			int duration = 1000;
			float end = 1.0f;
			const float start = game::Scr_GetFloat(0);

			if (game::Scr_GetNumParam() >= 2)
			{
				end = game::Scr_GetFloat(1u);
			}

			if (game::Scr_GetNumParam() >= 3)
			{
				duration = static_cast<int>(game::Scr_GetFloat(2u) * 1000.0f);
			}

			game::SV_SetConfigstring(10, utils::string::va("%i %i %g %g", *game::mp::gameTime, duration, start, end));
			game::Com_SetSlowMotion(start, end, duration);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			utils::hook::jump(0x140365480, scr_cmd_set_slow_motion);
		}
	};
}

REGISTER_COMPONENT(slowmotion::component)
