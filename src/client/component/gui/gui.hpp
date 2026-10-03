#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

class gui final : public component_interface
{
public:
	void post_unpack() override;
	void pre_destroy() override;

	struct notification_t
	{
		std::string title;
		std::string text;
		std::chrono::milliseconds duration{};
		std::chrono::high_resolution_clock::time_point creation_time{};
	};

	static std::unordered_map<std::string, bool> enabled_menus;

	static ID3D11Device* device;
	static ID3D11DeviceContext* device_context;

	static bool gui_key_event(const int local_client_num, const int key, const int down);
	static bool gui_char_event(const int local_client_num, const int key);
	static bool gui_mouse_event(const int local_client_num, int x, int y);

	static void on_frame(const std::function<void()>& callback, bool always = false);
	static bool is_menu_open(const std::string& name);
	static void notification(const std::string& title, const std::string& text, const std::chrono::milliseconds duration = 3s);
	static void copy_to_clipboard(const std::string& text);

	static void register_menu(const std::string& name, const std::string& title,
		const std::function<void()>& callback, bool always = false);

	static void register_callback(const std::function<void()>& callback, bool always = false);

	static bool InputU8(const char* label, unsigned char* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	static bool InputUInt6(const char* label, unsigned int v[6], ImGuiInputTextFlags flags = 0);

	static void shutdown_gui();

private:
	static void initialize_gui_context();
	static void run_event_queue();
	static void update_colors();
	static void new_gui_frame();
	static void end_gui_frame();
	static void toggle_menu(const std::string& name);
	static std::string truncate(const std::string& text, const size_t length, const std::string& end);
	static void show_notifications();
	static void menu_checkbox(const std::string& name, const std::string& menu);
	static void run_frame_callbacks();
	static void draw_main_menu_bar();
	static void gui_on_frame();
	static char gui_frame_stub();
	static LRESULT wnd_proc_stub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void toggle();
};
#endif
