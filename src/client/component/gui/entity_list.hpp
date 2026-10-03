#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class entity_list final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void update_entity_list();
	static void render_window();
};
#endif
