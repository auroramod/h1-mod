#include <std_include.hpp>
#include "window.hpp"

#include <utils/nt.hpp>

std::mutex window::mutex_;
std::vector<window*> window::windows_;

window::window()
{
	ZeroMemory(&this->wc_, sizeof(this->wc_));

	this->classname_ = "window-base-" + std::to_string(time(nullptr));

	this->wc_.cbSize = sizeof(this->wc_);
	this->wc_.style = CS_HREDRAW | CS_VREDRAW;
	this->wc_.lpfnWndProc = static_processor;
	this->wc_.hInstance = GetModuleHandle(nullptr);
	this->wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	this->wc_.hIcon = LoadIcon(this->wc_.hInstance, MAKEINTRESOURCE(102));
	this->wc_.hIconSm = this->wc_.hIcon;
	this->wc_.hbrBackground = HBRUSH(COLOR_WINDOW);
	this->wc_.lpszClassName = this->classname_.data();
	RegisterClassEx(&this->wc_);
}

void window::create(const std::string& title, const int width, const int height, const long flags)
{
	{
		std::lock_guard _(mutex_);
		windows_.push_back(this);
	}

	const auto x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	const auto y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	this->handle_ = CreateWindowExA(NULL, this->wc_.lpszClassName, title.data(), flags, x, y, width, height, nullptr,
	                                nullptr, this->wc_.hInstance, this);

	SendMessageA(this->handle_, WM_DPICHANGED, 0, 0);
}

window::~window()
{
	this->close();
	UnregisterClass(this->wc_.lpszClassName, this->wc_.hInstance);
}

void window::close()
{
	if (!this->handle_) return;

	SendMessageA(this->handle_, WM_KILL_WINDOW, NULL, NULL);
	this->handle_ = nullptr;
}

void window::run()
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void window::close_all()
{
	std::unique_lock lock(mutex_);
	auto window_list = windows_;
	lock.unlock();

	const auto current_thread_id = GetCurrentThreadId();
	for (auto& window : window_list)
	{
		const auto thread_id = GetWindowThreadProcessId(*window, nullptr);

		if (thread_id == current_thread_id)
		{
			window->close();
		}
	}
}

void window::remove_window(const window* window)
{
	std::lock_guard _(mutex_);

	for (auto i = windows_.begin(); i != windows_.end(); ++i)
	{
		if (*i == window)
		{
			windows_.erase(i);
			break;
		}
	}
}

int window::get_window_count()
{
	std::lock_guard _(mutex_);

	auto count = 0;
	const auto current_thread_id = GetCurrentThreadId();

	for (const auto& window : windows_)
	{
		const auto thread_id = GetWindowThreadProcessId(*window, nullptr);

		if (thread_id == current_thread_id)
		{
			++count;
		}
	}

	return count;
}

void window::show() const
{
	ShowWindow(this->handle_, SW_SHOW);
	UpdateWindow(this->handle_);
}

void window::hide() const
{
	ShowWindow(this->handle_, SW_HIDE);
	UpdateWindow(this->handle_);
}

void window::set_callback(const std::function<LRESULT(window*, UINT, WPARAM, LPARAM)>& callback)
{
	this->callback_ = callback;
}

LRESULT window::processor(const UINT message, const WPARAM w_param, const LPARAM l_param)
{
	if (message == WM_DPICHANGED)
	{
		const utils::nt::library user32{"user32.dll"};
		const auto get_dpi = user32 ? user32.get_proc<UINT(WINAPI *)(HWND)>("GetDpiForWindow") : nullptr;

		if (get_dpi)
		{
			const auto dpi = get_dpi(*this);
			if (dpi != this->last_dpi_)
			{
				RECT rect;
				GetWindowRect(*this, &rect);

				const auto scale = dpi * 1.0 / this->last_dpi_;
				this->last_dpi_ = dpi;

				const auto width = rect.right - rect.left;
				const auto height = rect.bottom - rect.top;

				MoveWindow(*this, rect.left, rect.top, int(width * scale), int(height * scale), TRUE);
			}
		}
	}

	if (message == WM_DESTROY)
	{
		remove_window(this);

		if (get_window_count() == 0)
		{
			PostQuitMessage(0);
		}

		return TRUE;
	}

	if (message == WM_KILL_WINDOW)
	{
		DestroyWindow(*this);
		return 0;
	}

	if (this->callback_)
	{
		return this->callback_(this, message, w_param, l_param);
	}

	return DefWindowProc(*this, message, w_param, l_param);
}

LRESULT CALLBACK window::static_processor(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (message == WM_CREATE)
	{
		auto data = reinterpret_cast<LPCREATESTRUCT>(l_param);
		SetWindowLongPtrA(hwnd, GWLP_USERDATA, LONG_PTR(data->lpCreateParams));

		static_cast<window*>(data->lpCreateParams)->handle_ = hwnd;
	}

	const auto self = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (self) return self->processor(message, w_param, l_param);

	return DefWindowProc(hwnd, message, w_param, l_param);
}


window::operator HWND() const
{
	return this->handle_;
}
