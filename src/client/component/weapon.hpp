#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

class weapon final : public component_interface
{
public:
	void post_unpack() override;

	static void clear_modifed_enums();

private:
	static int camo_table_get_id_stub(const char* value);
	static std::uint32_t get_weapon_camo(std::uint32_t weapon);
	static void get_weapon_model_info_stub(std::uint32_t weapon, bool alt, int* variant, std::uint32_t* camo, int* emblem);
	static std::uint32_t weapon_to_stream_key_stub(std::uint32_t weapon);
	static std::uint32_t stream_key_to_weapon_stub(std::uint32_t key);
	static char* append_camo_name(char* dest, std::uint32_t weapon, char separator);
	static void patch_camo_call(std::uintptr_t address, std::size_t size, const std::function<void(utils::hook::assembler&)>& body);
	static void g_setup_level_weapon_def_stub();
	static int xmodel_get_bone_index_stub(game::XModel* model, game::scr_string_t name, unsigned int offset, char* index);
	static void cw_mismatch_error_stub(int, const char* msg, ...);
	static int g_find_config_string_index_stub(const char* string, int start, int max, int create, const char* errormsg);
	template <typename T> static void set_weapon_field(const std::string& weapon_name, unsigned int field, T value);
	static void set_weapon_field_float(const std::string& weapon_name, unsigned int field, float value);
	static void set_weapon_field_int(const std::string& weapon_name, unsigned int field, int value);
	static void set_weapon_field_bool(const std::string& weapon_name, unsigned int field, bool value);
	static int compare_hash(const void* a, const void* b);
	static std::vector<const char*> get_stringtable_entries(const std::string& name);
	static void add_entries_to_enum(game::DDLEnum* enum_, const std::vector<const char*> entries);
	static void load_ddl_asset_stub(game::DDLRoot** asset);
	static void patch_num_weapons_reg();
	static void patch_camo_bits();
};
