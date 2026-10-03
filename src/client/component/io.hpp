#pragma once
#include "loader/component_loader.hpp"

class io final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void check_path(const std::filesystem::path& path);
	static std::string convert_path(const std::filesystem::path& path);
	static void replace(std::string& str, const std::string& from, const std::string& to);
};
