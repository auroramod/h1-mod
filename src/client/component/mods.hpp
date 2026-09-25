#pragma once

#include "game/structs.hpp"

namespace mods
{
	void set_mod(const std::string& path, bool change_fs_game = true);
	std::optional<std::string> get_mod();
	void read_stats();

	void load(const std::string& path);
	void unload();

	std::vector<std::string> get_mod_list();
	std::optional<nlohmann::json> get_mod_info(const std::string& mod);
	bool mod_exists(const std::string& folder);

	void execute_restart(const std::optional<game::netadr_s>& server = {});
}
