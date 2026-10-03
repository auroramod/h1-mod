#pragma once
#include "loader/component_loader.hpp"

#include "autocomplete.hpp"
#include "game/dvars.hpp"

class game_console final : public component_interface
{
public:
	void post_unpack() override;

	static void print_internal(const char* fmt, ...);
	static void print(const int type, const std::string& data);
	static bool console_char_event(int local_client_num, int key);
	static bool console_key_event(int local_client_num, int key, int down);

private:
	static void clear();
	static void clear_output();
	static void set_buffer(const std::string& text);
	static void execute(const std::string& input);
	static void print_internal(const std::string& data);
	static void toggle_console();
	static void toggle_console_output();
	static void check_resize();
	static void draw_text(const char* text, game::Font_s* font, float x, float y, const float* color);
	static void draw_box(const float x, const float y, const float w, const float h, float* color);
	static void draw_input_box(const int lines, float* color);
	static void draw_input_text_and_over(const char* str, float* color);
	static float draw_hint_box(const int lines, float* color, float offset_x = 0.0f, float offset_y = 0.0f);
	static void draw_hint_text(const int line, const char* text, float* color, const float offset_x = 0.0f, const float offset_y = 0.0f);
	static void draw_hint_highlight(const int line, const float offset_y = 0.0f);
	static float* get_match_color(const autocomplete::match& match);
	static float draw_dvar_details(const std::string& name, game::dvar_t* dvar);
	static void draw_match_list(const autocomplete::result& result, const float offset_y = 0.0f);
	static void draw_command_hints(const autocomplete::result& result);
	static void draw_argument_hints(const autocomplete::result& result);
	static void draw_input();
	static void draw_output_scrollbar(const float x, float y, const float width, const float height, std::deque<std::string>& output);
	static void draw_output_text(const float x, float y, std::deque<std::string>& output);
	static void draw_output_window();
	static void draw_console();
	static bool handle_key_event(int local_client_num, int key, int down);
};
