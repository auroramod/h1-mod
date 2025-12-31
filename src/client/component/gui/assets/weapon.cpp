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

#define ADD_STATETIMER(__struct__, __name__) \
			ImGui::InputInt(#__name__, &asset->__struct__.__name__, 5, 10); \

			if (ImGui::TreeNode("stateTimers"))
			{
				ADD_STATETIMER(stateTimers, aiFuseTime);
				ADD_STATETIMER(stateTimers, altDropTime);
				ADD_STATETIMER(stateTimers, altRaiseTime);
				ADD_STATETIMER(stateTimers, blastBackTime);
				ADD_STATETIMER(stateTimers, blastLeftTime);
				ADD_STATETIMER(stateTimers, blastRightTime);
				ADD_STATETIMER(stateTimers, breachRaiseTime);
				ADD_STATETIMER(stateTimers, detonateDelay);
				ADD_STATETIMER(stateTimers, detonateTime);
				ADD_STATETIMER(stateTimers, dodgeTime);
				ADD_STATETIMER(stateTimers, dropTime);
				ADD_STATETIMER(stateTimers, emptyDropTime);
				ADD_STATETIMER(stateTimers, emptyRaiseTime);
				ADD_STATETIMER(stateTimers, fireDelay);
				ADD_STATETIMER(stateTimers, fireTime);
				ADD_STATETIMER(stateTimers, firstRaiseTime);
				ADD_STATETIMER(stateTimers, fuseTime);
				ADD_STATETIMER(stateTimers, grenadePrimeReadyToThrowTime);
				ADD_STATETIMER(stateTimers, heatCooldownInTime);
				ADD_STATETIMER(stateTimers, heatCooldownOutReadyTime);
				ADD_STATETIMER(stateTimers, heatCooldownOutTime);
				ADD_STATETIMER(stateTimers, highJumpDropInTime);
				ADD_STATETIMER(stateTimers, highJumpDropLandTime);
				ADD_STATETIMER(stateTimers, highJumpDropLoopTime);
				ADD_STATETIMER(stateTimers, highJumpInTime);
				ADD_STATETIMER(stateTimers, holdFireTime);
				ADD_STATETIMER(stateTimers, hybridSightInTime);
				ADD_STATETIMER(stateTimers, hybridSightOutTime);
				ADD_STATETIMER(stateTimers, landDipTime);
				ADD_STATETIMER(stateTimers, meleeChargeDelay);
				ADD_STATETIMER(stateTimers, meleeChargeTime);
				ADD_STATETIMER(stateTimers, meleeDelay);
				ADD_STATETIMER(stateTimers, meleeTime);
				ADD_STATETIMER(stateTimers, missileTime);
				ADD_STATETIMER(stateTimers, nightVisionRemoveTime);
				ADD_STATETIMER(stateTimers, nightVisionRemoveTimeFadeInStart);
				ADD_STATETIMER(stateTimers, nightVisionRemoveTimePowerDown);
				ADD_STATETIMER(stateTimers, nightVisionWearTime);
				ADD_STATETIMER(stateTimers, nightVisionWearTimeFadeOutEnd);
				ADD_STATETIMER(stateTimers, nightVisionWearTimePowerUp);
				ADD_STATETIMER(stateTimers, offhandSwitchTime);
				ADD_STATETIMER(stateTimers, overheatOutReadyTime);
				ADD_STATETIMER(stateTimers, overheatOutTime);
				ADD_STATETIMER(stateTimers, primeTime);
				ADD_STATETIMER(stateTimers, quickDropTime);
				ADD_STATETIMER(stateTimers, quickRaiseTime);
				ADD_STATETIMER(stateTimers, raiseTime);
				ADD_STATETIMER(stateTimers, rechamberBoltTime);
				ADD_STATETIMER(stateTimers, rechamberTime);
				ADD_STATETIMER(stateTimers, rechamberTimeOneHanded);
				ADD_STATETIMER(stateTimers, reloadAddTime);
				ADD_STATETIMER(stateTimers, reloadAddTimeDualWield);
				ADD_STATETIMER(stateTimers, reloadEmptyAddTime);
				ADD_STATETIMER(stateTimers, reloadEmptyAddTimeDualMag);
				ADD_STATETIMER(stateTimers, reloadEmptyDualMag);
				ADD_STATETIMER(stateTimers, reloadEmptyTime);
				ADD_STATETIMER(stateTimers, reloadEndTime);
				ADD_STATETIMER(stateTimers, reloadShowRocketTime);
				ADD_STATETIMER(stateTimers, reloadStartAddTime);
				ADD_STATETIMER(stateTimers, reloadStartTime);
				ADD_STATETIMER(stateTimers, reloadTime);
				ADD_STATETIMER(stateTimers, reloadTimeDualWield);
				ADD_STATETIMER(stateTimers, slideInTime);
				ADD_STATETIMER(stateTimers, slideLoopTime);
				ADD_STATETIMER(stateTimers, slideOutTime);
				ADD_STATETIMER(stateTimers, speedReloadAddTime);
				ADD_STATETIMER(stateTimers, speedReloadTime);
				ADD_STATETIMER(stateTimers, sprintInTime);
				ADD_STATETIMER(stateTimers, sprintLoopTime);
				ADD_STATETIMER(stateTimers, sprintOutTime);
				ADD_STATETIMER(stateTimers, stunnedTimeBegin);
				ADD_STATETIMER(stateTimers, stunnedTimeEnd);
				ADD_STATETIMER(stateTimers, stunnedTimeLoop);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("akimboStateTimers"))
			{
				ADD_STATETIMER(akimboStateTimers, aiFuseTime);
				ADD_STATETIMER(akimboStateTimers, altDropTime);
				ADD_STATETIMER(akimboStateTimers, altRaiseTime);
				ADD_STATETIMER(akimboStateTimers, blastBackTime);
				ADD_STATETIMER(akimboStateTimers, blastLeftTime);
				ADD_STATETIMER(akimboStateTimers, blastRightTime);
				ADD_STATETIMER(akimboStateTimers, breachRaiseTime);
				ADD_STATETIMER(akimboStateTimers, detonateDelay);
				ADD_STATETIMER(akimboStateTimers, detonateTime);
				ADD_STATETIMER(akimboStateTimers, dodgeTime);
				ADD_STATETIMER(akimboStateTimers, dropTime);
				ADD_STATETIMER(akimboStateTimers, emptyDropTime);
				ADD_STATETIMER(akimboStateTimers, emptyRaiseTime);
				ADD_STATETIMER(akimboStateTimers, fireDelay);
				ADD_STATETIMER(akimboStateTimers, fireTime);
				ADD_STATETIMER(akimboStateTimers, firstRaiseTime);
				ADD_STATETIMER(akimboStateTimers, fuseTime);
				ADD_STATETIMER(akimboStateTimers, grenadePrimeReadyToThrowTime);
				ADD_STATETIMER(akimboStateTimers, heatCooldownInTime);
				ADD_STATETIMER(akimboStateTimers, heatCooldownOutReadyTime);
				ADD_STATETIMER(akimboStateTimers, heatCooldownOutTime);
				ADD_STATETIMER(akimboStateTimers, highJumpDropInTime);
				ADD_STATETIMER(akimboStateTimers, highJumpDropLandTime);
				ADD_STATETIMER(akimboStateTimers, highJumpDropLoopTime);
				ADD_STATETIMER(akimboStateTimers, highJumpInTime);
				ADD_STATETIMER(akimboStateTimers, holdFireTime);
				ADD_STATETIMER(akimboStateTimers, hybridSightInTime);
				ADD_STATETIMER(akimboStateTimers, hybridSightOutTime);
				ADD_STATETIMER(akimboStateTimers, landDipTime);
				ADD_STATETIMER(akimboStateTimers, meleeChargeDelay);
				ADD_STATETIMER(akimboStateTimers, meleeChargeTime);
				ADD_STATETIMER(akimboStateTimers, meleeDelay);
				ADD_STATETIMER(akimboStateTimers, meleeTime);
				ADD_STATETIMER(akimboStateTimers, missileTime);
				ADD_STATETIMER(akimboStateTimers, nightVisionRemoveTime);
				ADD_STATETIMER(akimboStateTimers, nightVisionRemoveTimeFadeInStart);
				ADD_STATETIMER(akimboStateTimers, nightVisionRemoveTimePowerDown);
				ADD_STATETIMER(akimboStateTimers, nightVisionWearTime);
				ADD_STATETIMER(akimboStateTimers, nightVisionWearTimeFadeOutEnd);
				ADD_STATETIMER(akimboStateTimers, nightVisionWearTimePowerUp);
				ADD_STATETIMER(akimboStateTimers, offhandSwitchTime);
				ADD_STATETIMER(akimboStateTimers, overheatOutReadyTime);
				ADD_STATETIMER(akimboStateTimers, overheatOutTime);
				ADD_STATETIMER(akimboStateTimers, primeTime);
				ADD_STATETIMER(akimboStateTimers, quickDropTime);
				ADD_STATETIMER(akimboStateTimers, quickRaiseTime);
				ADD_STATETIMER(akimboStateTimers, raiseTime);
				ADD_STATETIMER(akimboStateTimers, rechamberBoltTime);
				ADD_STATETIMER(akimboStateTimers, rechamberTime);
				ADD_STATETIMER(akimboStateTimers, rechamberTimeOneHanded);
				ADD_STATETIMER(akimboStateTimers, reloadAddTime);
				ADD_STATETIMER(akimboStateTimers, reloadAddTimeDualWield);
				ADD_STATETIMER(akimboStateTimers, reloadEmptyAddTime);
				ADD_STATETIMER(akimboStateTimers, reloadEmptyAddTimeDualMag);
				ADD_STATETIMER(akimboStateTimers, reloadEmptyDualMag);
				ADD_STATETIMER(akimboStateTimers, reloadEmptyTime);
				ADD_STATETIMER(akimboStateTimers, reloadEndTime);
				ADD_STATETIMER(akimboStateTimers, reloadShowRocketTime);
				ADD_STATETIMER(akimboStateTimers, reloadStartAddTime);
				ADD_STATETIMER(akimboStateTimers, reloadStartTime);
				ADD_STATETIMER(akimboStateTimers, reloadTime);
				ADD_STATETIMER(akimboStateTimers, reloadTimeDualWield);
				ADD_STATETIMER(akimboStateTimers, slideInTime);
				ADD_STATETIMER(akimboStateTimers, slideLoopTime);
				ADD_STATETIMER(akimboStateTimers, slideOutTime);
				ADD_STATETIMER(akimboStateTimers, speedReloadAddTime);
				ADD_STATETIMER(akimboStateTimers, speedReloadTime);
				ADD_STATETIMER(akimboStateTimers, sprintInTime);
				ADD_STATETIMER(akimboStateTimers, sprintLoopTime);
				ADD_STATETIMER(akimboStateTimers, sprintOutTime);
				ADD_STATETIMER(akimboStateTimers, stunnedTimeBegin);
				ADD_STATETIMER(akimboStateTimers, stunnedTimeEnd);
				ADD_STATETIMER(akimboStateTimers, stunnedTimeLoop);
				ImGui::TreePop();
			}

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
