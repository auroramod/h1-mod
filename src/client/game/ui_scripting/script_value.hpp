#pragma once
#include "game/game.hpp"

namespace ui_scripting
{
	class lightuserdata;
	class userdata;
	class table;
	class function;

	class script_value
	{
	public:
		script_value() = default;
		script_value(const game::hks::HksObject& value);

		script_value(int value);
		script_value(unsigned int value);
		script_value(bool value);

		script_value(float value);
		script_value(double value);

		script_value(const char* value);
		script_value(const std::string& value);

		script_value(const lightuserdata& value);
		script_value(const userdata& value);
		script_value(const table& value);
		script_value(const function& value);

		template <typename T>
		bool is() const;

		template <typename T>
		T as() const
		{
			if (!this->is<T>())
			{
				throw std::runtime_error("Invalid type");
			}

			return get<T>();
		}

		const game::hks::HksObject& get_raw() const;

	private:
		template <typename T>
		T get() const;

		game::hks::HksObject value_{};
	};

	using arguments = std::vector<script_value>;
}
