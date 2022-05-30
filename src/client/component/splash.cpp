#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game_module.hpp"

#include <utils/nt.hpp>
#include <utils/hook.hpp>

namespace splash
{
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

			this->show();
		}

		void post_unpack() override
		{
			// Disable native splash screen
			utils::hook::set<uint8_t>(SELECT_VALUE(0x462B90_b, 0x5BDF20_b), 0xC3);
			utils::hook::jump(SELECT_VALUE(0x462E40_b, 0x5BE1D0_b), destroy_stub, true);
			utils::hook::jump(SELECT_VALUE(0x462E80_b, 0x5BE210_b), destroy_stub, true);
		}

		void pre_destroy() override
		{
			this->destroy();

			MSG msg;
			while (this->window_ && IsWindow(this->window_))
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

			this->window_ = nullptr;
		}

	private:
		HWND window_{};
		HANDLE image_{};

		static void destroy_stub()
		{
			component_loader::get<component>()->destroy();
		}

		void destroy() const
		{
			if (this->window_ && IsWindow(this->window_))
			{
				ShowWindow(this->window_, SW_HIDE);
				DestroyWindow(this->window_);
				UnregisterClassA("H1 Splash Screen", utils::nt::library{});
			}
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
					this->window_ = CreateWindowExA(WS_EX_APPWINDOW, "H1 Splash Screen", "H1",
						WS_POPUP | WS_SYSMENU,
						(x_pixels - 320) / 2, (y_pixels - 100) / 2, 320, 100, nullptr,
						nullptr,
						self, nullptr);

					if (this->window_)
					{
						auto* const image_window = CreateWindowExA(0, "Static", nullptr, WS_CHILD | WS_VISIBLE | 0xEu,
							0, 0,
							320, 100, this->window_, nullptr, self, nullptr);
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
							SetWindowPos(this->window_, nullptr, rect.left, rect.top, rect.right - rect.left,
								rect.bottom - rect.top, SWP_NOZORDER);

							ShowWindow(this->window_, SW_SHOW);
							UpdateWindow(this->window_);
						}
					}
				}
			}
		}
	};
}

REGISTER_COMPONENT(splash::component)
