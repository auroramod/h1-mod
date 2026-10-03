#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

class fonts final : public component_interface
{
public:
	void post_unpack() override;

	static void add(const std::string& name, const std::string& data);
	static void clear();

private:
	static game::TTFDef* create_font(const std::string& name, const std::string& data);
	static void free_font(game::TTFDef* font);
	static game::TTFDef* load_font(const std::string& name);
	static game::TTFDef* try_load_font(const std::string& name);
	static game::TTFDef* db_find_xasset_header_stub(game::XAssetType type, const char* name, int create_default);
	static int font_name_compare_stub(const char* a1, const char* a2);

	static void font_init_stub();
	static game::Font_s* get_custom_font(int font);
	static game::Font_s* ui_get_font_handle_stub(void* a1, int font);
	static game::Font_s* ui_get_font_handle_stub2(void* a1, __int64 a2);
	static int get_font_handle_index(int hudelem_font_index, int current);
	static void get_hud_elem_info_stub(utils::hook::assembler& a);
	static void hudelem_setfont_stub(__int64 a1, __int64 a2, __int64 a3, int a4);
	static void* hudelem_getfont_stub_get_fonts();
	static void hudelem_getfont_stub(utils::hook::assembler& a);
};
