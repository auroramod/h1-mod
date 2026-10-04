#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class lui final : public component_interface
{
public:
	void post_unpack() override;

private:
	static bool begin_game_message_event_stub(int a1, const char* name, void* a3);
	static void cg_entity_event_stub(void* a1, void* a2, unsigned int event_type, void* a4);
	static void vl_depot_loaded_stub(game::dvar_t* dvar, bool value);
};
