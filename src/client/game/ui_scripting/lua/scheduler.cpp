#include "std_include.hpp"
#include "context.hpp"
#include "error.hpp"

namespace ui_scripting::lua
{
	scheduler::scheduler(sol::state& state)
	{
		auto task_handle_type = state.new_usertype<task_handle>("task_handle");

		task_handle_type["clear"] = [this](const task_handle& handle)
		{
			this->remove(handle);
		};
	}

	void scheduler::run_frame()
	{
		callbacks_.access([&](task_list& tasks)
		{
			this->merge_callbacks();

			for (auto i = tasks.begin(); i != tasks.end();)
			{
				const auto now = std::chrono::high_resolution_clock::now();
				const auto diff = now - i->last_call;

				if (diff < i->delay)
				{
					++i;
					continue;
				}

				i->last_call = now;

				if (!i->is_deleted)
				{
					handle_error(i->callback());
				}

				if (i->is_volatile || i->is_deleted)
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

	void scheduler::clear()
	{
		callbacks_.access([&](task_list& tasks)
		{
			new_callbacks_.access([&](task_list& new_tasks)
			{
				new_tasks.clear();
				tasks.clear();
			});
		});
	}

	task_handle scheduler::add(const sol::protected_function& callback, const long long milliseconds,
							   const bool is_volatile)
	{
		return this->add(callback, std::chrono::milliseconds(milliseconds), is_volatile);
	}

	task_handle scheduler::add(const sol::protected_function& callback, const std::chrono::milliseconds delay,
							   const bool is_volatile)
	{
		const uint64_t id = ++this->current_task_id_;

		task task;
		task.is_volatile = is_volatile;
		task.callback = callback;
		task.delay = delay;
		task.last_call = std::chrono::steady_clock::now();
		task.id = id;
		task.is_deleted = false;

		new_callbacks_.access([&task](task_list& tasks)
		{
			tasks.emplace_back(std::move(task));
		});

		return {id};
	}

	void scheduler::remove(const task_handle& handle)
	{
		auto mask_as_deleted = [&](task_list& tasks)
		{
			for (auto& task : tasks)
			{
				if (task.id == handle.id)
				{
					task.is_deleted = true;
					break;
				}
			}
		};

		callbacks_.access(mask_as_deleted);
		new_callbacks_.access(mask_as_deleted);
	}

	void scheduler::merge_callbacks()
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
}
