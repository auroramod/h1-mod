#pragma once
#include "loader/component_loader.hpp"

class filesystem final : public component_interface
{
public:
	void post_unpack() override;

	static std::string read_file(const std::string& path);
	static bool read_file(const std::string& path, std::string* data, std::string* real_path = nullptr);
	static bool find_file(const std::string& path, std::string* real_path);
	static bool exists(const std::string& path);

	static void register_path(const std::filesystem::path& path);
	static void unregister_path(const std::filesystem::path& path);

	static std::vector<std::string> get_search_paths();
	static std::vector<std::string> get_search_paths_rev();

private:
	static std::deque<std::filesystem::path>& get_search_paths_internal();
	static bool is_fallback_lang();
	static void fs_startup_stub(const char* name);
	static std::vector<std::filesystem::path> get_paths(const std::filesystem::path& path);
	static bool can_insert_path(const std::filesystem::path& path);
	static const char* sys_default_install_path_stub();
};
