#pragma once

namespace callstack
{
	std::string get_summary(const CONTEXT* context, int max_frames = 32);
}
