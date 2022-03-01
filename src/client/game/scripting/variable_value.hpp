#pragma once
#include "game/game.hpp"

namespace scripting
{
	class variable_value
	{
	public:
		variable_value() = default;
		variable_value(const game::VariableValue& value);
		variable_value(const variable_value& other) noexcept;
		variable_value(variable_value&& other) noexcept;

		variable_value& operator=(const variable_value& other) noexcept;
		variable_value& operator=(variable_value&& other) noexcept;

		~variable_value();

		const game::VariableValue& get() const;

	private:
		void assign(const game::VariableValue& value);
		void release();

		game::VariableValue value_{{0}, game::SCRIPT_NONE};
	};
}
