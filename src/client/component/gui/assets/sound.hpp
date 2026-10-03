#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_sound final : public component_interface
{
public:
	void post_unpack() override;

private:
	static bool draw_sound_window(game::snd_alias_list_t* asset);
	static void play_sound(game::snd_alias_list_t* asset);
};
#endif
