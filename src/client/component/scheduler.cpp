#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/string.hpp>
#include <utils/thread.hpp>

namespace scheduler
{
	namespace
	{
		struct task
		{
			std::function<bool()> handler{};
			std::chrono::milliseconds interval{};
			std::chrono::high_resolution_clock::time_point last_call{};
		};

		using task_list = std::vector<task>;

		class task_pipeline
		{
		public:
			void add(task&& task)
			{
				new_callbacks_.access([&task](task_list& tasks)
				{
					tasks.emplace_back(std::move(task));
				});
			}

			void execute()
			{
				callbacks_.access([&](task_list& tasks)
				{
					this->merge_callbacks();

					for (auto i = tasks.begin(); i != tasks.end();)
					{
						const auto now = std::chrono::high_resolution_clock::now();
						const auto diff = now - i->last_call;

						if (diff < i->interval)
						{
							++i;
							continue;
						}

						i->last_call = now;

						const auto res = i->handler();
						if (res == cond_end)
						{
							i = tasks.erase(i);
						}
						else
						{
							++i;
						}
					}
				});
			}

		private:
			utils::concurrency::container<task_list> new_callbacks_;
			utils::concurrency::container<task_list, std::recursive_mutex> callbacks_;

			void merge_callbacks()
			{
				callbacks_.access([&](task_list& tasks)
				{
					new_callbacks_.access([&](task_list& new_tasks)
					{
							tasks.insert(tasks.end(), std::move_iterator<task_list::iterator>(new_tasks.begin()),
								std::move_iterator<task_list::iterator>(new_tasks.end()));
						new_tasks = {};
					});
				});
			}
		};

		volatile bool kill = false;
		std::thread thread;
		task_pipeline pipelines[pipeline::count];
		utils::hook::detour r_end_frame_hook;
		utils::hook::detour g_run_frame_hook;
		utils::hook::detour main_frame_hook;
		utils::hook::detour hks_frame_hook;

		void execute(const pipeline type)
		{
			assert(type >= 0 && type < pipeline::count);
			pipelines[type].execute();
		}

		void r_end_frame_stub()
		{
			execute(pipeline::renderer);
		}

		void server_frame_stub()
		{
			g_run_frame_hook.invoke<void>();
			execute(pipeline::server);
		}

		void* main_frame_stub()
		{
			const auto _0 = gsl::finally([]()
			{
				execute(pipeline::main);
			});

			return main_frame_hook.invoke<void*>();
		}

		void hks_frame_stub()
		{
			const auto state = *game::hks::lua_state;
			if (state)
			{
				execute(pipeline::lui);
			}
			hks_frame_hook.invoke<bool>();
		}

		void r_end_frame_stub_mp()
		{
			execute(pipeline::renderer);
			utils::hook::invoke<void>(0x1405FE470); // R_EndFrame
		}

		void server_frame_stub_mp()
		{
			utils::hook::invoke<void>(0x14033A640); // G_Glass_Update
			execute(pipeline::server);
		}

		void main_frame_stub_mp()
		{
			utils::hook::invoke<void>(0x1400D8310); // Com_Frame_Try_Block_Function
			execute(pipeline::main);
		}

		void hks_frame_stub_mp()
		{
			const auto state = *game::hks::lua_state;
			if (state)
			{
				execute(pipeline::lui);
			}
			utils::hook::invoke<bool>(0x1401755B0);
		}
	}

	void schedule(const std::function<bool()>& callback, const pipeline type,
	              const std::chrono::milliseconds delay)
	{
		assert(type >= 0 && type < pipeline::count);

		task task;
		task.handler = callback;
		task.interval = delay;
		task.last_call = std::chrono::high_resolution_clock::now();

		pipelines[type].add(std::move(task));
	}

	void loop(const std::function<void()>& callback, const pipeline type,
	          const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_continue;
		}, type, delay);
	}

	void once(const std::function<void()>& callback, const pipeline type,
	          const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_end;
		}, type, delay);
	}

	void on_game_initialized(const std::function<void()>& callback, const pipeline type,
	                         const std::chrono::milliseconds delay)
	{
		schedule([=]()
		{
			const auto dw_init = game::environment::is_sp() ? true : game::Live_SyncOnlineDataFlags(0) == 0;
			if (dw_init && game::Sys_IsDatabaseReady2())
			{
				once(callback, type, delay);
				return cond_end;
			}

			return cond_continue;
		}, pipeline::main);
	}

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			thread = utils::thread::create_named_thread("Async Scheduler", []()
			{
				while (!kill)
				{
					execute(pipeline::async);
					std::this_thread::sleep_for(10ms);
				}
			});
		}

		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				utils::hook::jump(0x140581FB0, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.pushad64();
				a.call_aligned(r_end_frame_stub);
				a.popad64();

				a.sub(rsp, 0x28);
					a.call(0x140581840);
					a.mov(rax, 0x15182A680);
				a.mov(rax, qword_ptr(rax));
					a.jmp(0x140581FC0);
			}), true);

				g_run_frame_hook.create(0x1402992E0, scheduler::server_frame_stub);
				main_frame_hook.create(0x1401B1DF0, scheduler::main_frame_stub);
				hks_frame_hook.create(0x1401028D0, scheduler::hks_frame_stub);
				return;
			}

			utils::hook::call(0x14025B782, r_end_frame_stub_mp); // R_EndFrame
			utils::hook::call(0x1403455E9, server_frame_stub_mp); // G_RunFrame -> G_Glass_Update
			utils::hook::call(0x1400D82C2, main_frame_stub_mp); // Com_Frame
			utils::hook::call(0x140178228, hks_frame_stub_mp);
		}

		void pre_destroy() override
		{
			kill = true;
			if (thread.joinable())
			{
				thread.join();
			}
		}
	};
}

REGISTER_COMPONENT(scheduler::component)