#pragma once
#include "loader/component_loader.hpp"

class console final : public component_interface
{
public:
	console();

	void post_unpack() override;
	void pre_destroy() override;

	enum console_type
	{
		con_type_error = 1,
		con_type_debug = 2,
		con_type_warning = 3,
		con_type_info = 7
	};

	static void print(int type, const char* fmt, ...);

	template <typename... Args>
	static void error(const char* fmt, Args&&... args)
	{
		print(con_type_error, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void debug(const char* fmt, Args&&... args)
	{
#ifdef _DEBUG
		print(con_type_debug, fmt, std::forward<Args>(args)...);
#endif
	}

	template <typename... Args>
	static void warn(const char* fmt, Args&&... args)
	{
		print(con_type_warning, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void info(const char* fmt, Args&&... args)
	{
		print(con_type_info, fmt, std::forward<Args>(args)...);
	}

private:
	static void set_cursor_pos(int x);
	static void show_cursor(bool show);
	template <typename... Args>
	static int invoke_printf(const char* fmt, Args&&... args);
	static std::string format(va_list* ap, const char* message);
	static uint8_t get_attribute(int type);
	static void update();
	static void clear_output();
	static int dispatch_message(int type, const std::string& message);
	static void clear();
	static size_t get_max_input_length();
	static void handle_resize();
	static void handle_input(INPUT_RECORD record);
	static int __cdecl printf_stub(const char* fmt, ...);
};
