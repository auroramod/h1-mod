#pragma once

namespace gsc
{
	void load_main_handles();
	void load_init_handles();
	game::ScriptFile* find_script(game::XAssetType type, const char* name, int allow_create_default);
}
