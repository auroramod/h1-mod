#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace map_patches
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			// skip fx name prefix checks
			utils::hook::set<uint8_t>(0x2F377D_b, 0xEB); // createfx parse
			utils::hook::set<uint8_t>(0x4444E0_b, 0xEB); // scr_loadfx
		}
	};
}

REGISTER_COMPONENT(map_patches::component)