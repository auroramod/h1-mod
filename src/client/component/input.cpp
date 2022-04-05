#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "game_console.hpp"
#include "game/ui_scripting/execution.hpp"

#include <utils/hook.hpp>

namespace input
{
	namespace
	{
		utils::hook::detour cl_char_event_hook;
		utils::hook::detour cl_key_event_hook;

		void cl_char_event_stub(const int local_client_num, const int key)
		{
			ui_scripting::notify("keypress",
			{
				{"keynum", key},
				{"key", game::Key_KeynumToString(key, 0, 1)},
			});

			if (!game_console::console_char_event(local_client_num, key))
			{
				return;
			}

			cl_char_event_hook.invoke<void>(local_client_num, key);
		}

		void cl_key_event_stub(const int local_client_num, const int key, const int down)
		{
			ui_scripting::notify(down ? "keydown" : "keyup", 
			{
				{"keynum", key},
				{"key", game::Key_KeynumToString(key, 0, 1)},
			});

			if (!game_console::console_key_event(local_client_num, key, down))
			{
				return;
			}

			cl_key_event_hook.invoke<void>(local_client_num, key, down);
		}
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

			cl_char_event_hook.create(SELECT_VALUE(0x1401871A0, 0x14024E810), cl_char_event_stub);
			cl_key_event_hook.create(SELECT_VALUE(0x1401874D0, 0x14024EA60), cl_key_event_stub);
		}
	};
}

REGISTER_COMPONENT(input::component)
