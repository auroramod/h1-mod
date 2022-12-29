#pragma once

namespace mods
{
	void set_mod(const std::string& path, bool change_fs_game = true);
	std::optional<std::string> get_mod();
}
