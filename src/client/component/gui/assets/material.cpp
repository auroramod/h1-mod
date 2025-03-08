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

namespace gui::asset_list::material
{
	namespace
	{
		std::unordered_map<unsigned char, std::string> image_type_names =
		{
			{0x0, "2D"},
			{0x1, "FUNCTION"},
			{0x2, "COLOR_MAP"},
			{0x3, "DETAIL_MAP"},
			{0x4, "UNUSED_2"},
			{0x5, "NORMAL_MAP"},
			{0x6, "UNUSED_3"},
			{0x7, "UNUSED_4"},
			{0x8, "SPECULAR_MAP"},
			{0x9, "UNUSED_5"},
			{0xA, "OCEANFLOW_DISPLACEMENT_MAP"},
			{0xB, "WATER_MAP"},
			{0xC, "OCEAN_DISPLACEMENT_MAP"},
			{0xD, "DISPLACEMENT_MAP"},
			{0xE, "PARALLAX_MAP"},
		};

		std::string get_image_type_name(unsigned char type)
		{
			if (!image_type_names.contains(type))
			{
				return "UNKNOWN";
			}

			return image_type_names[type];
		}

		void copy_constant_table_to_cbt(game::Material* mat)
		{
			for (auto i = 0u; i < mat->constantBufferCount; i++)
			{
				const auto cbt = &mat->constantBufferTable[i];
				for (auto o = 0u; o < mat->constantCount; o++)
				{
#define COPY_CONSTANT_TABLE_VALUES(__data__, __offset_data__) \
						if (cbt->__offset_data__ && cbt->__offset_data__[o] != 0xFFFF) \
						{ \
							const auto constant = reinterpret_cast<float*>(&cbt->__data__[cbt->__offset_data__[o]]); \
							for (auto j = 0; j < 4; j++) \
							{ \
								constant[j] = mat->constantTable[o].literal[j]; \
							} \
						} \

					COPY_CONSTANT_TABLE_VALUES(vsData, vsOffsetData);
					COPY_CONSTANT_TABLE_VALUES(hsData, hsOffsetData);
					COPY_CONSTANT_TABLE_VALUES(dsData, dsOffsetData);
					COPY_CONSTANT_TABLE_VALUES(psData, psOffsetData);
				}

				// free buffers
				game::R_DestroyComputeBuffers(&cbt->vsConstantBuffer);
				game::R_DestroyComputeBuffers(&cbt->hsConstantBuffer);
				game::R_DestroyComputeBuffers(&cbt->dsConstantBuffer);
				game::R_DestroyComputeBuffers(&cbt->psConstantBuffer);
			}
		}

		bool draw_material_window(game::Material* asset)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
			if (ImGui::TreeNode("textures"))
			{
				for (auto i = 0; i < asset->textureCount; i++)
				{
					if (asset->textureTable && asset->textureTable[i].u.image && asset->textureTable[i].u.image->texture.shaderView)
					{
						const auto type_name = get_image_type_name(asset->textureTable[i].semantic);

						ImGui::Text("%s", type_name.data());
						ImGui::SameLine();
						if (ImGui::Button(asset->textureTable[i].u.image->name))
						{
							gui::copy_to_clipboard(asset->textureTable[i].u.image->name);
						}

						const auto width = asset->textureTable[i].u.image->width;
						const auto height = asset->textureTable[i].u.image->height;
						const auto ratio = static_cast<float>(width) / static_cast<float>(height);
						constexpr auto size = 200.f;

						ImGui::Image(asset->textureTable[i].u.image->texture.shaderView,
							ImVec2(size * ratio, size)
						);
					}
				}

				ImGui::TreePop();
			}

#define DRAW_ASSET_PROPERTY_INPUT_U8(__name__) \
				InputU8(#__name__, &asset->__name__); \

#define DRAW_ASSET_PROPERTY_INPUT_S32(__name__) \
				ImGui::InputInt(#__name__, &asset->__name__); \

#define DRAW_ASSET_PROPERTY(__name__, __fmt__) \
				ImGui::Text(#__name__ ": " __fmt__, asset->__name__); \

#define DRAW_ASSET_PROPERTY_COPY(__name__) \
				ImGui::Text(#__name__ ": "); \
				ImGui::SameLine(); \
				if (ImGui::Button(asset->__name__)) \
				{ \
					gui::copy_to_clipboard(asset->__name__); \
				} \

			DRAW_ASSET_PROPERTY_COPY(name);
			DRAW_ASSET_PROPERTY_COPY(techniqueSet->name);

			ImGui::Separator();

			static char buffer[64]{};
			if (ImGui::InputText("techniqueSet", buffer, 64))
			{
				auto* tech = game::DB_FindXAssetHeader(game::ASSET_TYPE_TECHNIQUE_SET, buffer, 0).techniqueSet;
				if (tech)
				{
					asset->techniqueSet = tech;
					memset(buffer, 0, 64);
				}
			}

			ImGui::Separator();

			DRAW_ASSET_PROPERTY(textureCount, "%i");
			DRAW_ASSET_PROPERTY(constantCount, "%i");
			if (asset->constantCount > 0)
			{
				ImGui::Separator();
				for (auto i = 0; i < asset->constantCount; i++)
				{
					char name[13]{};
					memcpy(name, asset->constantTable[i].name, 12);
					ImGui::Text(name);
					ImGui::Text("%d", asset->constantTable[i].nameHash);

					ImGui::PushID(i);
					if (ImGui::DragFloat4("##constant", asset->constantTable[i].literal, 0.01f))
					{
						copy_constant_table_to_cbt(asset);
						utils::hook::invoke<void>(SELECT_VALUE(0x691200_b, 0x691200_b), asset->constantBufferCount, asset); // refresh buffers
					}
					ImGui::PopID();
				}
				ImGui::Separator();
			}

			DRAW_ASSET_PROPERTY(stateBitsCount, "%i");
			if (asset->stateBitsCount > 0)
			{
				ImGui::Separator();
				for (auto i = 0; i < asset->stateBitsCount; i++)
				{
					ImGui::PushID(i);
					if (InputUInt6("##loadBits", asset->stateBitsTable[i].loadBits))
					{
						
					}
					ImGui::PopID();
				}
				ImGui::Separator();
			}

			if (InputU8("info.sortKey", &asset->info.sortKey))
			{
				utils::hook::invoke<void>(SELECT_VALUE(0x56E040_b, 0x6915B0_b)); // Material_DirtySort
			}

			DRAW_ASSET_PROPERTY_INPUT_U8(info.gameFlags);
			DRAW_ASSET_PROPERTY_INPUT_U8(info.renderFlags);

			DRAW_ASSET_PROPERTY_INPUT_U8(cameraRegion);
			DRAW_ASSET_PROPERTY_INPUT_U8(stateFlags);

			DRAW_ASSET_PROPERTY(materialType, "%i");
			DRAW_ASSET_PROPERTY(layerCount, "%i");
			DRAW_ASSET_PROPERTY(assetFlags, "%X");

			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::Material>(game::ASSET_TYPE_MATERIAL, draw_material_window);
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::material::component)
#endif
