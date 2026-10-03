#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class debug final : public component_interface
{
public:
	void post_unpack() override;

	static size_t add_debug_line(const float* start, const float* end, const float* color);
	static void remove_debug_line(const size_t line);
	static void set_debug_line_color(size_t line, const float* color);
	static size_t add_debug_square(const float* origin, const float* color, const std::string& text, const float thickness);
	static void remove_debug_square(const size_t line);
	static void set_debug_square_color(size_t square, const float* color);
	static void reset_debug_items();

private:
	static void add_entity_draw(const std::string& name, const std::array<float, 4>& color, const int draw_type, const std::function<bool(const std::string&)>& match_func);
	static float vector_dot(const float* a, const float* b);
	static bool world_pos_to_screen_pos(const float* origin, float* out);
	static void draw_line(float* start, float* end, float* color, float thickness);
	static void draw_square(const float* origin, float width, const float* color);
	static void draw_square_from_points(const float* p1, const float* p2, const float* p3, const float* p4, const float* color, float thickness, bool mesh_only);
	static void draw_cube(float* origin, float width, float* color, float thickness, bool mesh_only);
	static float get_pi();
	static void draw_cylinder(const float* center, float radius, float height, int point_count, const float* color, float thickness, bool mesh_only);
	static void draw_rectangular_prism(const float* center, game::Bounds bounds, const float* color, float thickness, bool mesh_only);
	static void draw_window();
	static float distance_2d(float* a, float* b);
	static void get_pathnode_origin(game::pathnode_t* node, float* out);
	static void draw_node_links(game::pathnode_t* node, float* origin);
	static void begin_render_window();
	static void end_render_window();
	static void draw_nodes();
	static void draw_triggers();
	static void draw_debug_items();
	static void update_camera();
};
#endif
