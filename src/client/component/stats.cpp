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

		utils::hook::detour is_item_unlocked_hook;
		utils::hook::detour is_item_unlocked_hook2;
		utils::hook::detour is_item_unlocked_hook3;

		int is_item_unlocked_stub(void* a1, void* a2, void* a3)
		{
			if (cg_unlock_all_items->current.enabled)
			{
				return 0;
			}

			return is_item_unlocked_hook.invoke<int>(a1, a2, a3);
		}

		int is_item_unlocked_stub2(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6)
		{
			if (cg_unlock_all_items->current.enabled)
			{
				return 0;
			}

			return is_item_unlocked_hook2.invoke<int>(a1, a2, a3, a4, a5, a6);
		}

		int is_item_unlocked_stub3(void* a1)
		{
			if (cg_unlock_all_items->current.enabled)
			{
				return 0;
			}

			return is_item_unlocked_hook3.invoke<int>(a1);
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

			cg_unlock_all_items = dvars::register_bool("cg_unlockall_items", false, game::DVAR_FLAG_SAVED, true);
			dvars::register_bool("cg_unlockall_classes", false, game::DVAR_FLAG_SAVED, true);

			is_item_unlocked_hook.create(0x140413E60, is_item_unlocked_stub);
			is_item_unlocked_hook2.create(0x140413860, is_item_unlocked_stub2);
			is_item_unlocked_hook3.create(0x140412B70, is_item_unlocked_stub3);
		}
	};
}

REGISTER_COMPONENT(stats::component)
