#include <std_include.hpp>
#include "html_window.hpp"

window* html_window::get_window()
{
	return this;
}

html_frame* html_window::get_html_frame()
{
	return this;
}

LRESULT html_window::processor(const UINT message, const WPARAM w_param, const LPARAM l_param)
{
	if (message == WM_SIZE)
	{
		this->resize(LOWORD(l_param), HIWORD(l_param));
		return 0;
	}

	if (message == WM_CREATE)
	{
		this->initialize(*this);
		return 0;
	}

	return window::processor(message, w_param, l_param);
}
