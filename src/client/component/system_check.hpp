#pragma once
#include "loader/component_loader.hpp"

class system_check final : public component_interface
{
public:
	void post_load() override;

	static bool is_valid();

private:
	static std::string read_zone(const std::string& name);
	static std::string hash_zone(const std::string& name);
	static bool verify_hashes(const std::unordered_map<std::string, std::string>& zone_hashes);
	static bool is_system_valid();
	static void verify_binary_version();
};
