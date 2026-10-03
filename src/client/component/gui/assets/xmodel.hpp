#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

class asset_xmodel final : public component_interface
{
public:
	void post_unpack() override;

	static void spawn_xmodel(game::XModel* asset, const float scale = 1.f);

private:
	static ImVec2 project_vertex(game::vec3_t v, bool flip_axis, float scale = 1.f, bool rotate = false, float rotation_speed = 0.f);
	static int sum_verts(game::XSurface* surf, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin);
	static void draw_surf(game::XSurface* surf, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin, game::vec2_t maxs_2d, ImVec2 window_pos, bool flip_axis);
	static int sum_verts_in_xmodels(game::XModel* asset, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin);
	static void draw_xmodel(game::XModel* asset, bool flip_axis, game::vec3_t mins, game::vec3_t maxs, game::vec3_t origin, game::vec3_t maxs_2d);
	static bool draw_xmodel_window(game::XModel* asset);
	static void r_generate_sorted_draw_surfs_stub(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6);
	static void spawn_xmodel_button(game::XModel* asset);
	static void update();
	static float distance_3d(float* a, float* b);
};
#endif
