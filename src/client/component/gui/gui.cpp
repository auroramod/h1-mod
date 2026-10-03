#include <std_include.hpp>

#ifdef _DEBUG
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
}

std::unordered_map<std::string, bool> gui::enabled_menus;

ID3D11Device* gui::device;
ID3D11DeviceContext* gui::device_context;

static utils::concurrency::container<std::vector<frame_callback>> on_frame_callbacks;
static utils::concurrency::container<std::deque<gui::notification_t>> notifications;
static utils::concurrency::container<std::vector<event>> event_queue;
static std::vector<menu_t> menus;

static bool initialized = false;
static bool toggled = false;

static std::vector<int> imgui_colors =
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

static utils::hook::detour wnd_proc_hook;

void gui::post_unpack()
{
	if (game::environment::is_dedi() || game::environment::is_sp())
	{
		return;
	}

	utils::hook::call(0x1406222A7, gui_frame_stub);
	wnd_proc_hook.create(0x1405162D0, wnd_proc_stub); // WndProc

	// weird build number that renders when gui is shown
	utils::hook::nop(0x1404CE03E, 5);

	on_frame([]
	{
		show_notifications();
		draw_main_menu_bar();
	});
}

void gui::pre_destroy()
{
	if (game::environment::is_dedi() || game::environment::is_sp())
	{
		return;
	}

	shutdown_gui();
}

void gui::toggle()
{
	if (!toggled)
	{
		*reinterpret_cast<std::uint8_t*>(0x14DDF84D5) = 0;
		*game::keyCatchers |= 0x10;
	}
	else
	{
		*reinterpret_cast<std::uint8_t*>(0x14DDF84D5) = 1;
		*game::keyCatchers &= ~0x10;
	}
	toggled = !toggled;
}

bool gui::gui_key_event(const int local_client_num, const int key, const int down)
{
	if (key == game::K_F11 && down)
	{
		toggle();
		return false;
	}

	if (key == game::K_ESCAPE && down && toggled)
	{
		toggle();
		return false;
	}

	return !toggled;
}

bool gui::gui_char_event(const int local_client_num, const int key)
{
	return !toggled;
}

bool gui::gui_mouse_event(const int local_client_num, int x, int y)
{
	return !toggled;
}

void gui::on_frame(const std::function<void()>& callback, bool always)
{
	on_frame_callbacks.access([always, callback](std::vector<frame_callback>& callbacks)
	{
		callbacks.emplace_back(callback, always);
	});
}

bool gui::is_menu_open(const std::string& name)
{
	return enabled_menus[name];
}

void gui::notification(const std::string& title, const std::string& text, const std::chrono::milliseconds duration)
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

void gui::copy_to_clipboard(const std::string& text)
{
	utils::string::set_clipboard_data(text);
	gui::notification("Text copied to clipboard", utils::string::va("\"%s\"", text.data()));
}

void gui::register_menu(const std::string& name, const std::string& title,
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

void gui::register_callback(const std::function<void()>& callback, bool always)
{
	on_frame([=]
	{
		callback();
	}, always);
}

bool gui::InputU8(const char* label, unsigned char* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
	const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
	return ImGui::InputScalar(label, ImGuiDataType_U8, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool gui::InputUInt6(const char* label, unsigned int v[6], ImGuiInputTextFlags flags)
{
	return ImGui::InputScalarN(label, ImGuiDataType_U32, v, 6, NULL, NULL, "%d", flags);
}

void gui::shutdown_gui()
{
	if (initialized)
	{
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	initialized = false;
}

void gui::initialize_gui_context()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(*reinterpret_cast<HWND*>(0x14DDFCB80)); // hWnd
	ImGui_ImplDX11_Init(device, device_context);

	initialized = true;
}

void gui::run_event_queue()
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

void gui::update_colors()
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

void gui::new_gui_frame()
{
	ImGui::GetIO().MouseDrawCursor = toggled;

	update_colors();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	run_event_queue();

	ImGui::NewFrame();
}

void gui::end_gui_frame()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void gui::toggle_menu(const std::string& name)
{
	enabled_menus[name] = !enabled_menus[name];
}

std::string gui::truncate(const std::string& text, const size_t length, const std::string& end)
{
	return text.size() <= length
		? text
		: text.substr(0, length - end.size()) + end;
}

void gui::show_notifications()
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

void gui::menu_checkbox(const std::string& name, const std::string& menu)
{
	ImGui::Checkbox(name.data(), &enabled_menus[menu]);
}

void gui::run_frame_callbacks()
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

void gui::draw_main_menu_bar()
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

void gui::gui_on_frame()
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

char gui::gui_frame_stub()
{
	const auto result = utils::hook::invoke<char>(0x14064CF80);

	// only draw on frames the game renders
	if (result == 0 && *reinterpret_cast<int*>(0x14DDFCBE0) <= 0)
	{
		gui_on_frame();
	}

	return result;
}

LRESULT gui::wnd_proc_stub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

REGISTER_COMPONENT(gui)
#endif
