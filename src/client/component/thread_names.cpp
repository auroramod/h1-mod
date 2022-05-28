#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/thread.hpp>

namespace thread_names
{
	namespace
	{
		void set_thread_names()
		{
			static std::unordered_map<int, std::string> thread_names =
			{
				{game::THREAD_CONTEXT_MAIN, "Main"},
				{game::THREAD_CONTEXT_BACKEND, "Backend"}, // Renderer
				{game::THREAD_CONTEXT_WORKER0, "Worker0"},
				{game::THREAD_CONTEXT_WORKER1, "Worker1"},
				{game::THREAD_CONTEXT_WORKER2, "Worker2"},
				{game::THREAD_CONTEXT_WORKER3, "Worker3"},
				{game::THREAD_CONTEXT_WORKER4, "Worker4"},
				{game::THREAD_CONTEXT_WORKER5, "Worker5"},
				{game::THREAD_CONTEXT_WORKER6, "Worker6"},
				{game::THREAD_CONTEXT_WORKER7, "Worker7"},
				{game::THREAD_CONTEXT_SERVER, "Server"},
				{game::THREAD_CONTEXT_CINEMATIC, "Cinematic"},
				{game::THREAD_CONTEXT_DATABASE, "Database"},
				{game::THREAD_CONTEXT_STREAM, "Stream"},
				{game::THREAD_CONTEXT_SNDSTREAMPACKETCALLBACK, "Snd stream packet callback"},
				{game::THREAD_CONTEXT_STATS_WRITE, "Stats write"},
			};

			for (const auto& thread_name : thread_names)
			{
				const auto id = game::threadIds[thread_name.first];
				if (id)
				{
					utils::thread::set_name(id, thread_name.second);
				}
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			set_thread_names();
			scheduler::once(set_thread_names, scheduler::pipeline::main);
			scheduler::once(set_thread_names, scheduler::pipeline::renderer);
			scheduler::once(set_thread_names, scheduler::pipeline::server);
		}
	};
}

REGISTER_COMPONENT(thread_names::component)
