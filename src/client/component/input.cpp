#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "game_console.hpp"
#include "gui.hpp"
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
			if (ui_scripting::lui_running())
			{
				ui_scripting::notify("keypress",
				{
					{"keynum", key},
					{"key", game::Key_KeynumToString(key, 0, 1)},
				});
			}

			if (!game_console::console_char_event(local_client_num, key))
			{
				return;
			}

			cl_char_event_hook.invoke<void>(local_client_num, key);
		}

		void cl_key_event_stub(const int local_client_num, const int key, const int down)
		{
			const auto key_to_string = game::Key_KeynumToString(key, 0, 1);
			printf("key to string: %s\n", key_to_string);

			if (ui_scripting::lui_running())
			{
				ui_scripting::notify(down ? "keydown" : "keyup",
				{
					{"keynum", key},
					{"key", key_to_string},
				});
			}

			if (!game_console::console_key_event(local_client_num, key, down))
			{
				return;
			}

			if (!gui::gui_key_event(local_client_num, key, down))
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

			cl_char_event_hook.create(SELECT_VALUE(0x1AB8F0_b, 0x12C8F0_b), cl_char_event_stub);
			cl_key_event_hook.create(SELECT_VALUE(0x1ABC20_b, 0x135A70_b), cl_key_event_stub);
		}
	};
}

REGISTER_COMPONENT(input::component)
