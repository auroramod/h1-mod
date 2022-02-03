#pragma once
#include "../window.hpp"
#include "html_frame.hpp"

class html_window final : public window, public html_frame
{
public:
	~html_window() = default;

	window* get_window();
	html_frame* get_html_frame();

private:
	LRESULT processor(UINT message, WPARAM w_param, LPARAM l_param) override;
};
