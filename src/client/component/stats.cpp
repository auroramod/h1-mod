#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"
#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/nt.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace stats
{
	namespace
	{
		game::dvar_t* cg_unlock_all_items;
		game::dvar_t* cg_unlock_all_loot;

		utils::hook::detour is_item_unlocked_hook;
		utils::hook::detour is_item_unlocked_hook2;

		int is_item_unlocked_stub(int a1, void* a2, void* a3, void* a4, int a5, void* a6)
		{
			if (cg_unlock_all_items->current.enabled)
			{
				return 0;
			}

			return is_item_unlocked_hook.invoke<int>(a1, a2, a3, a4, a5, a6);
		}

		int is_item_unlocked()
		{
			return 0;
		}

		int is_item_unlocked_stub2(void* a1, void* a2)
		{
			const auto state = is_item_unlocked_hook2.invoke<int>(a1, a2);
			if (state == 15 /*Not In Inventory*/ && cg_unlock_all_loot->current.enabled)
			{
				return 0;
			}
			return state;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			utils::hook::jump(0x19E6E0_b, is_item_unlocked, true);

			if (game::environment::is_dedi())
			{
				utils::hook::jump(0x19E070_b, is_item_unlocked, true);
				utils::hook::jump(0x19D390_b, is_item_unlocked, true);
				utils::hook::jump(0x19D140_b, is_item_unlocked, true);
			}
			else
			{
				is_item_unlocked_hook.create(0x19E070_b, is_item_unlocked_stub);
				is_item_unlocked_hook2.create(0x19D140_b, is_item_unlocked_stub2);

				cg_unlock_all_items = dvars::register_bool("cg_unlockall_items", false, game::DVAR_FLAG_SAVED,
					"Whether items should be locked based on the player's stats or always unlocked.");
				dvars::register_bool("cg_unlockall_classes", false, game::DVAR_FLAG_SAVED,
					"Whether classes should be locked based on the player's stats or always unlocked.");
				cg_unlock_all_loot = dvars::register_bool("cg_unlockall_loot", false, game::DVAR_FLAG_SAVED,
					"Whether loot should be locked based on the player's stats or always unlocked.");
			}
		}
	};
}

REGISTER_COMPONENT(stats::component)
