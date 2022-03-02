#pragma once
#include <utils/concurrency.hpp>

namespace ui_scripting::lua
{
	class context;

	class task_handle
	{
	public:
		uint64_t id = 0;
	};

	class task final : public task_handle
	{
	public:
		std::chrono::steady_clock::time_point last_call{};
		sol::protected_function callback{};
		std::chrono::milliseconds delay{};
		bool is_volatile = false;
		bool is_deleted = false;
	};

	class scheduler final
	{
	public:
		scheduler(sol::state& state);

		scheduler(scheduler&&) noexcept = delete;
		scheduler& operator=(scheduler&&) noexcept = delete;

		scheduler(const scheduler&) = delete;
		scheduler& operator=(const scheduler&) = delete;

		void run_frame();
		void clear();

		task_handle add(const sol::protected_function& callback, long long milliseconds, bool is_volatile);
		task_handle add(const sol::protected_function& callback, std::chrono::milliseconds delay, bool is_volatile);

	private:
		using task_list = std::vector<task>;
		utils::concurrency::container<task_list> new_callbacks_;
		utils::concurrency::container<task_list, std::recursive_mutex> callbacks_;
		std::atomic_int64_t current_task_id_ = 0;

		void remove(const task_handle& handle);
		void merge_callbacks();
	};
}
