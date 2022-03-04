#pragma once
#include "game/game.hpp"

namespace scripting
{
	class stack_isolation final
	{
	public:
		stack_isolation();
		~stack_isolation();

		stack_isolation(stack_isolation&&) = delete;
		stack_isolation(const stack_isolation&) = delete;
		stack_isolation& operator=(stack_isolation&&) = delete;
		stack_isolation& operator=(const stack_isolation&) = delete;

	private:
		game::VariableValue stack_[512]{};

		game::VariableValue* max_stack_;
		game::VariableValue* top_;
		unsigned int in_param_count_;
		unsigned int out_param_count_;
	};
}
