#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "menus.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"

#include "utils/hook.hpp"
#include "utils/string.hpp"

namespace menus
{
	void* ui_info_array;
	std::string script_main_menu;

	bool UI_AllowScriptMenuResponse()
	{
		return *reinterpret_cast<bool*>(0x3532A7C_b);
	}

	bool UI_Started()
	{
		return *reinterpret_cast<bool*>(0x2ED2074_b);
	}

	bool UI_KeysBypassMenu()
	{
		game::dvar_t* cl_bypassMouseInput = game::Dvar_FindVar("cl_bypassMouseInput");
		if (cl_bypassMouseInput && cl_bypassMouseInput->current.enabled)
		{
			return true;
		}
		return false;
	}

	void CL_ShowSystemCursor(int a1)
	{
		return utils::hook::invoke<void>(0x5BAA60_b, a1);
	}

	void CL_GetCursorPos(tagPOINT* position)
	{
		return utils::hook::invoke<void>(0x5BA800_b, position);
	}

	int Menu_Count()
	{
		return *reinterpret_cast<int*>(0x352F9B8_b);
	}

	void* Menus_FindByName(void* dc, const char* name)
	{
		return utils::hook::invoke<void*>(0x1AC810_b, dc, name);
	}

	void Menus_Open(void* dc, void* menu, int a3)
	{
		return utils::hook::invoke<void>(0x1E1296_b, dc, menu, a3);
	}

	void Display_MouseMove(void* dc)
	{
		return utils::hook::invoke<void>(0x180B70_b, dc);
	}

	namespace
	{
		game::XAssetHeader load_script_menu_internal(const char* menu)
		{
			const char* menu_file = utils::string::va("ui_mp/scriptmenus/%s.menu", menu);
			return game::DB_FindXAssetHeader(game::ASSET_TYPE_MENUFILE, menu_file, 1);
		}

		bool load_script_menu(int client_num, const char* menu)
		{
			game::XAssetHeader asset = load_script_menu_internal(menu);
			if (asset.data)
			{
				game::UI_AddMenuList(ui_info_array, asset.data, 1);
				return true;
			}
			return false;
		}

		void cg_precache_script_menu(int client_num, int config_string_index)
		{
			const char* menu = game::CL_GetConfigString(config_string_index);
			if (menu)
			{
				if (!load_script_menu(client_num, menu))
				{
					game::Com_Error(game::ERR_DROP, "Could not load script menu file %s", menu);
				}
			}
		}

		utils::hook::detour cg_set_config_values_hook;
		void cg_set_config_values_stub(int client_num)
		{
			cg_set_config_values_hook.invoke<void>(client_num);

			auto nesting = game::R_PopRemoteScreenUpdate();
			for (auto i = 3432; i < (3432 + 50); i++)
			{
				cg_precache_script_menu(client_num, i);
			}
			game::R_PushRemoteScreenUpdate(nesting);
		}

		void ui_mouse_event(int client_num, int x, int y)
		{
			auto scrPlaceFull = game::ScrPlace_GetViewPlacement();
			auto vX = x / (game::ScrPlace_HiResGetScaleX() * scrPlaceFull->scaleVirtualToFull[0]);
			auto vY = y / (game::ScrPlace_HiResGetScaleY() * scrPlaceFull->scaleVirtualToFull[1]);
			*reinterpret_cast<float*>(0x352E590_b) = vX; // cursorX
			*reinterpret_cast<float*>(0x352E594_b) = vY; // cursorY
			int isCursorVisible = vX >= 0.0 && vX <= 640.0 && vY >= 0.0 && vY <= 480.0;
			
			if (isCursorVisible)
			{
				auto menu_count = Menu_Count();
				if (menu_count > 0)
				{
					*reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(ui_info_array) + 16) = vX; // cursor X
					*reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(ui_info_array) + 20) = vY; // cursor Y

					*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(ui_info_array) + 24) = game::Sys_Milliseconds() + 200; // cursor time until ready to move

					*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(ui_info_array) + 28) = isCursorVisible; // ingame cursor visible

					Display_MouseMove(ui_info_array);
				}
			}
		}

		int ui_mouse_fix(int cx_, int cy_, int dx_, int dy_)
		{
			if ((*game::keyCatchers & 0x10) != 0 && !UI_KeysBypassMenu())
			{
				tagPOINT cursor;

				CL_ShowSystemCursor(0);
				CL_GetCursorPos(&cursor);

				ui_mouse_event(0, cursor.x, cursor.y);
				return 0;
			}
			return utils::hook::invoke<int>(0x1384C0_b, cx_, cy_, dx_, dy_);
		}

		bool open_script_main_menu()
		{
			if (!script_main_menu.empty())
			{
				void* menu = Menus_FindByName(ui_info_array, script_main_menu.data());
				if (menu)
				{
					Menus_Open(ui_info_array, menu, 0);
					return true;
				}
			}
			return false;
		}

		void ui_set_active_menu_stub(int client_num, int idx)
		{
			if (open_script_main_menu())
			{
				*game::keyCatchers = *game::keyCatchers & 1 | 0x10;
				return;
			}
			return utils::hook::invoke<void>(0x1E4D80_b, client_num, idx); // UI_SetActiveMenu
		}
	}

	void set_script_main_menu(const std::string& menu)
	{
		script_main_menu = menu;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			ui_info_array = reinterpret_cast<void*>(0x352E580_b);

			// add back legacy menu precache
			cg_set_config_values_hook.create(0x11AC50_b, cg_set_config_values_stub);

			// add legacy menu mouse fix
			utils::hook::call(0x5BA535_b, ui_mouse_fix);

			// add script main menu (ESC)
			utils::hook::call(0x135C82_b, ui_set_active_menu_stub);

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
		}
	};
}

REGISTER_COMPONENT(menus::component)