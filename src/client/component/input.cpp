#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "game_console.hpp"
#include "gui/gui.hpp"
#include "game/ui_scripting/execution.hpp"

#include <utils/hook.hpp>

namespace input
{
	namespace
	{
		utils::hook::detour cl_char_event_hook;
		utils::hook::detour cl_key_event_hook;
		utils::hook::detour cl_mouse_move_hook;

		void cl_char_event_stub(const int local_client_num, const int key)
		{
			if (!game_console::console_char_event(local_client_num, key))
			{
				return;
			}

#ifdef DEBUG
			if (!gui::gui_char_event(local_client_num, key))
			{
				return;
			}
#endif

			cl_char_event_hook.invoke<void>(local_client_num, key);
		}

		void cl_key_event_stub(const int local_client_num, const int key, const int down)
		{
			if (!game_console::console_key_event(local_client_num, key, down))
			{
				return;
			}

#ifdef DEBUG
			if (!gui::gui_key_event(local_client_num, key, down))
			{
				return;
			}
#endif

			cl_key_event_hook.invoke<void>(local_client_num, key, down);
		}

#ifdef DEBUG
		void cl_mouse_move_stub(const int local_client_num, int x, int y)
		{
			if (!gui::gui_mouse_event(local_client_num, x, y))
			{
				return;
			}

			cl_mouse_move_hook.invoke<void>(local_client_num, x, y);
		}
#endif
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			cl_char_event_hook.create(SELECT_VALUE(0x1AB8F0_b, 0x12C8F0_b), cl_char_event_stub);
			cl_key_event_hook.create(SELECT_VALUE(0x1ABC20_b, 0x135A70_b), cl_key_event_stub);
#ifdef DEBUG
			if (game::environment::is_sp())
			{
				cl_mouse_move_hook.create(SELECT_VALUE(0x0_b, 0x27B310_b), cl_mouse_move_stub);
			}
#endif
		}
	};
}

REGISTER_COMPONENT(input::component)
