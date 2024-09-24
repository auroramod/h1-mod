#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/console.hpp"
#include "gui.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gui
{
	std::unordered_map<std::string, bool> enabled_menus;

	namespace
	{
		struct frame_callback
		{
			std::function<void()> callback;
			bool always;
		};

		struct event
		{
			HWND hWnd;
			UINT msg;
			WPARAM wParam;
			LPARAM lParam;
		};

		struct menu_t
		{
			std::string name;
			std::string title;
			std::function<void()> render;
		};

		utils::concurrency::container<std::vector<frame_callback>> on_frame_callbacks;
		utils::concurrency::container<std::deque<notification_t>> notifications;
		utils::concurrency::container<std::vector<event>> event_queue;
		std::vector<menu_t> menus;

		ID3D11Device* device;
		ID3D11DeviceContext* device_context;
		bool initialized = false;
		bool toggled = false;

		void initialize_gui_context()
		{
			ImGui::CreateContext();
			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(*reinterpret_cast<HWND*>(0xC9DD2E0_b));
			ImGui_ImplDX11_Init(device, device_context);

			initialized = true;
		}

		void run_event_queue()
		{
			event_queue.access([](std::vector<event>& queue)
				{
					for (const auto& event : queue)
					{
						ImGui_ImplWin32_WndProcHandler(event.hWnd, event.msg, event.wParam, event.lParam);
					}

					queue.clear();
				});
		}

		std::vector<int> imgui_colors =
		{
			ImGuiCol_FrameBg,
			ImGuiCol_FrameBgHovered,
			ImGuiCol_FrameBgActive,
			ImGuiCol_TitleBgActive,
			ImGuiCol_ScrollbarGrabActive,
			ImGuiCol_CheckMark,
			ImGuiCol_SliderGrab,
			ImGuiCol_SliderGrabActive,
			ImGuiCol_Button,
			ImGuiCol_ButtonHovered,
			ImGuiCol_ButtonActive,
			ImGuiCol_Header,
			ImGuiCol_HeaderHovered,
			ImGuiCol_HeaderActive,
			ImGuiCol_SeparatorHovered,
			ImGuiCol_SeparatorActive,
			ImGuiCol_ResizeGrip,
			ImGuiCol_ResizeGripHovered,
			ImGuiCol_ResizeGripActive,
			ImGuiCol_TextSelectedBg,
			ImGuiCol_NavHighlight,
		};

		void update_colors()
		{
			auto& style = ImGui::GetStyle();
			const auto colors = style.Colors;

			const auto now = std::chrono::system_clock::now();
			const auto days = std::chrono::floor<std::chrono::days>(now);
			std::chrono::year_month_day y_m_d{ days };

			if (y_m_d.month() != std::chrono::month(6))
			{
				return;
			}

			for (const auto& id : imgui_colors)
			{
				const auto color = colors[id];

				ImVec4 hsv_color =
				{
					static_cast<float>((game::Sys_Milliseconds() / 100) % 256) / 255.f,
					1.f, 1.f, 1.f,
				};

				ImVec4 rgba_color{};
				ImGui::ColorConvertHSVtoRGB(hsv_color.x, hsv_color.y, hsv_color.z, rgba_color.x, rgba_color.y, rgba_color.z);

				rgba_color.w = color.w;
				colors[id] = rgba_color;
			}
		}

		void new_gui_frame()
		{
			ImGui::GetIO().MouseDrawCursor = toggled;
			if (toggled)
			{
				*reinterpret_cast<int*>(0xC9DC405_b) = 0;
				*game::keyCatchers |= 0x10;
			}
			else
			{
				*reinterpret_cast<int*>(0xC9DC405_b) = 1;
				*game::keyCatchers &= ~0x10;
			}

			update_colors();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			run_event_queue();

			ImGui::NewFrame();
		}

		void end_gui_frame()
		{
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		void toggle_menu(const std::string& name)
		{
			enabled_menus[name] = !enabled_menus[name];
		}

		std::string truncate(const std::string& text, const size_t length, const std::string& end)
		{
			return text.size() <= length
				? text
				: text.substr(0, length - end.size()) + end;
		}

		void show_notifications()
		{
			static const auto window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
				ImGuiWindowFlags_NoMove;

			notifications.access([](std::deque<notification_t>& notifications_)
				{
					auto index = 0;
					for (auto i = notifications_.begin(); i != notifications_.end();)
					{
						const auto now = std::chrono::high_resolution_clock::now();
						if (now - i->creation_time >= i->duration)
						{
							i = notifications_.erase(i);
							continue;
						}

						const auto title = truncate(i->title, 34, "...");
						const auto text = truncate(i->text, 34, "...");

						ImGui::SetNextWindowSizeConstraints(ImVec2(250, 50), ImVec2(250, 50));
						ImGui::SetNextWindowBgAlpha(0.6f);
						ImGui::Begin(utils::string::va("Notification #%i", index), nullptr, window_flags);

						ImGui::SetWindowPos(ImVec2(10, 30.f + static_cast<float>(index) * 60.f));
						ImGui::SetWindowSize(ImVec2(250, 0));
						ImGui::Text(title.data());
						ImGui::Text(text.data());

						ImGui::End();

						++i;
						++index;
					}
				});
		}

		void menu_checkbox(const std::string& name, const std::string& menu)
		{
			ImGui::Checkbox(name.data(), &enabled_menus[menu]);
		}

		void run_frame_callbacks()
		{
			on_frame_callbacks.access([](std::vector<frame_callback>& callbacks)
				{
					for (const auto& callback : callbacks)
					{
						if (callback.always || toggled)
						{
							callback.callback();
						}
					}
				});
		}

		void draw_main_menu_bar()
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("Windows"))
				{
					for (const auto& menu : menus)
					{
						menu_checkbox(menu.title, menu.name);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}

		void gui_on_frame()
		{
			if (!game::Sys_IsDatabaseReady2())
			{
				return;
			}

			if (!initialized)
			{
				console::info("[ImGui] Initializing\n");
				initialize_gui_context();
			}
			else
			{
				new_gui_frame();
				run_frame_callbacks();
				end_gui_frame();
			}
		}

		void shutdown_gui()
		{
			if (initialized)
			{
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
			}

			initialized = false;
		}

		HRESULT d3d11_create_device_stub(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
			UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
			ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
		{
			shutdown_gui();

			const auto result = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels,
				FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);

			if (ppDevice != nullptr && ppImmediateContext != nullptr)
			{
				device = *ppDevice;
				device_context = *ppImmediateContext;
			}

			return result;
		}

		utils::hook::detour wnd_proc_hook;
		LRESULT wnd_proc_stub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (wParam != VK_ESCAPE && toggled)
			{
				event_queue.access([hWnd, msg, wParam, lParam](std::vector<event>& queue)
					{
						queue.emplace_back(hWnd, msg, wParam, lParam);
					});
			}

			return wnd_proc_hook.invoke<LRESULT>(hWnd, msg, wParam, lParam);
		}
	}

	bool gui_key_event(const int local_client_num, const int key, const int down)
	{
		if (key == game::K_F11 && down)
		{
			toggled = !toggled;
			return false;
		}

		if (key == game::K_ESCAPE && down && toggled)
		{
			toggled = false;
			return false;
		}

		return !toggled;
	}

	bool gui_char_event(const int local_client_num, const int key)
	{
		return !toggled;
	}

	bool gui_mouse_event(const int local_client_num, int x, int y)
	{
		return !toggled;
	}

	void on_frame(const std::function<void()>& callback, bool always)
	{
		on_frame_callbacks.access([always, callback](std::vector<frame_callback>& callbacks)
			{
				callbacks.emplace_back(callback, always);
			});
	}

	bool is_menu_open(const std::string& name)
	{
		return enabled_menus[name];
	}

	void notification(const std::string& title, const std::string& text, const std::chrono::milliseconds duration)
	{
		notification_t notification{};
		notification.title = title;
		notification.text = text;
		notification.duration = duration;
		notification.creation_time = std::chrono::high_resolution_clock::now();

		notifications.access([notification](std::deque<notification_t>& notifications_)
			{
				notifications_.push_front(notification);
			});
	}

	void copy_to_clipboard(const std::string& text)
	{
		utils::string::set_clipboard_data(text);
		gui::notification("Text copied to clipboard", utils::string::va("\"%s\"", text.data()));
	}

	void register_menu(const std::string& name, const std::string& title,
		const std::function<void()>& callback, bool always)
	{
		menus.emplace_back(name, title, callback);
		enabled_menus[name] = false;

		on_frame([=]
			{
				if (enabled_menus.at(name))
				{
					callback();
				}
			}, always);
	}

	void register_callback(const std::function<void()>& callback, bool always)
	{
		on_frame([=]
		{
			callback();
		}, always);
	}

	class component final : public component_interface
	{
	public:
		void* load_import(const std::string& library, const std::string& function) override
		{
			if (function == "D3D11CreateDevice" && !game::environment::is_dedi())
			{
				return d3d11_create_device_stub;
			}

			return nullptr;
		}

		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			utils::hook::nop(0x6CB16D_b, 9);
			utils::hook::call(0x6CB170_b, gui_on_frame);
			wnd_proc_hook.create(0x5BFF60_b, wnd_proc_stub);

			on_frame([]
			{
				show_notifications();
				draw_main_menu_bar();
			});
		}

		void pre_destroy() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			shutdown_gui();
		}
	};
}

REGISTER_COMPONENT(gui::component)
#endif
