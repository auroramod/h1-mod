#pragma once
#include "scheduler.hpp"
#include "game/game.hpp"

namespace autocomplete
{
	enum class match_type
	{
		command,
		dvar,
		argument,
	};

	struct match
	{
		std::string name;
		std::string description;
		match_type type{};
	};

	struct result
	{
		std::string head;
		std::string token;
		std::string command;
		std::size_t arg_index{};
		std::vector<match> matches;
		int selected = -1;
		bool show_list = true;
	};

	// builds suggestions for the current input (returns the cycle state when the input is the last completion)
	result query(const std::string& input);

	// remaining characters of the best match, drawn after the cursor
	std::string get_ghost_text(const result& result);

	// tab completion: returns the new input buffer
	std::string complete(const std::string& input, bool reverse = false);

	// strips the leading slash and normalizes bool values (true/false -> 1/0)
	std::string prepare_command(const std::string& input);

	// incremented whenever a cached argument list finished refreshing
	std::uint32_t get_generation();

	void add_history(const std::string& input);
	std::deque<std::string> get_history();
	void clear_history();
}
