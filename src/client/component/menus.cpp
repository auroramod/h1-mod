#include <std_include.hpp>
#include "menus.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

static std::string script_main_menu;

static constexpr auto patch_menu_list_name = "ui_mp/patch_code.txt";

void menus::post_unpack()
{
	if (!game::environment::is_mp())
	{
		return;
	}

	// add back legacy menu precache
	utils::hook::call(0x1402218A5, cg_set_config_values_stub);
	utils::hook::call(0x1402458E2, cg_set_config_values_stub);

	// add legacy menu mouse fix
	utils::hook::call(0x14050EB85, ui_mouse_fix);

	// add script main menu
	utils::hook::call(0x14024BA34, lui_toggle_menu_stub); // (CL_ExecuteKey)
	utils::hook::call(0x1404D15CE, lui_toggle_menu_stub); // (UI_SetActiveMenu)

	command::add("openmenu", [](const command::params& params)
	{
		if (params.size() != 2)
		{
			console::info("usage: openmenu <name>\n");
			return;
		}

		*game::keyCatchers = *game::keyCatchers & 1 | 0x10;
		game::Menus_OpenByName(0, params.get(1));
	});

	utils::hook::call(0x1404CCCF4, ui_add_menu_list_stub);
}

void menus::set_script_main_menu(const std::string& menu)
{
	script_main_menu = menu;
}

bool menus::keys_bypass_menu()
{
	const auto* cl_bypass_mouse_input = game::Dvar_FindVar("cl_bypassMouseInput");
	return (cl_bypass_mouse_input && cl_bypass_mouse_input->current.enabled);
}

game::XAssetHeader menus::load_script_menu_internal(const char* menu)
{
	const char* menu_file = utils::string::va("ui_mp/scriptmenus/%s.menu", menu);
	return game::DB_FindXAssetHeader(game::ASSET_TYPE_MENULIST, menu_file, 1);
}

bool menus::load_script_menu(int client_num, const char* menu)
{
	if (const auto asset = load_script_menu_internal(menu); asset.data != nullptr)
	{
		game::UI_AddMenuList(game::ui_info_array, asset.data, 1);
		return true;
	}

	return false;
}

void menus::precache_script_menu(int client_num, int config_string_index)
{
	if (const char* menu = game::CL_GetConfigString(config_string_index))
	{
		if (!load_script_menu(client_num, menu))
		{
			game::Com_Error(game::ERR_DROP, "Could not load script menu file %s", menu);
		}
	}
}

void menus::cg_set_config_values_stub(int client_num)
{
	utils::hook::invoke<void>(0x140236300, client_num);

	auto nesting = game::R_PopRemoteScreenUpdate();
	for (auto i = 3432; i < (3432 + 50); i++)
	{
		precache_script_menu(client_num, i);
	}

	game::R_PushRemoteScreenUpdate(nesting);
}

void menus::ui_mouse_event(int client_num, int x, int y)
{
	const auto scr_place = game::ScrPlace_GetViewPlacement();

	const auto v_x = x / (game::ScrPlace_HiResGetScaleX() * scr_place->scaleVirtualToFull[0]);
	const auto v_y = y / (game::ScrPlace_HiResGetScaleY() * scr_place->scaleVirtualToFull[1]);

	game::ui_info_array->cursor_x = v_x;
	game::ui_info_array->cursor_y = v_y;

	const auto cursor_visible = v_x >= 0.0 && v_x <= 640.0 && v_y >= 0.0 && v_y <= 480.0;
	if (!cursor_visible)
	{
		return;
	}

	const auto menu_count = *reinterpret_cast<int*>(0x14CF1F658);
	if (menu_count > 0)
	{
		game::ui_info_array->cursor_x = v_x;
		game::ui_info_array->cursor_y = v_y;
		game::ui_info_array->cursor_time = game::Sys_Milliseconds() + 200;
		game::ui_info_array->ingame_cursor_visible = cursor_visible;
		game::Display_MouseMove(game::ui_info_array);
	}
}

int menus::ui_mouse_fix(int cx_, int cy_, int dx_, int dy_)
{
	if ((*game::keyCatchers & 0x10) != 0 && !keys_bypass_menu())
	{
		tagPOINT cursor{};

		game::CL_ShowSystemCursor(0);
		game::CL_GetCursorPos(&cursor);

		ui_mouse_event(0, cursor.x, cursor.y);
		return 0;
	}

	return utils::hook::invoke<int>(0x14024CD10, cx_, cy_, dx_, dy_);
}

bool menus::open_script_main_menu()
{
	if (!script_main_menu.empty())
	{
		void* menu = game::Menus_FindByName(game::ui_info_array, script_main_menu.data());
		if (menu)
		{
			game::Menus_Open(game::ui_info_array, menu, 0);
			return true;
		}
	}

	return false;
}

void menus::lui_toggle_menu_stub(int controller_index, void* context)
{
	if (!game::VirtualLobby_Loaded())
	{
		if (!script_main_menu.empty())
		{
			if (game::Menu_IsMenuOpenAndVisible(0, script_main_menu.data()))
			{
				game::UI_SetActiveMenu(0, 0);
				return;
			}
			else if (open_script_main_menu())
			{
				*game::keyCatchers = *game::keyCatchers & 1 | 0x10;
				return;
			}
		}
	}

	// LUI_ToggleMenu
	return utils::hook::invoke<void>(0x14016CD30, controller_index, context);
}

void menus::ui_add_menu_list_stub(void* context, void* menu_list, int a3)
{
	game::UI_AddMenuList(context, menu_list, a3);

	if (game::DB_XAssetExists(game::ASSET_TYPE_MENULIST, patch_menu_list_name) &&
		!game::DB_IsXAssetDefault(game::ASSET_TYPE_MENULIST, patch_menu_list_name))
	{
		const auto patch_code_list = game::UI_LoadMenus(patch_menu_list_name);
		game::UI_AddMenuList(context, patch_code_list, a3);
	}
}

REGISTER_COMPONENT(menus)
