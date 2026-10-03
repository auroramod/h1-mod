#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class menus final : public component_interface
{
public:
	void post_unpack() override;

	static void set_script_main_menu(const std::string& menu);

private:
	static bool keys_bypass_menu();
	static game::XAssetHeader load_script_menu_internal(const char* menu);
	static bool load_script_menu(int client_num, const char* menu);
	static void precache_script_menu(int client_num, int config_string_index);
	static void cg_set_config_values_stub(int client_num);
	static void ui_mouse_event(int client_num, int x, int y);
	static int ui_mouse_fix(int cx_, int cy_, int dx_, int dy_);
	static bool open_script_main_menu();
	static void lui_toggle_menu_stub(int controller_index, void* context);
	static void ui_add_menu_list_stub(void* context, void* menu_list, int a3);
};
