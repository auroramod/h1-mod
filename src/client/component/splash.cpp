#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game_module.hpp"

#include "game/game.hpp"

#include <utils/nt.hpp>
#include <utils/hook.hpp>

namespace splash
{
	namespace
		{
		HWND window_{};

		HANDLE image_{};

		void destroy()
			{
			if (window_ && IsWindow(window_))
				{
				ShowWindow(window_, SW_HIDE);
				DestroyWindow(window_);
				UnregisterClassA("H1 Splash Screen", utils::nt::library{});
			}
		}

		void destroy_stub()
		{
			destroy();
		}

		void show()
		{
			WNDCLASSA wnd_class;

			const auto self = game_module::get_host_module();

			wnd_class.style = CS_DROPSHADOW;
			wnd_class.cbClsExtra = 0;
			wnd_class.cbWndExtra = 0;
			wnd_class.lpszMenuName = nullptr;
			wnd_class.lpfnWndProc = DefWindowProcA;
			wnd_class.hInstance = self;
			wnd_class.hIcon = LoadIconA(self, reinterpret_cast<LPCSTR>(102));
			wnd_class.hCursor = LoadCursorA(nullptr, IDC_APPSTARTING);
			wnd_class.hbrBackground = reinterpret_cast<HBRUSH>(6);
			wnd_class.lpszClassName = "H1 Splash Screen";

			if (RegisterClassA(&wnd_class))
			{
				const auto x_pixels = GetSystemMetrics(SM_CXFULLSCREEN);
				const auto y_pixels = GetSystemMetrics(SM_CYFULLSCREEN);

				if (image_)
				{
					window_ = CreateWindowExA(WS_EX_APPWINDOW, "H1 Splash Screen", "H1",
						WS_POPUP | WS_SYSMENU,
						(x_pixels - 320) / 2, (y_pixels - 100) / 2, 320, 100, nullptr,
						nullptr,
						self, nullptr);

					if (window_)
					{
						auto* const image_window = CreateWindowExA(0, "Static", nullptr, WS_CHILD | WS_VISIBLE | 0xEu,
							0, 0,
							320, 100, window_, nullptr, self, nullptr);
						if (image_window)
						{
							RECT rect;
							SendMessageA(image_window, 0x172u, 0, reinterpret_cast<LPARAM>(image_));
							GetWindowRect(image_window, &rect);

							const int width = rect.right - rect.left;
							rect.left = (x_pixels - width) / 2;

							const int height = rect.bottom - rect.top;
							rect.top = (y_pixels - height) / 2;

							rect.right = rect.left + width;
							rect.bottom = rect.top + height;
							AdjustWindowRect(&rect, WS_CHILD | WS_VISIBLE | 0xEu, 0);
							SetWindowPos(window_, nullptr, rect.left, rect.top, rect.right - rect.left,
								rect.bottom - rect.top, SWP_NOZORDER);

							ShowWindow(window_, SW_SHOW);
							UpdateWindow(window_);
						}
					}
				}
						}
					}
				}

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			const utils::nt::library self;
			image_ = LoadImageA(self, MAKEINTRESOURCE(IMAGE_SPLASH), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		}

		void post_load() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			show();
			}

		void post_unpack() override
		{
			// Disable native splash screen
			utils::hook::set<uint8_t>(SELECT_VALUE(0x140462B90, 0x140513840), 0xC3); // Sys_CreateSplashWindow
			utils::hook::jump(SELECT_VALUE(0x140462E40, 0x140513AF0), destroy_stub, true); // Sys_DestroySplashWindow
			utils::hook::jump(SELECT_VALUE(0x140462E80, 0x140513B30), destroy_stub, true); // Sys_HideSplashWindow
		}

		void pre_destroy() override
		{
			destroy();

			MSG msg;
			while (window_ && IsWindow(window_))
			{
				if (PeekMessageA(&msg, nullptr, NULL, NULL, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					std::this_thread::sleep_for(1ms);
				}
			}

			window_ = nullptr;
		}
	};
}

REGISTER_COMPONENT(splash::component)
