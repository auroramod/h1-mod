#pragma once

#include <string>
#include <optional>

namespace utils::flags
{
	bool has_flag(const std::string& flag);
	std::optional<std::string> get_flag(const std::string& flag);
	std::optional<std::string> get_flag(const std::string& flag, const std::string& shortname);
	std::string get_flag(const std::string& flag, const std::string& shortname,
		const std::string& default_);
}
