#pragma once

namespace localized_strings
{
	void override(const std::string& key, const std::string& value, bool volatile_ = false);
	void clear();
}