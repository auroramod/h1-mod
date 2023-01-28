#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace experimental
{
	namespace
	{
		const game::dvar_t* map_experiments = nullptr;

		utils::hook::detour load_gfx_world_asset_hook;

		void load_gfx_world_asset_stub(game::GfxWorld** gfx_world_ptr)
		{
			load_gfx_world_asset_hook.invoke<void>(gfx_world_ptr);

			if (!map_experiments || !map_experiments->current.enabled)
			{
				return;
			}

			auto gfx_world = *gfx_world_ptr;

			for (unsigned int i = 0; i < gfx_world->dpvs.smodelCount; ++i)
			{
				if ((gfx_world->dpvs.smodelDrawInsts[i].flags & 64) != 0) // STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING
				{
					gfx_world->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[0] = 14340; // r: 0.4
					gfx_world->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[1] = 14340; // g: 0.4
					gfx_world->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[2] = 14340; // b: 0.4
					gfx_world->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[3] = 14340; // a: 0.4
					gfx_world->dpvs.smodelLighting[i].modelLightGridLightingInfo.a = 47280;
					gfx_world->dpvs.smodelLighting[i].modelLightGridLightingInfo.b = 1.0f;
				}
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// modify GfxWorld to use static model lighting
			load_gfx_world_asset_hook.create(0x39C460_b, load_gfx_world_asset_stub);

			scheduler::once([]
			{
				map_experiments = dvars::register_bool("map_experiments", 0, 0x0, "Toggle experimental map settings");
			}, scheduler::main);
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(experimental::component)
#endif