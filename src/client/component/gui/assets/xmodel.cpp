#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/fastfiles.hpp"
#include "component/gsc/script_loading.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"

#include "utils/mapents.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>
#include <game/scripting/execution.hpp>

namespace gui::asset_list::xmodel
{
	namespace
	{
		ImVec2 project_vertex(game::vec3_t v, bool flip_axis, float scale = 1.f,
			bool rotate = false, float rotation_speed = 0.f)
		{
			constexpr auto left = -1.0f;
			constexpr auto right = 1.0f;
			constexpr auto bottom = -1.0f;
			constexpr auto top = 1.0f;

			auto rotation_angle = 0.f;
			if (rotate)
			{
				rotation_angle = game::Sys_Milliseconds() * rotation_speed;
			}

			const auto cos_angle = std::cos(rotation_angle);
			const auto sin_angle = std::sin(rotation_angle);

			float o_x = 0.f;
			float o_y = 0.f;
			float o_z = 0.f;

			if (flip_axis)
			{
				o_x = v[0] * scale;
				o_y = v[1] * scale;
				o_z = v[2] * scale;
			}
			else
			{
				o_x = v[1] * scale;
				o_y = v[0] * scale;
				o_z = v[2] * scale;
			}

			const auto x = o_x * cos_angle - o_z * sin_angle;
			const auto y = o_y;

			const auto proj_x = (2.0f * x) / (right - left) - (right + left) / (right - left);
			const auto proj_y = (2.0f * y) / (top - bottom) - (top + bottom) / (top - bottom);

			const auto screen_x = (proj_x + 1.0f) * 0.5f * 50.f;
			const auto screen_y = (1.0f - proj_y) * 0.5f * 50.f;

			return ImVec2(screen_x, screen_y);
		}

		int sum_verts(game::XSurface* surf, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin)
		{
			for (auto i = 0u; i < surf->triCount; i++)
			{
				game::GfxPackedVertex* vertices[3]{};

				vertices[0] = &surf->verts0.packedVerts0[surf->triIndices[i].v1];
				vertices[1] = &surf->verts0.packedVerts0[surf->triIndices[i].v2];
				vertices[2] = &surf->verts0.packedVerts0[surf->triIndices[i].v3];

				for (auto o = 0; o < 3; o++)
				{
					for (auto j = 0; j < 3; j++)
					{
						origin[j] += vertices[o]->xyz[j];

						if (vertices[o]->xyz[j] > maxs[j])
						{
							maxs[j] = vertices[o]->xyz[j];
						}

						if (vertices[o]->xyz[j] < mins[j])
						{
							mins[j] = vertices[o]->xyz[j];
						}
					}
				}
			}

			return surf->triCount * 2;
		}

		void draw_surf(game::XSurface* surf, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin,
			game::vec2_t maxs_2d, ImVec2 window_pos, bool flip_axis)
		{
			const auto draw_list = ImGui::GetWindowDrawList();
			constexpr auto max_size = 10.f;
			const auto extent = std::sqrt(maxs[0] * maxs[0] + maxs[1] * maxs[1] + maxs[2] * maxs[2]);
			const auto scale = max_size / extent;

			for (auto i = 0u; i < surf->triCount; i++)
			{
				game::GfxPackedVertex* vertices[3]{};

				vertices[0] = &surf->verts0.packedVerts0[surf->triIndices[i].v1];
				vertices[1] = &surf->verts0.packedVerts0[surf->triIndices[i].v2];
				vertices[2] = &surf->verts0.packedVerts0[surf->triIndices[i].v3];

				ImVec2 points[3]{};

				for (auto o = 0; o < 3; o++)
				{
					game::vec3_t trans_vertex{};
					trans_vertex[0] = vertices[o]->xyz[0] - origin[0];
					trans_vertex[1] = vertices[o]->xyz[1] - origin[1];
					trans_vertex[2] = vertices[o]->xyz[2] - origin[2];
					points[o] = project_vertex(trans_vertex, flip_axis, scale, true, 0.001f);

					if (maxs_2d[0] < points[o].x)
					{
						maxs_2d[0] = points[o].x;
					}

					if (maxs_2d[1] < points[o].y)
					{
						maxs_2d[1] = points[o].y;
					}

					points[o][0] += window_pos.x;
					points[o][1] += window_pos.y;
				}

				draw_list->AddLine(points[0], points[1], IM_COL32_WHITE, 1.f);
				draw_list->AddLine(points[1], points[2], IM_COL32_WHITE, 1.f);
				draw_list->AddLine(points[2], points[0], IM_COL32_WHITE, 1.f);
			}
		}


		int sum_verts_in_xmodels(game::XModel* asset, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin)
		{
			int vert_count = 0;

			for (auto o = 0; o < asset->lodInfo[0].numsurfs; o++)
			{
				const auto surf = &asset->lodInfo[0].surfs[o];
				if (surf)
				{
					vert_count += sum_verts(surf, mins, maxs, origin);
				}
			}

			return vert_count;
		}

		void draw_xmodel(game::XModel* asset, bool flip_axis, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin, game::vec3_t maxs_2d)
		{
			if (asset->numLods > 0)
			{
				auto window_pos = ImGui::GetCursorScreenPos();
				constexpr auto offset = ImVec2(400, 150);
				window_pos.x += offset.x;
				window_pos.y += offset.y;

				for (auto o = 0; o < asset->lodInfo[0].numsurfs; o++)
				{
					const auto surf = &asset->lodInfo[0].surfs[o];
					if (surf)
					{
						draw_surf(surf, mins, maxs, origin, maxs_2d, window_pos, flip_axis);
					}
				}
			}
		}

		bool draw_xmodel_window(game::XModel* asset)
		{
			static bool flip_axis = false;
			ImGui::Checkbox("flip axis", &flip_axis);

			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
			if (ImGui::TreeNode("3d mesh"))
			{
				int vert_count = 0;

				game::vec3_t mins{};
				game::vec3_t maxs{};
				game::vec3_t origin{};
				game::vec2_t maxs_2d{};

				vert_count += sum_verts_in_xmodels(asset, mins, maxs, origin);
				for (auto i = 0; i < asset->numCompositeModels; i++)
				{
					vert_count += sum_verts_in_xmodels(asset->compositeModels[i], mins, maxs, origin);
				}

				for (auto o = 0; o < 3; o++)
				{
					origin[o] /= static_cast<float>(vert_count);
				}

				draw_xmodel(asset, flip_axis, mins, maxs, origin, maxs_2d);
				for (auto i = 0; i < asset->numCompositeModels; i++)
				{
					draw_xmodel(asset->compositeModels[i], flip_axis, mins, maxs, origin, maxs_2d);
				}

				ImGui::Dummy(ImVec2(maxs_2d[0] + 800, maxs_2d[1] + 250));
				ImGui::TreePop();
			}

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
			DRAW_ASSET_PROPERTY(numRootBones, "%i");
			DRAW_ASSET_PROPERTY(numsurfs, "%i");
			DRAW_ASSET_PROPERTY(lodRampType, "%i");
			DRAW_ASSET_PROPERTY(numBonePhysics, "%i");
			DRAW_ASSET_PROPERTY(numCompositeModels, "%i");
			DRAW_ASSET_PROPERTY(scale, "%f");

			if (ImGui::TreeNode("bones"))
			{
				for (auto i = 0; i < asset->numBones; i++)
				{
					const auto bone_name = game::SL_ConvertToString(asset->boneNames[i]);
					if (bone_name)
					{
						if (ImGui::Button(bone_name))
						{
							gui::copy_to_clipboard(bone_name);
						}
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("lods"))
			{
				for (auto i = 0; i < asset->numLods; i++)
				{
					const auto lod = asset->lodInfo[i];
					if (lod.modelSurfs)
					{
						if (ImGui::Button(lod.modelSurfs->name))
						{
							gui::copy_to_clipboard(lod.modelSurfs->name);
						}
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("surface materials"))
			{
				game::Material* prev_material = nullptr;

				for (auto i = 0; i < asset->numsurfs; i++)
				{
					if (prev_material == asset->materialHandles[i])
					{
						continue;
					}

					prev_material = asset->materialHandles[i];

					if (ImGui::Button(asset->materialHandles[i]->name))
					{
						gui::copy_to_clipboard(asset->materialHandles[i]->name);
					}

					add_view_button(i, game::ASSET_TYPE_MATERIAL, asset->materialHandles[i]->name);
				}

				ImGui::TreePop();
			}

			if (asset->numCompositeModels > 0)
			{
				if (ImGui::TreeNode("composite models"))
				{
					for (auto i = 0; i < asset->numCompositeModels; i++)
					{
						if (ImGui::Button(asset->compositeModels[i]->name))
						{
							gui::copy_to_clipboard(asset->compositeModels[i]->name);
						}

						gui::asset_list::add_view_button(i, game::ASSET_TYPE_XMODEL, asset->compositeModels[i]->name);
					}

					ImGui::TreePop();
				}
			}

			if (ImGui::Button("Spawn Model"))
			{
				auto host = scripting::call("getentbynum", { 0 }).as<scripting::entity>();
				auto hostOrigin = host.call("getorigin").as<scripting::vector>();

				auto spawned_model = scripting::call("spawn", { "script_model", hostOrigin }).as<scripting::entity>();
				scripting::call("precachemodel", { asset->name });
				spawned_model.call("setmodel", { asset->name });
			}

			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::XModel>(game::ASSET_TYPE_XMODEL, draw_xmodel_window);
		}
	};
}


REGISTER_COMPONENT(gui::asset_list::xmodel::component)
#endif
