#pragma once
#include "loader/component_loader.hpp"

#include "game/dvars.hpp"

class game_console final : public component_interface
{
public:
	void post_load() override;
	void post_unpack() override;

	static void print_internal(const char* fmt, ...);
	static void print(const int type, const std::string& data);
	static bool console_char_event(int local_client_num, int key);
	static bool console_key_event(int local_client_num, int key, int down);

private:
	static void clear();
	static void print_internal(const std::string& data);
	static void toggle_console();
	static void toggle_console_output();
	static void check_resize();
	static void draw_box(const float x, const float y, const float w, const float h, float* color);
	static void draw_input_box(const int lines, float* color);
	static void draw_input_text_and_over(const char* str, float* color);
	static float draw_hint_box(const int lines, float* color, [[maybe_unused]] float offset_x = 0.0f, [[maybe_unused]] float offset_y = 0.0f);
	static void draw_hint_text(const int line, const char* text, float* color, const float offset_x = 0.0f, const float offset_y = 0.0f);
	static void find_matches(std::string input, std::vector<dvars::dvar_info>& suggestions, const bool exact);
	static void draw_input();
	static void draw_output_scrollbar(const float x, float y, const float width, const float height, std::deque<std::string>& output);
	static void draw_output_text(const float x, float y, std::deque<std::string>& output);
	static void draw_output_window();
	static void draw_console();
};
