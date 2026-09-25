#include <std_include.hpp>

#ifdef _DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"
#include "xmodel.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

namespace gui::asset_list::xmodel
{
	namespace
	{
		struct xmodel_draw_t
		{
			game::XModel* asset;
			game::GfxScaledPlacement placement;
			unsigned short cached_lighting_handle;
			float color_lit[3];
			float color_unlit[3];
			float color_emissive[3];
		};

		std::vector<xmodel_draw_t> spawned_xmodels;

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
			static float scale = 1.f;
			if (ImGui::Button("spawn model"))
			{
				spawn_xmodel(asset, scale);
			}

			ImGui::SameLine();
			ImGui::DragFloat("model scale", &scale, 0.1f, 0.f, 10.f);

			auto id = 0;
			for (auto i = spawned_xmodels.begin(); i != spawned_xmodels.end(); )
			{
				if (i->asset != asset)
				{
					++i;
					continue;
				}

				ImGui::Text("(%f, %f, %f)", i->placement.base.origin[0], i->placement.base.origin[1], i->placement.base.origin[2]);
				ImGui::SameLine();
				ImGui::PushID(id++);
				if (ImGui::Button("delete"))
				{
					i = spawned_xmodels.erase(i);
				}
				else
				{
					++i;
				}
				ImGui::PopID();
			}

			static bool flip_axis = false;
			ImGui::Checkbox("flip axis", &flip_axis);

			ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
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
			DRAW_ASSET_PROPERTY(radius, "%f");
			DRAW_ASSET_PROPERTY(contents, "%i");
			ImGui::Text("bounds.midPoint: (%f, %f, %f)", asset->bounds.midPoint[0], asset->bounds.midPoint[1], asset->bounds.midPoint[2]);
			ImGui::Text("bounds.halfSize: (%f, %f, %f)", asset->bounds.halfSize[0], asset->bounds.halfSize[1], asset->bounds.halfSize[2]);

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

			return true;
		}

		utils::hook::detour r_generate_sorted_draw_surfs_hook;
		void r_generate_sorted_draw_surfs_stub(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6)
		{
			for (auto& model : spawned_xmodels)
			{
				game::R_FilterXModelIntoScene(model.asset, &model.placement, 1, &model.cached_lighting_handle, 
					model.color_lit, model.color_unlit, model.color_emissive);
			}

			r_generate_sorted_draw_surfs_hook.invoke<void>(a1, a2, a3, a4, a5, a6);
		}

		void spawn_xmodel_button(game::XModel* asset)
		{
			spawn_xmodel(asset, 1.f);
		}

		void update()
		{
			if (!game::CL_IsCgameInitialized())
			{
				spawned_xmodels.clear();
			}
		}

		float distance_3d(float* a, float* b)
		{
			return std::sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));
		}
	}

	void spawn_xmodel(game::XModel* asset, const float scale)
	{
		const auto refdef = (*game::refdef);
		if (!game::CL_IsCgameInitialized() || refdef == nullptr)
		{
			return;
		}

		xmodel_draw_t xmodel_draw{};
		xmodel_draw.asset = asset;

		float angles[3]{};
		float forward[3]{};
		game::AxisToAngles(refdef->view.axis, angles);
		game::AngleVectors(angles, forward, nullptr, nullptr);

		auto forward_dist = 50.f;

		const auto is_too_close = [&]()
		{
			for (auto& model : spawned_xmodels)
			{
				if (model.asset == asset && 
					distance_3d(model.placement.base.origin, xmodel_draw.placement.base.origin) < 50.f)
				{
					return true;
				}
			}

			return false;
		};

		xmodel_draw.placement.base.origin[0] = refdef->view.org[0] + forward[0] * forward_dist;
		xmodel_draw.placement.base.origin[1] = refdef->view.org[1] + forward[1] * forward_dist;
		xmodel_draw.placement.base.origin[2] = refdef->view.org[2] + forward[2] * forward_dist;

		while (is_too_close())
		{
			forward_dist += 50.f;
			xmodel_draw.placement.base.origin[0] += forward[0] * forward_dist;
			xmodel_draw.placement.base.origin[1] += forward[1] * forward_dist;
			xmodel_draw.placement.base.origin[2] += forward[2] * forward_dist;
		}

		xmodel_draw.placement.base.quat[0] = 0.f;
		xmodel_draw.placement.base.quat[1] = 0.f;
		xmodel_draw.placement.base.quat[2] = 0.f;
		xmodel_draw.placement.base.quat[3] = 1.f;

		xmodel_draw.placement.scale = scale;

		xmodel_draw.cached_lighting_handle = 0;

		xmodel_draw.color_lit[0] = 1.f;
		xmodel_draw.color_lit[1] = 1.f;
		xmodel_draw.color_lit[2] = 1.f;

		xmodel_draw.color_unlit[0] = 1.f;
		xmodel_draw.color_unlit[1] = 1.f;
		xmodel_draw.color_unlit[2] = 1.f;

		xmodel_draw.color_emissive[0] = 1.f;
		xmodel_draw.color_emissive[1] = 1.f;
		xmodel_draw.color_emissive[2] = 1.f;

		spawned_xmodels.emplace_back(xmodel_draw);
	}


	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			r_generate_sorted_draw_surfs_hook.create(0x1BFC60_b, r_generate_sorted_draw_surfs_stub);

			scheduler::loop(update, scheduler::main);

			gui::asset_list::add_asset_view<game::XModel>(game::ASSET_TYPE_XMODEL, draw_xmodel_window);
			gui::asset_list::add_asset_button<game::XModel>(game::ASSET_TYPE_XMODEL, "spawn", spawn_xmodel_button, game::CL_IsCgameInitialized);

			command::add("spawn_xmodel", [](const command::params& params)
			{
				if (!game::CL_IsCgameInitialized())
				{
					return;
				}

				const auto name = params.get(1);
				const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_XMODEL, name, 0);
				if (asset.model == nullptr)
				{
					return;
				}

				spawn_xmodel(asset.model);
			});

			command::add("clear_spawned_xmodels", []
			{
				spawned_xmodels.clear();
			});
		}
	};
}


REGISTER_COMPONENT(gui::asset_list::xmodel::component)
#endif
