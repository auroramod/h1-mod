#pragma once

#include "game/scripting/lua/value_conversion.hpp"
#include "game/scripting/array.hpp"
#include "game/scripting/execution.hpp"

namespace http
{
	unsigned int http_get(const std::string& url);
	unsigned int http_request(const std::string& url, const sol::variadic_args& va);
}
