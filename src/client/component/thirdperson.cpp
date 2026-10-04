#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "scheduler.hpp"

#include <utils/hook.hpp>

namespace thirdperson
{
	namespace
	{
		game::dvar_t* cg_thirdPerson = nullptr;
		game::dvar_t* cg_thirdPersonRange = nullptr;
		game::dvar_t* cg_thirdPersonAngle = nullptr;

		utils::hook::detour update_thirdperson_hook;

		__int64 update_thirdperson_stub()
		{
			if (cg_thirdPerson && cg_thirdPerson->current.enabled)
			{
				if (const auto next_snap = *game::next_snap; next_snap->ps.pm_type < 7u)
				{
					// same checks as the game: ps+0x5C & 2, ps+0x30 & 4
					if ((next_snap->ps.otherFlags & 2) == 0 && (next_snap->ps.linkFlags & 4) == 0)
					{
						if (!*game::is_rendering_thirdperson || !*game::is_rendering_thirdperson1)
						{
							*reinterpret_cast<int*>(0x1429C87D4) = 1;
							return 1;
						}
					}
				}
			}

			return update_thirdperson_hook.invoke<__int64>();
		}

		void offset_thirdperson_view_internal_stub(int local_client_num, float angle, float range, int a4, int a5, int a6, int a7)
		{
			angle = cg_thirdPersonAngle->current.value;
			range = cg_thirdPersonRange->current.value;
			utils::hook::invoke<void>(0x1400BC290, local_client_num, angle, range, a4, a5, a6, a7);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			scheduler::once([]()
			{
				cg_thirdPerson = dvars::register_bool("cg_thirdPerson", 0, 4, "Use third person view");
				cg_thirdPersonAngle = dvars::register_float("cg_thirdPersonAngle", 356.0f, -180.0f, 360.0f, 4,
					"The angle of the camera from the player in third person view");
				cg_thirdPersonRange = dvars::register_float("cg_thirdPersonRange", 120.0f, 0.0f, 1024, 4,
					"The range of the camera from the player in third person view");
			}, scheduler::main);

			update_thirdperson_hook.create(0x1400F8AD0, &update_thirdperson_stub);
			utils::hook::call(0x1400BC27D, &offset_thirdperson_view_internal_stub);
		}
	};
}

REGISTER_COMPONENT(thirdperson::component)
