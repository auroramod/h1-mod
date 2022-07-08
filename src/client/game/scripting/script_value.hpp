#pragma once
#include "game/game.hpp"
#include "variable_value.hpp"
#include "vector.hpp"

#include <utils/string.hpp>

namespace scripting
{
	class entity;
	class array;

	class script_value
	{
	public:
		script_value() = default;
		script_value(const game::VariableValue& value);

		script_value(int value);
		script_value(unsigned int value);
		script_value(bool value);

		script_value(float value);
		script_value(double value);

		script_value(const char* value);
		script_value(const std::string& value);

		script_value(const entity& value);
		script_value(const array& value);

		script_value(const vector& value);

		template <typename T>
		bool is() const;

		// was gonna do this but no clue if this is the same on H1 so just return string (https://github.com/fedddddd/t6-gsc-utils/blob/main/src/game/scripting/script_value.hpp#L18)
		std::string type_name() const
		{
			return utils::string::va("%s", this->get_raw().type);
		}

		std::string to_string() const;

		template <typename T>
		T as() const
		{
			if (!this->is<T>())
			{
				throw std::runtime_error("Invalid type");
			}

			return get<T>();
		}

		const game::VariableValue& get_raw() const;

		variable_value value_{};

	private:
		template <typename T>
		T get() const;

	};
}
