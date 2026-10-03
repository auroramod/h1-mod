#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_rawfile final : public component_interface
{
public:
	void post_unpack() override;

private:
	static std::string& get_decompressed_buffer(game::RawFile* asset, bool force = false);
	static bool draw_asset(game::RawFile* asset);
};
#endif
