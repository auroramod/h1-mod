#pragma once
#include "loader/component_loader.hpp"

class lui final : public component_interface
{
public:
	void post_unpack() override;

private:
	static bool begin_game_message_event_stub(int a1, const char* name, void* a3);
	static void cg_entity_event_stub(void* a1, void* a2, unsigned int event_type, void* a4);
};
