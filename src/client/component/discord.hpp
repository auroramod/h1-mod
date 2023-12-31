#pragma once

#include "game/game.hpp"

namespace discord
{
	game::Material* get_avatar_material(const std::string& id);
	void respond(const std::string& id, int reply);
}
