#pragma once

namespace console
{
	HWND get_window();
	void set_title(std::string title);
	void set_size(int width, int height);

	enum console_type
	{
		con_type_error = 1,
		con_type_debug = 2,
		con_type_warning = 3,
		con_type_info = 7
	};

	void print(int type, const char* fmt, ...);

	template <typename... Args>
	void error(const char* fmt, Args&&... args)
	{
		print(con_type_error, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void debug(const char* fmt, Args&&... args)
	{
#ifdef DEBUG
		print(con_type_debug, fmt, std::forward<Args>(args)...);
#endif
	}

	template <typename... Args>
	void warn(const char* fmt, Args&&... args)
	{
		print(con_type_warning, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void info(const char* fmt, Args&&... args)
	{
		print(con_type_info, fmt, std::forward<Args>(args)...);
	}
}