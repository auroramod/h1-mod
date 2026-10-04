#pragma once
#include "game/game.hpp"
#include <utils/hook.hpp>

namespace fastfiles
{
	bool exists(const std::string& zone, bool ignore_usermap = false);
	std::string get_current_fastfile();
	std::string get_load_state();
	void enum_assets(game::XAssetType type, const std::function<void(game::XAssetHeader)>& callback, bool include_override);
	void enum_asset_entries(game::XAssetType type, const std::function<void(game::XAssetEntry*)>& callback, bool include_override);
	void close_fastfile_handles();
	std::string get_zone_name(unsigned int index);

	void set_usermap(const std::string& usermap);
	void clear_usermap();
	bool usermap_exists(const std::string& name);
	bool is_stock_map(const std::string& name);
}
