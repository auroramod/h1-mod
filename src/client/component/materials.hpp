#pragma once

#include "game/game.hpp"

namespace materials
{
	bool setup_material_image(game::Material* material, const std::string& data);
	game::Material* create_material(const std::string& name);
	void free_material(game::Material* material);
}
