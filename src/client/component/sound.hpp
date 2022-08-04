#pragma once

namespace sound
{
	bool dump_sound(game::snd_alias_list_t* asset);
	game::XAssetHeader find_sound(const char* name);

	void clear();
}