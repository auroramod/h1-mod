#pragma once
#include "script_value.hpp"
#include "entity.hpp"

namespace scripting
{
	struct event
	{
		std::string name;
		entity entity{};
		std::vector<script_value> arguments;
	};
}
