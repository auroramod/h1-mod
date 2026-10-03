#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class weapon final : public component_interface
{
public:
	void post_unpack() override;

	static void clear_modifed_enums();

private:
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
};
