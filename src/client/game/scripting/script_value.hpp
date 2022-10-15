#pragma once
#include "game/game.hpp"
#include "variable_value.hpp"
#include "vector.hpp"

#include <utils/string.hpp>

namespace scripting
{
	class entity;
	class array;
	class function;
	class value_wrap;

	class script_value
	{
	public:
		script_value() = default;
		script_value(const game::VariableValue& value);
		script_value(const value_wrap& value);

		script_value(int value);
		script_value(unsigned int value);
		script_value(bool value);

		script_value(float value);
		script_value(double value);

		script_value(const char* value);
		script_value(const std::string& value);

		script_value(const entity& value);
		script_value(const array& value);

		script_value(const function& value);

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

		template <typename T, typename I = int>
		T* as_ptr()
		{
			const auto value = this->as<I>();

			if (!value)
			{
				throw std::runtime_error("is null");
			}

			return reinterpret_cast<T*>(value);
		}

		const game::VariableValue& get_raw() const;

		variable_value value_{};

	private:
		template <typename T>
		T get() const;
	};

	class value_wrap
	{
	public:
		value_wrap(const scripting::script_value& value, int argument_index);

		std::string to_string() const
		{
			return this->value_.to_string();
		}

		std::string type_name() const
		{
			return this->value_.type_name();
		}

		template <typename T>
		T as() const
		{
			try
			{
				return this->value_.as<T>();
			}
			catch (const std::exception& e)
			{
				throw std::runtime_error(utils::string::va("parameter %d %s", this->argument_index_, e.what()));
			}
		}

		template <typename T, typename I = int>
		T* as_ptr()
		{
			try
			{
				return this->value_.as_ptr<T>();
			}
			catch (const std::exception& e)
			{
				throw std::runtime_error(utils::string::va("parameter %d %s", this->argument_index_, e.what()));
			}
		}

		template <typename T>
		T is() const
		{
			return this->value_.is<T>();
		}

		const game::VariableValue& get_raw() const
		{
			return this->value_.get_raw();
		}

		int argument_index_{};
		scripting::script_value value_;
	};
}
