#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <xsk/gsc/engine/h1.hpp>

class script_loading final : public component_interface
{
public:
	void post_unpack() override;
	void pre_destroy() override;

	static std::unique_ptr<xsk::gsc::h1::context> gsc_ctx;

	static void load_main_handles();
	static void load_init_handles();
	static game::ScriptFile* find_script(game::XAssetType type, const char* name, int allow_create_default);

private:
	static char* allocate_buffer(size_t size);
	static void free_script_memory();
	static void clear();
	static bool read_raw_script_file(const std::string& name, std::string* data);
	static game::ScriptFile* load_custom_script(const char* file_name, const std::string& real_name);
	static std::string get_script_file_name(const std::string& name);
	static std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>> read_compiled_script_file(const std::string& name, const std::string& real_name);
	static void load_script(const std::string& name);
	static void load_scripts(const std::filesystem::path& root_dir, const std::filesystem::path& subfolder);
	static void load_custom_scripts();
	static int db_is_x_asset_default(game::XAssetType type, const char* name);
	static void load_gametype_script_stub(void* a1, void* a2);
	static void db_get_raw_buffer_stub(const game::RawFile* rawfile, char* buf, const int size);
	static void scr_begin_load_scripts_stub();
	static void scr_end_load_scripts_stub();

};
