#pragma once
#include "autocomplete.hpp"
#include "game/dvars.hpp"

namespace game_console
{
	void print(const int type, const std::string& data);
	bool console_char_event(int local_client_num, int key);
	bool console_key_event(int local_client_num, int key, int down);
}
