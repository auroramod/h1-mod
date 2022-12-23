#pragma once

namespace mods
{
	void set_mod(const std::string& path);
	void clear_mod();
	std::optional<std::string> get_mod();
}