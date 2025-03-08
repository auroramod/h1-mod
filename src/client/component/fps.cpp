#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"
#include "fps.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace fps
{
	namespace
	{
		utils::hook::detour sub_5D6810_hook;
		utils::hook::detour com_frame_hook;
		utils::hook::detour r_wait_end_time_hook;

		game::dvar_t* cg_drawfps = nullptr;
		game::dvar_t* cg_drawping = nullptr;
		game::dvar_t* com_wait_end_frame_mode = nullptr;

		float fps_color_good[4] = {0.6f, 1.0f, 0.0f, 1.0f};
		float fps_color_ok[4] = {1.0f, 0.7f, 0.3f, 1.0f};
		float fps_color_bad[4] = {1.0f, 0.3f, 0.3f, 1.0f};
		float ping_color[4] = {1.0f, 1.0f, 1.0f, 0.65f};

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

		cg_perf_data cg_perf{};

		void perf_calc_fps(cg_perf_data* data, const std::int32_t value)
		{
			data->history[data->index % 32] = value;
			data->instant = value;
			data->min = 0x7FFFFFFF;
			data->max = 0;
			data->average = 0.0f;
			data->variance = 0.0f;
			data->total = 0;

			for (auto i = 0; i < data->count; ++i)
			{
				const std::int32_t idx = (data->index - i) % 32;

				if (idx < 0)
				{
					break;
				}

				data->total += data->history[idx];

				if (data->min > data->history[idx])
				{
					data->min = data->history[idx];
				}

				if (data->max < data->history[idx])
				{
					data->max = data->history[idx];
				}
			}

			data->average = static_cast<float>(data->total) / static_cast<float>(data->count);
			++data->index;
		}

		void perf_update()
		{
			cg_perf.count = 32;

			cg_perf.current_ms = static_cast<std::int32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now() - cg_perf.perf_start).count());
			cg_perf.frame_ms = cg_perf.current_ms - cg_perf.previous_ms;
			cg_perf.previous_ms = cg_perf.current_ms;

			perf_calc_fps(&cg_perf, cg_perf.frame_ms);
		}

		void cg_draw_fps()
		{
			if (cg_drawfps->current.integer > 0)
			{
				const auto fps = fps::get_fps();

				const auto font = game::R_RegisterFont("fonts/fira_mono_regular.ttf", 25);
				if (font)
				{
					const auto fps_string = utils::string::va("%i", fps);

					const auto x = (game::ScrPlace_GetViewPlacement()->realViewportSize[0] - 15.0f) - 
						game::R_TextWidth(fps_string, 0x7FFFFFFF, font);
					const auto y = font->pixelHeight + 10.f;

					const auto fps_color = fps >= 60 ? fps_color_good : (fps >= 30 ? fps_color_ok : fps_color_bad);
					game::R_AddCmdDrawText(fps_string, 0x7FFFFFFF, font, x, y, 1.f, 1.f, 0.0f, fps_color, 6);
				}
			}
		}

		void cg_draw_ping()
		{
			if (cg_drawping->current.integer > 0 && game::CL_IsCgameInitialized() && !game::VirtualLobby_Loaded() && *game::mp::client_state)
			{
				const auto font = game::R_RegisterFont("fonts/consolefont", 20);
				const auto ping_string = utils::string::va("Ping: %i", (*game::mp::client_state)->ping);

				const auto x = (game::ScrPlace_GetViewPlacement()->realViewportSize[0] - 375.0f) - game::R_TextWidth(
					ping_string, 0x7FFFFFFF, font);

				const auto y = font->pixelHeight + 15.f;
				game::R_AddCmdDrawText(ping_string, 0x7FFFFFFF, font, x, y, 1.f, 1.f, 0.0f, ping_color, 6);
			}
		}

		void sub_5D6810_stub()
		{
			perf_update();
			sub_5D6810_hook.invoke<void>();
		}

		bool r_wait_end_frame_stub()
		{
			if (com_wait_end_frame_mode->current.integer > 0)
			{
				return true;
			}

			return r_wait_end_time_hook.invoke<bool>();
		}

		void com_frame_stub()
		{
			const auto value = com_wait_end_frame_mode->current.integer;
			if (value == 0)
			{
				return com_frame_hook.invoke<void>();
			}

			const auto start = std::chrono::high_resolution_clock::now();
			com_frame_hook.invoke<void>();

			auto max_fps = 0;
			static const auto com_max_fps = game::Dvar_FindVar("com_maxfps");

			if (game::environment::is_mp())
			{
				max_fps = utils::hook::invoke<int>(0x183490_b, com_max_fps);

			}
			else
			{
				max_fps = com_max_fps->current.integer;
			}

			if (max_fps == 0)
			{
				max_fps = 1000;
			}


			constexpr auto nano_secs = std::chrono::duration_cast<std::chrono::nanoseconds>(1s);
			const auto frame_time = nano_secs / max_fps;

			if (value == 1)
			{
				const auto diff = (std::chrono::high_resolution_clock::now() - start);
				if (diff > frame_time)
				{
					return;
				}

				const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(frame_time - diff);
				std::this_thread::sleep_for(ms);
			}
			else if (value == 2)
			{
				while (std::chrono::high_resolution_clock::now() - start < frame_time)
				{
					std::this_thread::sleep_for(0ms);
				}
			}
		}
	}

	int get_fps()
	{
		return static_cast<std::int32_t>(static_cast<float>(1000.0f / static_cast<float>(cg_perf.
			average))
			+ 9.313225746154785e-10);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			// fps setup
			cg_perf.perf_start = std::chrono::high_resolution_clock::now();

			if (game::environment::is_mp())
			{
				utils::hook::jump(SELECT_VALUE(0, 0x343847_b), utils::hook::assemble([](utils::hook::assembler& a)
				{
					a.pushad64();
					a.call_aligned(perf_update);
					a.popad64();

					a.call(0x702250_b);
					a.mov(edx, 3);
					a.xor_(ecx, ecx);
					a.jmp(0x343853_b);
				}), true);

				// Don't register cg_drawfps
				utils::hook::nop(0x31D74F_b, 0x1C);
				utils::hook::nop(0x31D76F_b, 0x7);
			}
			else
			{
				sub_5D6810_hook.create(0x5D6810_b, sub_5D6810_stub);

				// Don't register cg_drawfps
				utils::hook::nop(0x15C97D_b, 0x20);
				utils::hook::nop(0x15C9A1_b, 0x7);
			}

			scheduler::loop(cg_draw_fps, scheduler::pipeline::renderer);

			cg_drawfps = dvars::register_int("cg_drawFps", 0, 0, 2, game::DVAR_FLAG_SAVED, "Draw frames per second");

			if (game::environment::is_mp())
			{
				// fix ping value
				utils::hook::nop(0x342C6C_b, 2);

				cg_drawping = dvars::register_int("cg_drawPing", 0, 0, 1, game::DVAR_FLAG_SAVED, "Choose to draw ping");

				scheduler::loop(cg_draw_ping, scheduler::pipeline::renderer);
			}

			dvars::register_bool("cg_infobar_fps", false, game::DVAR_FLAG_SAVED, "Show server latency");
			dvars::register_bool("cg_infobar_ping", false, game::DVAR_FLAG_SAVED, "Show FPS counter");

			// Make fps capping accurate
			com_wait_end_frame_mode = dvars::register_int("com_waitEndFrameMode", 0, 0, 2, game::DVAR_FLAG_SAVED, "Wait end frame mode (0 = default, 1 = sleep(n), 2 = loop sleep(0)");
			r_wait_end_time_hook.create(SELECT_VALUE(0x3A7330_b, 0x1C2420_b), r_wait_end_frame_stub);
			com_frame_hook.create(SELECT_VALUE(0x385210_b, 0x15A960_b), com_frame_stub);
		}
	};
}

REGISTER_COMPONENT(fps::component)
