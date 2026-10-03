#pragma once
#include "loader/component_loader.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

class autocomplete final : public component_interface
{
public:
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

	void post_unpack() override;

	// builds suggestions for the current input (returns the cycle state when the input is the last completion)
	static result query(const std::string& input);
	// remaining characters of the best match, drawn after the cursor
	static std::string get_ghost_text(const result& result);
	// tab completion: returns the new input buffer
	static std::string complete(const std::string& input, bool reverse = false);
	// strips the leading slash and normalizes bool values (true/false -> 1/0)
	static std::string prepare_command(const std::string& input);
	// incremented whenever a cached argument list finished refreshing
	static std::uint32_t get_generation();

	static void add_history(const std::string& input);
	static std::deque<std::string> get_history();
	static void clear_history();

	struct context;
	struct cached_list;

private:

	static std::size_t find_segment_start(const std::string& input);
	static std::vector<std::string> split_words(const std::string& text);
	static std::string quote_if_needed(const std::string& value);
	static std::string dvar_value(game::dvar_t* dvar, const game::dvar_value& value);
	static std::vector<match> get_cached(cached_list& list, std::chrono::milliseconds max_age,
		scheduler::pipeline pipeline, std::vector<match>(*refresh)());
	static std::vector<match> get_dvar_names();
	static std::vector<match> get_dvar_values(game::dvar_t* dvar);
	static std::vector<match> refresh_maps();
	static std::vector<match> refresh_gametypes();
	static std::vector<match> refresh_weapons();
	static std::vector<match> get_maps();
	static std::vector<match> get_gametypes();
	static std::vector<match> get_weapons();
	static std::vector<match> get_keys();
	static std::vector<match> get_dvar_arguments(const std::string& dvar_name, game::dvar_t* dvar);
	static std::vector<match> get_arguments(const context& ctx);
	static std::vector<match> filter(std::vector<match> candidates, const std::string& token, bool keep_order);
	static std::vector<match> find_commands(const std::string& token);
	static result build_result(const std::string& input);
	static bool should_append_space(const result& result);
	static void register_providers();
};
