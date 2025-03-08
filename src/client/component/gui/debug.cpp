#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "gui.hpp"

#include "game/scripting/execution.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>

namespace gui::debug
{
	namespace
	{
		struct debug_line
		{
			float start[3];
			float end[3];
			float color[4];
			bool deleted;
		};

		struct debug_square
		{
			float origin[3];
			float color[4];
			bool deleted;
		};

		std::vector<debug_line> debug_lines;
		std::vector<debug_square> debug_squares;
		std::mutex debug_items_mutex;

		game::dvar_t* cl_paused = nullptr;

		game::dvar_t* r_drawModelBoundingBoxes = nullptr;

		enum object_type
		{
			square,
			circle,
			circle_fill,
			cube,
			cube_mesh
		};

		enum entity_type
		{
			other,
			trigger_radius,
			generic_trigger,
			actor,
			count
		};

		float camera[3] = {};
		float axis[3][3] = {};

		struct draw_settings
		{
			bool enabled;
			bool camera_locked;
			object_type type;
			float mesh_thickness = 1.f;
			float range = 500.f;
			float camera[3] = {};
		};

		struct : draw_settings
		{
			bool draw_node_links;
			float size = 10.f;
			float link_thickness = 1.f;
			float color[4] = { 1.f, 0.f, 0.f, 1.f };
		} path_node_settings{};

		struct : draw_settings
		{
			bool enabled_types[entity_type::count];
			float type_colors[entity_type::count][4] =
			{
				{0.f, 0.5f, 1.f, 0.3f},
				{0.f, 1.f, 0.f, 0.3f},
				{1.0f, 1.f, 0.f, 0.3f},
				{1.f, 0.5f, 1.f, 0.3f},
			};
			bool mesh_only = false;
			int point_count = 30;
		} entity_bound_settings{};

		struct point
		{
			ImVec2 point;
			bool valid;
		};

		float vector_dot(float* a, float* b)
		{
			return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
		}

		bool world_pos_to_screen_pos(float* origin, float* out)
		{
			float local[3] =
			{
				origin[0] - camera[0],
				origin[1] - camera[1],
				origin[2] - camera[2]
			};

			float transform[3] =
			{
				vector_dot(local, axis[1]),
				vector_dot(local, axis[2]),
				vector_dot(local, axis[0])
			};

			if (transform[2] < 0.01f)
			{
				return false;
			}

			const auto width = game::ScrPlace_GetViewPlacement()->realViewportSize[0];
			const auto height = game::ScrPlace_GetViewPlacement()->realViewportSize[1];

			auto refdef = *game::refdef;
			if (refdef == nullptr)
			{
				return false;
			}

			out[0] = (width / 2) * (1 - transform[0] / refdef->view.tanHalfFovX / transform[2]);
			out[1] = (height / 2) * (1 - transform[1] / refdef->view.tanHalfFovY / transform[2]);

			return true;
		}

		void draw_line(float* start, float* end, float* color, float thickness)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			float start_screen[2] = {};
			float end_screen[2] = {};

			auto result = 1;
			result *= world_pos_to_screen_pos(start, start_screen);
			result *= world_pos_to_screen_pos(end, end_screen);

			if (!result)
			{
				return;
			}

			const auto start_ = ImVec2(start_screen[0], start_screen[1]);
			const auto end_ = ImVec2(end_screen[0], end_screen[1]);
			const auto color_ = ImGui::GetColorU32({ color[0], color[1], color[2], color[3] });

			window->DrawList->AddLine(start_, end_, color_, thickness);
		}

		void draw_square(float* origin, float width, float* color)
		{
			const auto half = width / 2.f;
			float p1[3] = { origin[0] - half, origin[1] + half, origin[2] };
			float p2[3] = { origin[0] + half, origin[1] + half, origin[2] };
			float p3[3] = { origin[0] + half, origin[1] - half, origin[2] };
			float p4[3] = { origin[0] - half, origin[1] - half, origin[2] };

			float p1_screen[2] = {};
			float p2_screen[2] = {};
			float p3_screen[2] = {};
			float p4_screen[2] = {};

			auto result = 1;
			result *= world_pos_to_screen_pos(p1, p1_screen);
			result *= world_pos_to_screen_pos(p2, p2_screen);
			result *= world_pos_to_screen_pos(p3, p3_screen);
			result *= world_pos_to_screen_pos(p4, p4_screen);

			if (!result)
			{
				return;
			}

			ImGuiWindow* window = ImGui::GetCurrentWindow();

			ImVec2 points[4] =
			{
				ImVec2(p1_screen[0], p1_screen[1]),
				ImVec2(p2_screen[0], p2_screen[1]),
				ImVec2(p3_screen[0], p3_screen[1]),
				ImVec2(p4_screen[0], p4_screen[1])
			};

			const auto color_ = ImGui::GetColorU32({ color[0], color[1], color[2], color[3] });
			window->DrawList->AddConvexPolyFilled(points, 4, color_);
		}

		void draw_square_from_points(float* p1, float* p2, float* p3, float* p4, float* color,
			float thickness, bool mesh_only)
		{
			float p1_screen[2] = {};
			float p2_screen[2] = {};
			float p3_screen[2] = {};
			float p4_screen[2] = {};

			auto result = 1;
			result *= world_pos_to_screen_pos(p1, p1_screen);
			result *= world_pos_to_screen_pos(p2, p2_screen);
			result *= world_pos_to_screen_pos(p3, p3_screen);
			result *= world_pos_to_screen_pos(p4, p4_screen);

			if (!result)
			{
				return;
			}

			ImGuiWindow* window = ImGui::GetCurrentWindow();

			const auto color_ = ImGui::GetColorU32({ color[0], color[1], color[2], color[3] });

			window->DrawList->PathClear();
			window->DrawList->PathLineTo(ImVec2(p1_screen[0], p1_screen[1]));
			window->DrawList->PathLineTo(ImVec2(p2_screen[0], p2_screen[1]));
			window->DrawList->PathLineTo(ImVec2(p3_screen[0], p3_screen[1]));
			window->DrawList->PathLineTo(ImVec2(p4_screen[0], p4_screen[1]));
			window->DrawList->PathLineTo(ImVec2(p1_screen[0], p1_screen[1]));

			if (mesh_only)
			{
				window->DrawList->PathStroke(color_, 0, thickness);
			}
			else
			{
				window->DrawList->PathFillConvex(color_);
			}
		}

		void draw_cube(float* origin, float width, float* color, float thickness, bool mesh_only)
		{
			const auto half = width / 2.f;

			float p1[3] = { origin[0] - half, origin[1] + half, origin[2] };
			float p2[3] = { origin[0] + half, origin[1] + half, origin[2] };
			float p3[3] = { origin[0] + half, origin[1] - half, origin[2] };
			float p4[3] = { origin[0] - half, origin[1] - half, origin[2] };

			float p1_top[3] = { p1[0], p1[1], origin[2] + width };
			float p2_top[3] = { p2[0], p2[1], origin[2] + width };
			float p3_top[3] = { p3[0], p3[1], origin[2] + width };
			float p4_top[3] = { p4[0], p4[1], origin[2] + width };

			draw_square_from_points(p1, p2, p3, p4, color, thickness, mesh_only);
			draw_square_from_points(p1_top, p2_top, p3_top, p4_top, color, thickness, mesh_only);

			draw_square_from_points(p3, p2, p2_top, p3_top, color, thickness, mesh_only);
			draw_square_from_points(p4, p1, p1_top, p4_top, color, thickness, mesh_only);

			draw_square_from_points(p1, p2, p2_top, p1_top, color, thickness, mesh_only);
			draw_square_from_points(p4, p3, p3_top, p4_top, color, thickness, mesh_only);
		}

		float get_pi()
		{
			static const auto pi = static_cast<float>(atan(1)) * 4.f;
			return pi;
		}

		void draw_cylinder(float* center, float radius, float height, int point_count, float* color,
			float thickness, bool mesh_only)
		{
			const auto pi = get_pi();

			ImGuiWindow* window = ImGui::GetCurrentWindow();
			const auto color_ = ImGui::GetColorU32({ color[0], color[1], color[2], color[3] });

			point_count = std::max(0, std::min(point_count, 360));
			const auto step = 360.f / point_count;

			point points_top[360];
			point points_bottom[360];

			auto point_index = 0;
			for (auto angle = 0.f; angle < 360.f; angle += step)
			{
				const auto x = center[0] + radius * cos(static_cast<float>(angle) * pi / 180.f);
				const auto y = center[1] + radius * sin(static_cast<float>(angle) * pi / 180.f);

				float point[3] = { x, y, center[2] };
				float point_top[3] = { x, y, center[2] + height };

				float point_screen[2] = {};
				float point_top_screen[2] = {};

				const auto index = point_index++;

				auto result = 1;
				result *= world_pos_to_screen_pos(point, point_screen);
				result *= world_pos_to_screen_pos(point_top, point_top_screen);

				if (!result)
				{
					points_bottom[index] = { {}, false };
					points_top[index] = { {}, false };
					continue;
				}

				points_bottom[index] = { ImVec2(point_screen[0], point_screen[1]), true };
				points_top[index] = { ImVec2(point_top_screen[0], point_top_screen[1]), true };
			}

			window->DrawList->PathClear();
			for (auto i = 0; i < point_count; i++)
			{
				if (!points_bottom[i].valid)
				{
					continue;
				}

				window->DrawList->PathLineTo(points_bottom[i].point);
			}

			if (mesh_only)
			{
				window->DrawList->PathStroke(color_, 0, thickness);
			}
			else
			{
				window->DrawList->PathFillConvex(color_);
			}

			window->DrawList->PathClear();
			for (auto i = 0; i < point_count; i++)
			{
				if (!points_top[i].valid)
				{
					continue;
				}

				window->DrawList->PathLineTo(points_top[i].point);
			}

			if (mesh_only)
			{
				window->DrawList->PathStroke(color_, 0, thickness);
			}
			else
			{
				window->DrawList->PathFillConvex(color_);
			}

			for (auto i = 0; i < point_count; i++)
			{
				window->DrawList->PathClear();

				if (!points_bottom[i].valid ||
					!points_top[i].valid)
				{
					continue;
				}

				window->DrawList->PathLineTo(points_bottom[i].point);

				if (i == point_count - 1)
				{
					if (!points_bottom[0].valid || !points_top[0].valid)
					{
						continue;
					}

					window->DrawList->PathLineTo(points_bottom[0].point);
					window->DrawList->PathLineTo(points_top[0].point);
				}
				else
				{
					if (!points_bottom[i + 1].valid || !points_top[i + 1].valid)
					{
						continue;
					}

					window->DrawList->PathLineTo(points_bottom[i + 1].point);
					window->DrawList->PathLineTo(points_top[i + 1].point);
				}

				window->DrawList->PathLineTo(points_top[i].point);

				if (mesh_only)
				{
					window->DrawList->PathStroke(color_, 0, thickness);
				}
				else
				{
					window->DrawList->PathFillConvex(color_);
				}
			}
		}

		void draw_rectangular_prism(float* center, game::Bounds bounds, float* color, float thickness, bool mesh_only)
		{
			float vertices[8][3] =
			{
				{center[0] - bounds.halfSize[0], center[1] - bounds.halfSize[1], center[2] - bounds.halfSize[2]},
				{center[0] - bounds.halfSize[0], center[1] + bounds.halfSize[1], center[2] - bounds.halfSize[2]},
				{center[0] + bounds.halfSize[0], center[1] + bounds.halfSize[1], center[2] - bounds.halfSize[2]},
				{center[0] + bounds.halfSize[0], center[1] - bounds.halfSize[1], center[2] - bounds.halfSize[2]},

				{center[0] - bounds.halfSize[0], center[1] - bounds.halfSize[1], center[2] + bounds.halfSize[2]},
				{center[0] - bounds.halfSize[0], center[1] + bounds.halfSize[1], center[2] + bounds.halfSize[2]},
				{center[0] + bounds.halfSize[0], center[1] + bounds.halfSize[1], center[2] + bounds.halfSize[2]},
				{center[0] + bounds.halfSize[0], center[1] - bounds.halfSize[1], center[2] + bounds.halfSize[2]},
			};

			draw_square_from_points(vertices[0], vertices[1], vertices[2], vertices[3], color, thickness, mesh_only);
			draw_square_from_points(vertices[4], vertices[5], vertices[6], vertices[7], color, thickness, mesh_only);

			draw_square_from_points(vertices[0], vertices[4], vertices[5], vertices[1], color, thickness, mesh_only);
			draw_square_from_points(vertices[7], vertices[6], vertices[2], vertices[3], color, thickness, mesh_only);

			draw_square_from_points(vertices[6], vertices[5], vertices[1], vertices[2], color, thickness, mesh_only);
			draw_square_from_points(vertices[7], vertices[4], vertices[0], vertices[3], color, thickness, mesh_only);
		}

		void draw_window()
		{
			static auto* enabled = &gui::enabled_menus["debug"];
			if (!*enabled)
			{
				return;
			}

			ImGui::Begin("Debug", enabled);

			if (ImGui::TreeNode("Path nodes"))
			{
				ImGui::Checkbox("Draw", &path_node_settings.enabled);
				ImGui::Checkbox("Lock camera", &path_node_settings.camera_locked);

				ImGui::Checkbox("Draw node links", &path_node_settings.draw_node_links);

				if (ImGui::TreeNode("Object type"))
				{
					ImGui::RadioButton("square", reinterpret_cast<int*>(&path_node_settings.type), object_type::square);
					ImGui::RadioButton("cube", reinterpret_cast<int*>(&path_node_settings.type), object_type::cube);
					ImGui::RadioButton("cube mesh", reinterpret_cast<int*>(&path_node_settings.type), object_type::cube_mesh);

					ImGui::TreePop();
				}

				ImGui::SliderFloat("range", &path_node_settings.range, 0.f, 10000.f);
				ImGui::SliderFloat("size", &path_node_settings.size, 5.f, 100.f);

				if (path_node_settings.draw_node_links)
				{
					ImGui::SliderFloat("link thickness", &path_node_settings.link_thickness, 1.f, 20.f);
				}

				if (path_node_settings.type == object_type::cube_mesh)
				{
					ImGui::SliderFloat("mesh thickness", &path_node_settings.mesh_thickness, 1.f, 20.f);
				}

				if (ImGui::TreeNode("Color picker"))
				{
					ImGui::ColorPicker4("color", path_node_settings.color);
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Entity bounds"))
			{
				ImGui::Checkbox("Draw", &entity_bound_settings.enabled);
				ImGui::Checkbox("Lock camera", &entity_bound_settings.camera_locked);

				if (ImGui::TreeNode("Types"))
				{
					ImGui::Checkbox("trigger_radius", &entity_bound_settings.enabled_types[entity_type::trigger_radius]);
					if (entity_bound_settings.enabled_types[entity_type::trigger_radius] && ImGui::TreeNode("Color picker #1"))
					{
						ImGui::ColorPicker4("color", entity_bound_settings.type_colors[entity_type::trigger_radius]);
						ImGui::TreePop();
					}

					ImGui::Checkbox("generic trigger", &entity_bound_settings.enabled_types[entity_type::generic_trigger]);
					if (entity_bound_settings.enabled_types[entity_type::generic_trigger] && ImGui::TreeNode("Color picker #2"))
					{
						ImGui::ColorPicker4("color", entity_bound_settings.type_colors[entity_type::generic_trigger]);
						ImGui::TreePop();
					}

					ImGui::Checkbox("actor", &entity_bound_settings.enabled_types[entity_type::actor]);
					if (entity_bound_settings.enabled_types[entity_type::actor] && ImGui::TreeNode("Color picker #3"))
					{
						ImGui::ColorPicker4("color", entity_bound_settings.type_colors[entity_type::actor]);
						ImGui::TreePop();
					}

					ImGui::Checkbox("other entities", &entity_bound_settings.enabled_types[entity_type::other]);
					if (entity_bound_settings.enabled_types[entity_type::other] && ImGui::TreeNode("Color picker #4"))
					{
						ImGui::ColorPicker4("color", entity_bound_settings.type_colors[entity_type::other]);
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				ImGui::SliderFloat("range", &entity_bound_settings.range, 0.f, 10000.f);
				ImGui::SliderFloat("mesh thickness", &entity_bound_settings.mesh_thickness, 1.f, 20.f);
				ImGui::Checkbox("mesh only", &entity_bound_settings.mesh_only);
				ImGui::SliderInt("circle max points", &entity_bound_settings.point_count, 3, 360);

				ImGui::TreePop();
			}

			ImGui::End();
		}

		float distance_2d(float* a, float* b)
		{
			return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
		}

		void get_pathnode_origin(game::pathnode_t* node, float* out)
		{
			out[0] = node->constant.vLocalOrigin[0];
			out[1] = node->constant.vLocalOrigin[1];
			out[2] = node->constant.vLocalOrigin[2];

			game::WorldifyPosFromParent(node, out);
		}

		void draw_node_links(game::pathnode_t* node, float* origin)
		{
			for (unsigned int i = 0; i < node->constant.totalLinkCount; i++)
			{
				float linked_origin[3] = {};
				const auto num = node->constant.Links[i].nodeNum;
				auto pathdata = *game::pathdata;
				const auto linked = &pathdata.nodes[num];

				get_pathnode_origin(linked, linked_origin);
				if (distance_2d(path_node_settings.camera, linked_origin) < path_node_settings.range)
				{
					draw_line(origin, linked_origin, path_node_settings.color,
						path_node_settings.link_thickness);
				}
			}
		}

		void begin_render_window()
		{
			const auto& io = ImGui::GetIO();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
			ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
			ImGui::Begin("debug window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

			ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
		}

		void end_render_window()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			window->DrawList->PushClipRectFullScreen();

			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar(2);
		}

		void draw_nodes()
		{
			if (!path_node_settings.enabled)
			{
				return;
			}

			auto pathdata = *game::pathdata;
			for (unsigned int i = 0; i < pathdata.nodeCount; i++)
			{
				float origin[3] = {};
				const auto node = &pathdata.nodes[i];

				get_pathnode_origin(node, origin);
				if (distance_2d(path_node_settings.camera, origin) >= path_node_settings.range)
				{
					continue;
				}

				float screen_center[2]{};
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				world_pos_to_screen_pos(origin, screen_center);
				window->DrawList->AddText(ImGui::GetDefaultFont(), ImGui::GetFontSize(), ImVec2(screen_center[0],
					screen_center[1]), ImColor(255, 255, 0, 255), utils::string::va("%i", node->constant.type), 0, 0.f, 0);

				switch (path_node_settings.type)
				{
				case object_type::square:
					draw_square(origin, path_node_settings.size, path_node_settings.color);
					break;
				case object_type::cube_mesh:
				case object_type::cube:
					draw_cube(origin, path_node_settings.size, path_node_settings.color,
						path_node_settings.mesh_thickness, path_node_settings.type == object_type::cube_mesh);
					break;
				}

				if (path_node_settings.draw_node_links)
				{
					draw_node_links(node, origin);
				}
			}
		}

		entity_type get_entity_type(const char* classname)
		{
			if (strstr(classname, "trigger_radius"))
			{
				return entity_type::trigger_radius;
			}
			else if (strstr(classname, "trigger"))
			{
				return entity_type::generic_trigger;
			}
			else if (strstr(classname, "actor"))
			{
				return entity_type::actor;
			}

			return entity_type::other;
		}

		void draw_triggers()
		{
			if (!entity_bound_settings.enabled)
			{
				return;
			}

			for (auto i = 0; i < *game::mp::num_entities; i++)
			{
				const auto entity = &game::mp::g_entities[i];
				const auto origin = entity->origin;

				const auto distance = distance_2d(entity_bound_settings.camera, origin);
				const auto* classname = game::SL_ConvertToString(entity->script_classname);

				if (distance > entity_bound_settings.range || !classname)
				{
					continue;
				}

				if (!strcmp(classname, "player"))
					continue;

				const auto type = get_entity_type(classname);
				if (!entity_bound_settings.enabled_types[type])
				{
					continue;
				}

				switch (type)
				{
				case entity_type::trigger_radius:
				{
					const auto radius = entity->box.halfSize[0];
					const auto height = entity->box.halfSize[2] * 2.f;

					draw_cylinder(origin, radius, height, entity_bound_settings.point_count,
						entity_bound_settings.type_colors[type], entity_bound_settings.mesh_thickness, entity_bound_settings.mesh_only);
					break;
				}
				default:
				{
					draw_rectangular_prism(origin, entity->box, entity_bound_settings.type_colors[type],
						entity_bound_settings.mesh_thickness, entity_bound_settings.mesh_only);
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					float screen_center[2];
					world_pos_to_screen_pos(origin, screen_center);
					window->DrawList->AddText(ImGui::GetDefaultFont(), ImGui::GetFontSize(), ImVec2(screen_center[0],
						screen_center[1]), ImColor(255, 255, 0, 255), utils::string::va("%s - %.f %.f %.f", game::SL_ConvertToString(entity->script_classname), origin[0], origin[1], origin[2]), 0, 0.f, 0);
					break;
				}
				}
			}
		}

		void draw_debug_items()
		{
			std::lock_guard _0(debug_items_mutex);

			for (auto& line : debug_lines)
			{
				if (line.deleted)
				{
					continue;
				}

				draw_line(line.start, line.end, line.color, 1.f);
			}

			for (auto& square : debug_squares)
			{
				if (square.deleted)
				{
					continue;
				}

				draw_square(square.origin, 50.f, square.color);
			}
		}

		void update_camera()
		{
			auto refdef = *game::refdef;
			if (refdef == nullptr)
			{
				return;
			}

			camera[0] = refdef->view.org[0];
			camera[1] = refdef->view.org[1];
			camera[2] = refdef->view.org[2];

			axis[0][0] = refdef->view.axis[0][0];
			axis[0][1] = refdef->view.axis[0][1];
			axis[0][2] = refdef->view.axis[0][2];

			axis[1][0] = refdef->view.axis[1][0];
			axis[1][1] = refdef->view.axis[1][1];
			axis[1][2] = refdef->view.axis[1][2];

			axis[2][0] = refdef->view.axis[2][0];
			axis[2][1] = refdef->view.axis[2][1];
			axis[2][2] = refdef->view.axis[2][2];

			if (!path_node_settings.camera_locked)
			{
				path_node_settings.camera[0] = camera[0];
				path_node_settings.camera[1] = camera[1];
				path_node_settings.camera[2] = camera[2];
			}

			if (!entity_bound_settings.camera_locked)
			{
				entity_bound_settings.camera[0] = camera[0];
				entity_bound_settings.camera[1] = camera[1];
				entity_bound_settings.camera[2] = camera[2];
			}
		}
	}

	size_t add_debug_line(const float* start, const float* end, const float* color)
	{
		debug_line line{};
		std::memcpy(line.start, start, sizeof(float[3]));
		std::memcpy(line.end, end, sizeof(float[3]));
		std::memcpy(line.color, color, sizeof(float[4]));

		std::lock_guard _0(debug_items_mutex);
		const auto index = debug_lines.size();
		debug_lines.emplace_back(line);
		return index;
	}

	void remove_debug_line(const size_t line)
	{
		std::lock_guard _0(debug_items_mutex);
		if (line >= debug_lines.size())
		{
			return;
		}

		debug_lines[line].deleted = true;
	}

	void set_debug_line_color(size_t line, const float* color)
	{
		std::lock_guard _0(debug_items_mutex);
		if (line >= debug_lines.size())
		{
			return;
		}

		auto& line_ = debug_lines.at(line);
		std::memcpy(line_.color, color, sizeof(float[4]));
	}

	size_t add_debug_square(const float* origin, const float* color)
	{
		debug_square line{};
		std::memcpy(line.origin, origin, sizeof(float[3]));
		std::memcpy(line.color, color, sizeof(float[4]));

		std::lock_guard _0(debug_items_mutex);
		const auto index = debug_squares.size();
		debug_squares.emplace_back(line);
		return index;
	}

	void remove_debug_square(const size_t square)
	{
		std::lock_guard _0(debug_items_mutex);
		if (square >= debug_squares.size())
		{
			return;
		}

		debug_squares[square].deleted = true;
	}

	void set_debug_square_color(size_t square, const float* color)
	{
		std::lock_guard _0(debug_items_mutex);
		if (square >= debug_squares.size())
		{
			return;
		}

		auto& square_ = debug_squares.at(square);
		std::memcpy(square_.color, color, sizeof(float[4]));
	}

	void reset_debug_items()
	{
		std::lock_guard _0(debug_items_mutex);
		debug_lines.clear();
		debug_squares.clear();
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			gui::register_menu("debug", "Debug", draw_window);

			gui::on_frame([]()
			{
				if (!game::SV_Loaded() || game::VirtualLobby_Loaded() || !game::CL_IsCgameInitialized() || cl_paused->current.integer)
				{
					return;
				}

				begin_render_window();
				draw_nodes();
				draw_triggers();
				draw_debug_items();
				end_render_window();
			}, true);

			scheduler::once([]()
			{
				cl_paused = game::Dvar_FindVar("cl_paused");
				r_drawModelBoundingBoxes = dvars::register_int("r_drawModelBoundingBoxes", 0, 1, 3, game::DVAR_FLAG_CHEAT, "Draw model bounding boxes");
			}, scheduler::pipeline::main);

			scheduler::loop([]()
			{
				if (!game::SV_Loaded() || game::VirtualLobby_Loaded() || !game::CL_IsCgameInitialized() || cl_paused->current.integer)
				{
					return;
				}

				update_camera();
			}, scheduler::pipeline::renderer);
		}
	};
}

REGISTER_COMPONENT(gui::debug::component)
#endif // DEBUG
