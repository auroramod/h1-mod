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

namespace gui::asset_list::comworld
{
	namespace
	{
		bool draw_window(game::ComWorld* asset)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
			if (ImGui::TreeNode("primaryLights"))
			{
				for (auto i = 0u; i < asset->primaryLightCount; i++)
				{
					ImGui::PushID(i);

					ImGui::Text("index: %d", i);
					InputU8("type", (uint8_t*)&asset->primaryLights[i].type);
					InputU8("canUseShadowMap", (uint8_t*)&asset->primaryLights[i].canUseShadowMap);
					InputU8("physicallyBased", (uint8_t*)&asset->primaryLights[i].physicallyBased);
					InputU8("exponent", (uint8_t*)&asset->primaryLights[i].exponent);
					ImGui::InputFloat3("color", asset->primaryLights[i].color);
					ImGui::InputFloat3("dir", asset->primaryLights[i].dir);
					ImGui::InputFloat3("up", asset->primaryLights[i].up);
					ImGui::InputFloat3("origin", asset->primaryLights[i].origin);

					ImGui::InputFloat2("fadeOffset", asset->primaryLights[i].fadeOffset);
					ImGui::InputFloat("bulbRadius", &asset->primaryLights[i].bulbRadius);
					ImGui::InputFloat3("bulbLength", asset->primaryLights[i].bulbLength);

					ImGui::InputFloat("radius", &asset->primaryLights[i].radius);
					ImGui::InputFloat("cosHalfFovOuter", &asset->primaryLights[i].cosHalfFovOuter);
					ImGui::InputFloat("cosHalfFovInner", &asset->primaryLights[i].cosHalfFovInner);
					ImGui::InputFloat("cosHalfFovExpanded", &asset->primaryLights[i].cosHalfFovExpanded);
					ImGui::InputFloat("rotationLimit", &asset->primaryLights[i].rotationLimit);
					ImGui::InputFloat("translationLimit", &asset->primaryLights[i].translationLimit);
					ImGui::InputFloat("cucRotationOffsetRad", &asset->primaryLights[i].cucRotationOffsetRad);
					ImGui::InputFloat("cucRotationSpeedRad", &asset->primaryLights[i].cucRotationSpeedRad);
					ImGui::InputFloat2("cucScrollVector", asset->primaryLights[i].cucScrollVector);
					ImGui::InputFloat2("cucScaleVector", asset->primaryLights[i].cucScaleVector);
					ImGui::InputFloat2("cucTransVector", asset->primaryLights[i].cucTransVector);
					ImGui::Text("defName: %s", asset->primaryLights[i].defName);

					ImGui::PopID();

					ImGui::Separator();
				}

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
			gui::asset_list::add_asset_view<game::ComWorld>(game::ASSET_TYPE_COMWORLD, draw_window);
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::comworld::component)
#endif
