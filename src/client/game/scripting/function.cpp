#include <std_include.hpp>
#include "function.hpp"
#include "execution.hpp"

namespace scripting
{
	function::function(const char* pos)
		: pos_(pos)
	{
	}

	script_value function::get_raw() const
	{
		game::VariableValue value;
		value.type = game::SCRIPT_FUNCTION;
		value.u.codePosValue = this->pos_;

		return value;
	}

	const char* function::get_pos() const
	{
		return this->pos_;
	}

	script_value function::call(const entity& self, std::vector<script_value> arguments) const
	{
		return exec_ent_thread(self, this->pos_, arguments);
	}
}
