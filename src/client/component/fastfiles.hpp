#pragma once

#include "game/game.hpp"

namespace fastfiles
{
	bool exists(const std::string& zone, bool ignore_usermap = false);

	std::string get_current_fastfile();

	void enum_assets(const game::XAssetType type,
		const std::function<void(game::XAssetHeader)>& callback, const bool includeOverride);

	void close_fastfile_handles();

	std::string get_zone_name(const unsigned int index);

	void set_usermap(const std::string& usermap);
	void clear_usermap();
	std::optional<std::string> get_current_usermap();
	bool usermap_exists(const std::string& name);
	bool is_stock_map(const std::string& name);

	void enum_asset_entries(const game::XAssetType type, const std::function<void(game::XAssetEntry*)>& callback, bool include_override);
}
