#pragma once
#include "steam/interface.hpp"
#include <utils/nt.hpp>

namespace steam_proxy
{
	const utils::nt::library& get_overlay_module();
}
