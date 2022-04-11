#pragma once

namespace materials
{
	void add(const std::string& name, const std::string& data);
	bool exists(const std::string& name);
	void clear();
}
