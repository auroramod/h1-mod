#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

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
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(experimental::component)
#endif