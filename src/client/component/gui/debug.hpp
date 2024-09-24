#ifdef DEBUG
#pragma once

namespace gui::debug
{
	size_t add_debug_line(const float* start, const float* end, const float* color);
	void remove_debug_line(const size_t line);
	void set_debug_line_color(size_t line, const float* color);

	size_t add_debug_square(const float* origin, const float* color);
	void remove_debug_square(const size_t line);
	void set_debug_square_color(size_t square, const float* color);

	void reset_debug_items();
}
#endif
