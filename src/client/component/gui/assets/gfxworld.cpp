#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/fastfiles.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

namespace gui::asset_list::gfxworld
{
	namespace
	{
		bool draw_window(game::GfxWorld* asset)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);

			ImGui::InputFloat3("sunFxPosition", asset->sun.sunFxPosition);

			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::GfxWorld>(game::ASSET_TYPE_GFXWORLD, draw_window);
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::gfxworld::component)
#endif
