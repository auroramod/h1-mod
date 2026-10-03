#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class fps final : public component_interface
{
public:
	void post_unpack() override;

	static int get_fps();

	struct cg_perf_data
	{
		std::chrono::time_point<std::chrono::steady_clock> perf_start;
		std::int32_t current_ms{};
		std::int32_t previous_ms{};
		std::int32_t frame_ms{};
		std::int32_t history[32]{};
		std::int32_t count{};
		std::int32_t index{};
		std::int32_t instant{};
		std::int32_t total{};
		float average{};
		float variance{};
		std::int32_t min{};
		std::int32_t max{};
	};

private:
	static void perf_calc_fps(cg_perf_data* data, std::int32_t value);
	static void perf_update();
	static void cg_draw_fps();
	static void cg_draw_ping();
	static void sub_5D6810_stub();
	static bool r_wait_end_frame_stub();
	static void com_frame_stub();
	static game::dvar_t* cg_draw_fps_register_stub(const char* name, const char** value_list, int default_index, unsigned int flags, const char* description);
};
