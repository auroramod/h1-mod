#pragma once
#include <xsk/gsc/engine/h1.hpp>

namespace gsc
{
	extern std::unique_ptr<xsk::gsc::h1::context> gsc_ctx;

	void load_main_handles();
	void load_init_handles();
	game::ScriptFile* find_script(game::XAssetType type, const char* name, int allow_create_default);
}
