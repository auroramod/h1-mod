#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace experimental
{
	namespace
	{
		game::dvar_t* cg_draw_material = nullptr;

		float distance_2d(float* a, float* b)
		{
			return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
		}

		float distance_3d(float* a, float* b)
		{
			return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));
		}

		constexpr auto EPSILON = std::numeric_limits<float>::epsilon();

		// Calculates the cross product of two 3D vectors
		void crossProduct3D(float v1[3], float v2[3], float result[3])
		{
			result[0] = v1[1] * v2[2] - v1[2] * v2[1];
			result[1] = v1[2] * v2[0] - v1[0] * v2[2];
			result[2] = v1[0] * v2[1] - v1[1] * v2[0];
		}

		// Normalizes a 3D vector
		void normalize3D(float v[3])
		{
			float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
			v[0] /= length;
			v[1] /= length;
			v[2] /= length;
		}

		// Calculates the normal vector of a triangle defined by three 3D points
		void calculateTriangleNormal(float p[3][3], float normal[3])
		{
			float v1[3], v2[3];
			for (int i = 0; i < 3; i++) {
				v1[i] = p[1][i] - p[0][i];
				v2[i] = p[2][i] - p[0][i];
			}
			crossProduct3D(v1, v2, normal);
			normalize3D(normal);
		}

		// Calculates the dot product of two 3D vectors
		float dotProduct3D(float v1[3], float v2[3])
		{
			return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
		}

		// Calculates the distance from a point to a plane defined by a point on the plane and the plane's normal vector
		float distancePointToPlane(float planePoint[3], float normal[3], float point[3])
		{
			float dist = 0.0f;
			for (int i = 0; i < 3; i++) {
				dist += (point[i] - planePoint[i]) * normal[i];
			}
			return dist;
		}

		// Calculates the barycentric coordinates of a point in a triangle defined by three 3D points
		void calculateBarycentricCoordinates(float p[3][3], float point[3], float& alpha, float& beta, float& gamma)
		{
			float v0[3], v1[3], v2[3];
			for (int i = 0; i < 3; i++) {
				v0[i] = p[2][i] - p[0][i];
				v1[i] = p[1][i] - p[0][i];
				v2[i] = point[i] - p[0][i];
			}
			float dot00 = dotProduct3D(v0, v0);
			float dot01 = dotProduct3D(v0, v1);
			float dot02 = dotProduct3D(v0, v2);
			float dot11 = dotProduct3D(v1, v1);
			float dot12 = dotProduct3D(v1, v2);
			float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
			beta = (dot11 * dot02 - dot01 * dot12) * invDenom;
			gamma = (dot00 * dot12 - dot01 * dot02) * invDenom;
			alpha = 1.0f - beta - gamma;
		}

		bool lineTriangleIntersection(float p[3][3], float linePoint[3], float lineDir[3])
		{
			// Calculate the normal vector of the triangle
			float normal[3];
			calculateTriangleNormal(p, normal);

			// Calculate the dot product of the normal vector and the line direction vector
			float dotProduct = dotProduct3D(normal, lineDir);

			// If the dot product is close to zero, the line is parallel to the triangle and does not intersect
			if (fabs(dotProduct) < EPSILON) {
				return false;
			}

			// Calculate the distance from the line point to the plane of the triangle
			float distance = distancePointToPlane(p[0], normal, linePoint);

			// If the distance is zero or the sign of the distance is different than the sign of the dot product, the line does not intersect the triangle
			if (fabs(distance) < EPSILON || (distance > 0 && dotProduct > 0) || (distance < 0 && dotProduct < 0)) {
				return false;
			}

			// Calculate the intersection point of the line and the plane of the triangle
			float t = -distance / dotProduct;
			float intersection[3];
			intersection[0] = linePoint[0] + t * lineDir[0];
			intersection[1] = linePoint[1] + t * lineDir[1];
			intersection[2] = linePoint[2] + t * lineDir[2];

			// Calculate the barycentric coordinates of the intersection point
			float alpha, beta, gamma;
			calculateBarycentricCoordinates(p, intersection, alpha, beta, gamma);

			// If the barycentric coordinates are all greater than or equal to zero, the intersection point is inside the triangle and the line intersects the triangle
			if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) {
				return true;
			}

			// Otherwise, the line does not intersect the triangle
			return false;
		}

		void getCenterPoint(float p[3][3], float center[3]) {
			for (int i = 0; i < 3; i++) {
				center[i] = (p[0][i] + p[1][i] + p[2][i]) / 3.0f;
			}
		}

		int frames_passed = 0; // 20 then resets to 0
		std::optional<std::string> current_material;

		void render_draw_material()
		{
			static const auto* sv_running = game::Dvar_FindVar("sv_running");
			if (!sv_running || !sv_running->current.enabled)
			{
				return;
			}

			if (!cg_draw_material || !cg_draw_material->current.integer)
			{
				return;
			}

			static const auto* cg_draw2d = game::Dvar_FindVar("cg_draw2D");
			if (cg_draw2d && !cg_draw2d->current.enabled)
			{
				return;
			}

			static const auto* gfx_map = *reinterpret_cast<game::GfxWorld**>(0xE973AE0_b);
			if (gfx_map == nullptr)
			{
				return;
			}

			const auto placement = game::ScrPlace_GetViewPlacement();
			static const float text_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

			game::rectDef_s rect{};
			rect.x = 0.f;
			rect.y = 0.f;
			rect.w = 500.f;
			rect.horzAlign = 1;
			rect.vertAlign = 0;

			game::rectDef_s text_rect{};

			static const auto font = game::R_RegisterFont("fonts/fira_mono_regular.ttf", 22);
			if (!font)
			{
				return;
			}

			/*
			if (frames_passed > 5)
			{
				frames_passed = 0;
				current_material = {};
			}
			else
			{
				frames_passed = frames_passed + 1;
			}

			if (current_material.has_value())
			{
				game::UI_DrawWrappedText(placement, current_material.value().data(), &rect, font,
					8.0, 240.0f, 0.2f, text_color, 0, 0, &text_rect, 0);
				return;
			}
			*/

			game::vec3_t origin{};
			const auto client = game::mp::g_entities[0].client;
			const auto angles = client->ps.delta_angles;
			utils::hook::invoke<void>(0x4057F0_b, client, origin); // G_GetPlayerViewOrigin

			game::vec3_t forward{};
			utils::hook::invoke<void>(0x59C600_b, angles, forward, nullptr, nullptr); // AngleVectors

			float min_distance = -1.f;
			float second_min_distance = -1.f;
			float third_min_distance = -1.f;
			game::vec3_t target_center{}, target_triangle[3]{};
			game::vec3_t secondary_target_center{}, secondary_target_triangle[3]{};
			game::vec3_t third_target_center{}, third_target_triangle[3]{};
			game::GfxSurface* target_surface = nullptr;
			game::GfxSurface* secondary_surface = nullptr;
			game::GfxSurface* third_surface = nullptr;

			for (auto i = 0u; i < gfx_map->surfaceCount; i++) {
				const auto surface = &gfx_map->dpvs.surfaces[i];
				const auto indices = &gfx_map->draw.indices[surface->tris.baseIndex];
				bool too_far = false;

				for (auto o = 0; o < surface->tris.triCount && !too_far; o++) {
					game::vec3_t triangle[3]{};

					for (auto j = 0; j < 3; j++) {
						const auto index = indices[o * 3 + j] + surface->tris.firstVertex;
						const auto vertex = &gfx_map->draw.vd.vertices[index];

						if (distance_2d(vertex->xyz, origin) > 1000.f) {
							too_far = true;
							break;
						}

						std::memcpy(&triangle[j], vertex->xyz, sizeof(float[3]));
					}

					if (!too_far && lineTriangleIntersection(triangle, origin, forward)) {
						game::vec3_t center{};
						getCenterPoint(triangle, center);
						const auto dist = distance_3d(center, origin);

						if (dist < min_distance || min_distance == -1.f) {
							// Shift second to third
							third_surface = secondary_surface;
							third_min_distance = second_min_distance;
							std::memcpy(&third_target_triangle, &secondary_target_triangle, sizeof(third_target_triangle));
							std::memcpy(&third_target_center, &secondary_target_center, sizeof(game::vec3_t));

							// Shift closest to second
							secondary_surface = target_surface;
							second_min_distance = min_distance;
							std::memcpy(&secondary_target_triangle, &target_triangle, sizeof(secondary_target_triangle));
							std::memcpy(&secondary_target_center, &target_center, sizeof(game::vec3_t));

							// Update closest surface
							target_surface = surface;
							min_distance = dist;
							std::memcpy(&target_triangle, &triangle, sizeof(target_triangle));
							std::memcpy(&target_center, &center, sizeof(game::vec3_t));
						}
						else if (dist < second_min_distance || second_min_distance == -1.f) {
							// Shift second to third
							third_surface = secondary_surface;
							third_min_distance = second_min_distance;
							std::memcpy(&third_target_triangle, &secondary_target_triangle, sizeof(third_target_triangle));
							std::memcpy(&third_target_center, &secondary_target_center, sizeof(game::vec3_t));

							// Update second closest surface
							secondary_surface = surface;
							second_min_distance = dist;
							std::memcpy(&secondary_target_triangle, &triangle, sizeof(secondary_target_triangle));
							std::memcpy(&secondary_target_center, &center, sizeof(game::vec3_t));
						}
						else if (dist < third_min_distance || third_min_distance == -1.f) {
							// Update third closest surface
							third_surface = surface;
							third_min_distance = dist;
							std::memcpy(&third_target_triangle, &triangle, sizeof(third_target_triangle));
							std::memcpy(&third_target_center, &center, sizeof(game::vec3_t));
						}
					}
				}
			}

			if (min_distance == -1.f) {
				return;
			}

			// Surface materials info
			const char* text = nullptr;

			auto format_material_info = [](const game::GfxSurface* surface) {
				if (!surface || !surface->material || !surface->material->name) return "";
				auto techniqueset_name = surface->material->techniqueSet && surface->material->techniqueSet->name ?
					utils::string::va("^3%s^7", surface->material->techniqueSet->name) : "^1null^7";
				return utils::string::va("%s (%s)", surface->material->name, techniqueset_name);
			};

			// Build the material display string
			std::string material1 = format_material_info(target_surface);
			std::string material2 = format_material_info(secondary_surface);
			std::string material3 = format_material_info(third_surface);

			if (!material1.empty() && !material2.empty() && !material3.empty()) {
				text = utils::string::va("%s\n%s\n%s", material1.c_str(), material2.c_str(), material3.c_str());
			}
			else if (!material1.empty() && !material2.empty()) {
				text = utils::string::va("%s\n%s", material1.c_str(), material2.c_str());
			}
			else if (!material1.empty()) {
				text = material1.c_str();
			}

			current_material = text;

			game::UI_DrawWrappedText(placement, text, &rect, font,
				8.0, 240.0f, 0.2f, text_color, 6, 0, &text_rect, 0);
		}

		utils::hook::detour cg_draw2d_hook;
		void cg_draw2d_stub(int local_client_num)
		{
			cg_draw2d_hook.invoke<void>(local_client_num);

			if (game::CL_IsCgameInitialized() && !game::VirtualLobby_Loaded())
			{
				render_draw_material();
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// change minimum cap to -2000 instead of -1000 (culling issue)
			dvars::override::register_float("r_lodBiasRigid", 0, -2000, 0, game::DVAR_FLAG_SAVED);

			cg_draw_material = dvars::register_bool("cg_drawMaterial", false, game::DVAR_FLAG_NONE, "Draws material name on screen");
			cg_draw2d_hook.create(0xF57D0_b, cg_draw2d_stub);
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(experimental::component)
#endif
