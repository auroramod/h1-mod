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

namespace gui::asset_list::weapon
{
	namespace
	{

#define DRAW_ASSET_PROPERTY(__name__, __fmt__) \
		ImGui::Text(#__name__ ": " __fmt__, asset->__name__); \

#define DRAW_ASSET_PROPERTY_COPY(__name__) \
		ImGui::Text(#__name__ ": "); \
		ImGui::SameLine(); \
		if (ImGui::Button(asset->__name__)) \
		{ \
			gui::copy_to_clipboard(asset->__name__); \
		} \


		bool draw_weapon_window(game::WeaponDef* asset)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
			DRAW_ASSET_PROPERTY_COPY(szInternalName);
			DRAW_ASSET_PROPERTY_COPY(szDisplayName);
			DRAW_ASSET_PROPERTY_COPY(killIcon->name);

			ImGui::Text("location multipliers");
			for (auto i = 0; i < 22; i++)
			{
				DRAW_ASSET_PROPERTY(locationDamageMultipliers[i], "%f");
			}

			DRAW_ASSET_PROPERTY(minDamage, "%i");
			DRAW_ASSET_PROPERTY(midDamage, "%i");
			DRAW_ASSET_PROPERTY(damage, "%i");
			DRAW_ASSET_PROPERTY(minPlayerDamage, "%i");
			DRAW_ASSET_PROPERTY(midPlayerDamage, "%i");
			DRAW_ASSET_PROPERTY(minDamageRange, "%f");
			DRAW_ASSET_PROPERTY(maxDamageRange, "%f");
			DRAW_ASSET_PROPERTY(playerDamage, "%i");
			DRAW_ASSET_PROPERTY(adsZoomFov, "%f");
			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::WeaponDef>(game::ASSET_TYPE_WEAPON, draw_weapon_window);
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::weapon::component)
#endif
