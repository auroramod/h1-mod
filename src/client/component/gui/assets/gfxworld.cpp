#include <std_include.hpp>

#ifdef _DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/fastfiles.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"
#include "gfxworld.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

void asset_gfxworld::post_unpack()
{
	asset_list::add_asset_view<game::GfxWorld>(game::ASSET_TYPE_GFXWORLD, draw_window);
}

bool asset_gfxworld::draw_window(game::GfxWorld* asset)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);

	ImGui::InputFloat3("sunFxPosition", asset->sun.sunFxPosition);

	return true;
}

REGISTER_COMPONENT(asset_gfxworld)
#endif
