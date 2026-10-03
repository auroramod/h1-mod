#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

class fastfiles final : public component_interface
{
public:
	struct buffer_info
	{
		void* ptr;
		size_t size;
	};

	void post_unpack() override;

	static bool exists(const std::string& zone, bool ignore_usermap = false);
	static std::string get_current_fastfile();
	static std::string get_load_state();
	static void enum_assets(game::XAssetType type, const std::function<void(game::XAssetHeader)>& callback, bool include_override);
	static void enum_asset_entries(game::XAssetType type, const std::function<void(game::XAssetEntry*)>& callback, bool include_override);
	static void close_fastfile_handles();
	static std::string get_zone_name(unsigned int index);

	static void set_usermap(const std::string& usermap);
	static void clear_usermap();
	static std::optional<std::string> get_current_usermap();
	static bool usermap_exists(const std::string& name);
	static bool is_stock_map(const std::string& name);

private:
	struct stream_select_context;

	static void db_init_load_x_file_stub(game::DBFile* file, std::uint64_t offset);
	static void db_try_load_x_file_internal(const char* zone_name, int flags);
	static game::XAssetEntry* db_link_xasset_entry_stub(game::XAssetType type, game::XAssetHeader* header);
	static void dump_gsc_script(const std::string& name, game::XAssetHeader header);
	static game::XAssetHeader db_find_xasset_header_stub(game::XAssetType type, const char* name, int allow_create_default);
	static void db_read_stream_file_stub(int a1, int a2);
	static void missing_content_error_stub();
	static void* create_alias_ptr_stub(size_t load_inline, size_t skip);
	static void skip_extra_zones_stub_mp(utils::hook::assembler& a);
	static void skip_extra_zones_stub_sp(utils::hook::assembler& a);
	static bool try_load_zone(std::string name, bool localized, bool game = false);

	static HANDLE find_fastfile(const std::string& filename, bool check_loc_folder);
	static HANDLE find_usermap(const std::string& mapname);
	static HANDLE sys_create_file(game::Sys_Folder folder, const char* base_filename, bool ignore_usermap);
	static HANDLE sys_create_file_stub(game::Sys_Folder folder, const char* base_filename);
	static bool db_file_exists_stub(const char* file, int a2);

	template <typename T>
	static void merge(std::vector<T>* target, T* source, size_t length);

	static void load_pre_gfx_zones(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode);
	static void load_post_gfx_and_ui_and_common_zones(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode);
	static void load_ui_zones(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode);
	static bool is_builtin_map(const char* name);
	static void db_level_load_add_zone_stub(void* load, const char* name, unsigned int alloc_flags, size_t size_est);
	static void db_find_aipaths_stub(game::XAssetType type, const char* name, int allow_create_default);
	static int format_bsp_name(char* filename, int size, const char* mapname);
	static void get_bsp_filename_stub(char* filename, int size, const char* mapname);
	static bool image_file_decrypt_value_stub(char* value, int size, char* buffer);
	static const char* get_zone_name_internal(unsigned int index);
	static void db_unload_x_zones_stub(const unsigned short* unload_zones, unsigned int unload_count, bool create_default);
	static void db_load_xassets_vlobby_stub(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode);
	static char wait_for_vlobby_stub(const char* zone, int a2);

	static constexpr unsigned int get_asset_type_size(game::XAssetType type);
	static constexpr unsigned int get_pool_type_size(game::XAssetType type);
	static constexpr unsigned int get_pool_type_size_sp(game::XAssetType type);

	template <game::XAssetType Type, size_t Size>
	static char* reallocate_asset_pool();

	template <game::XAssetType Type, size_t Multiplier>
	static char* reallocate_asset_pool_multiplier();

	static void memset_stub(void* place, int value, size_t size);
	static void reallocate_weapon_pool();
	static void reallocate_attachment_pool();
	static void reallocate_attachment_and_weapon();
	static void reallocate_sound_pool();
	static void reallocate_material_pool();
	static void reallocate_material_bitsets();
	static std::uint32_t append_xmodel_materials(void** list, std::uint32_t count);
	static void reallocate_image_pool();
	static void reallocate_customization();
	static void widen_stream_id_access(std::uintptr_t address, std::size_t length);
	static void sort_stream_ids(std::uint32_t* begin, std::uint32_t* end, std::int64_t count, void* compare);
	static void select_stream_reads(stream_select_context* context);
	static void reallocate_asset_pools();
};
