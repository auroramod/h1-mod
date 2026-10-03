#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "input.hpp"

#include "game/game.hpp"

#include "game_console.hpp"
#include "gui/gui.hpp"
#include "game/ui_scripting/execution.hpp"

#include <utils/hook.hpp>

static utils::hook::detour cl_char_event_hook;
static utils::hook::detour cl_key_event_hook;
static utils::hook::detour cl_mouse_move_hook;

void input::post_unpack()
{
	if (game::environment::is_dedi())
	{
		return;
	}

	cl_char_event_hook.create(SELECT_VALUE(0x1401AB8F0, 0x14024E810), cl_char_event_stub); // CL_CharEvent
	cl_key_event_hook.create(SELECT_VALUE(0x1401ABC20, 0x14024EA60), cl_key_event_stub); // CL_KeyEvent
#ifdef _DEBUG
	if (!game::environment::is_sp())
	{
		cl_mouse_move_hook.create(0x140177550, cl_mouse_move_stub); // CL_MouseMove
	}
#endif
}

void input::cl_char_event_stub(const int local_client_num, const int key)
{
	if (!game_console::console_char_event(local_client_num, key))
	{
		return;
	}

#ifdef _DEBUG
	if (!gui::gui_char_event(local_client_num, key))
	{
		return;
	}
#endif

	cl_char_event_hook.invoke<void>(local_client_num, key);
}

void input::cl_key_event_stub(const int local_client_num, const int key, const int down, const unsigned int time)
{
	if (!game_console::console_key_event(local_client_num, key, down))
	{
		return;
	}

#ifdef _DEBUG
	if (!gui::gui_key_event(local_client_num, key, down))
	{
		return;
	}
#endif

	cl_key_event_hook.invoke<void>(local_client_num, key, down, time);
}

void input::cl_mouse_move_stub(const int local_client_num, int x, int y)
{
#ifdef _DEBUG
	if (!gui::gui_mouse_event(local_client_num, x, y))
	{
		return;
	}
#endif

	cl_mouse_move_hook.invoke<void>(local_client_num, x, y);
}

REGISTER_COMPONENT(input)
