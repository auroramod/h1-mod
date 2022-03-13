#pragma once

namespace game_console
{
	bool console_char_event(int local_client_num, int key);
	bool console_key_event(int local_client_num, int key, int down);
}