#pragma once

#include "../event.hpp"

namespace scripting::lua::engine
{
	void start();
	void stop();
	void notify(const event& e);
	void run_frame();
}
