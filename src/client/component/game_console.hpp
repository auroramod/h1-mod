#pragma once
#include "game/dvars.hpp"

namespace game_console
{
	bool console_char_event(int local_client_num, int key);
	bool console_key_event(int local_client_num, int key, int down);

	bool match_compare(const std::string& input, const std::string& text, const bool exact);
	void find_matches(std::string input, std::vector<dvars::dvar_info>& suggestions, const bool exact);
}
