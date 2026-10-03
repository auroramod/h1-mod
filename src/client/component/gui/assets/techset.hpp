#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_techset final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void draw_technique(game::MaterialTechnique* technique);
	static bool draw_techset_window(game::MaterialTechniqueSet* asset);
};
#endif
