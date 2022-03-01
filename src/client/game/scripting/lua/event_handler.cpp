#include "std_include.hpp"
#include "context.hpp"
#include "error.hpp"
#include "value_conversion.hpp"

namespace scripting::lua
{
	event_handler::event_handler(sol::state& state)
		: state_(state)
	{
		auto event_listener_handle_type = state.new_usertype<event_listener_handle>("event_listener_handle");

		event_listener_handle_type["clear"] = [this](const event_listener_handle& handle)
		{
			this->remove(handle);
		};

		event_listener_handle_type["endon"] = [this](const event_listener_handle& handle, const entity& entity, const std::string& event)
		{
			this->add_endon_condition(handle, entity, event);
		};
	}

	void event_handler::dispatch(const event& event)
	{
		bool has_built_arguments = false;
		event_arguments arguments{};

		callbacks_.access([&](task_list& tasks)
		{
			this->merge_callbacks();
			this->handle_endon_conditions(event);

			for (auto i = tasks.begin(); i != tasks.end();)
			{
				if (i->event != event.name || i->entity != event.entity)
				{
					++i;
					continue;
				}

				if (!i->is_deleted)
				{
					if(!has_built_arguments)
					{
						has_built_arguments = true;
						arguments = this->build_arguments(event);
					}
					
					handle_error(i->callback(sol::as_args(arguments)));
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

	event_listener_handle event_handler::add_event_listener(event_listener&& listener)
	{
		const uint64_t id = ++this->current_listener_id_;
		listener.id = id;
		listener.is_deleted = false;

		new_callbacks_.access([&listener](task_list& tasks)
		{
			tasks.emplace_back(std::move(listener));
		});

		return {id};
	}

	void event_handler::add_endon_condition(const event_listener_handle& handle, const entity& entity,
			const std::string& event)
	{
		auto merger = [&](task_list& tasks)
		{
			for(auto& task : tasks)
			{
				if(task.id == handle.id)
				{
					task.endon_conditions.emplace_back(entity, event);
				}
			}
		};
		
		callbacks_.access([&](task_list& tasks)
		{
			merger(tasks);
			new_callbacks_.access(merger);
		});
	}

	void event_handler::clear()
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

	void event_handler::remove(const event_listener_handle& handle)
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

	void event_handler::merge_callbacks()
	{
		callbacks_.access([&](task_list& tasks)
		{
			new_callbacks_.access([&](task_list& new_tasks)
			{
				tasks.insert(tasks.end(), std::move_iterator(new_tasks.begin()),
				             std::move_iterator(new_tasks.end()));
				new_tasks = {};
			});
		});
	}

	void event_handler::handle_endon_conditions(const event& event)
	{
		auto deleter = [&](task_list& tasks)
		{
			for(auto& task : tasks)
			{
				for(auto& condition : task.endon_conditions)
				{
					if(condition.first == event.entity && condition.second == event.name)
					{
						task.is_deleted = true;
						break;
					}
				}
			}
		};
		
		callbacks_.access(deleter);
	}

	event_arguments event_handler::build_arguments(const event& event) const
	{
		event_arguments arguments;

		for (const auto& argument : event.arguments)
		{
			arguments.emplace_back(convert(this->state_, argument));
		}

		return arguments;
	}
}
