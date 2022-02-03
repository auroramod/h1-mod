#include <std_include.hpp>
#include "stack_isolation.hpp"

namespace scripting
{
	stack_isolation::stack_isolation()
	{
		this->in_param_count_ = game::scr_VmPub->inparamcount;
		this->out_param_count_ = game::scr_VmPub->outparamcount;
		this->top_ = game::scr_VmPub->top;
		this->max_stack_ = game::scr_VmPub->maxstack;

		game::scr_VmPub->top = this->stack_;
		game::scr_VmPub->maxstack = &this->stack_[ARRAYSIZE(this->stack_) - 1];
		game::scr_VmPub->inparamcount = 0;
		game::scr_VmPub->outparamcount = 0;
	}

	stack_isolation::~stack_isolation()
	{
		game::Scr_ClearOutParams();
		game::scr_VmPub->inparamcount = this->in_param_count_;
		game::scr_VmPub->outparamcount = this->out_param_count_;
		game::scr_VmPub->top = this->top_;
		game::scr_VmPub->maxstack = this->max_stack_;
	}
}
