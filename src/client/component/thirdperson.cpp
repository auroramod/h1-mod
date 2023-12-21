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

		namespace mp
		{
			__int64 sub_1D5950_stub([[maybe_unused]] int local_client_num, game::mp::cg_s* a2)
			{
				auto next_snap = a2->nextSnap;
				if (next_snap->ps.pm_type < 7u)
				{
					int link_flags = next_snap->ps.linkFlags;
					if ((link_flags & 2) == 0 && (next_snap->ps.otherFlags & 4) == 0)
					{
						auto client_globals = a2;
						if (!client_globals->unk_979676 || !client_globals->unk_979696)
						{
							if (cg_thirdPerson && cg_thirdPerson->current.enabled)
							{
								return 1;
							}

							if (!(link_flags & (1 << 0xE)) || client_globals->unk_979696)
								return (link_flags >> 27) & 1;
							if (link_flags & (1 << 0x1D))
								return 0;
							if (!(link_flags & (1 << 0x1C)))
								return a2->unk_601088;
						}
					}
				}
				return 1;
			}

			void sub_10C280_stub(int local_client_num, float angle, float range, int a4, int a5, int a6, int a7)
			{
				angle = cg_thirdPersonAngle->current.value;
				range = cg_thirdPersonRange->current.value;
				utils::hook::invoke<void>(0x10C280_b, local_client_num, angle, range, a4, a5, a6, a7);
			}
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

			utils::hook::jump(0x1D5950_b, mp::sub_1D5950_stub);
			utils::hook::call(0x10C26B_b, mp::sub_10C280_stub);
		}
	};
}

REGISTER_COMPONENT(thirdperson::component)
