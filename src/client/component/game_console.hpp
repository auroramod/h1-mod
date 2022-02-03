#pragma once

namespace game_console
{
	enum console_type
	{
		con_type_error = 1,
		con_type_warning = 3,
		con_type_info = 7
	};

	void print(int type, const char* fmt, ...);

	bool console_char_event(int local_client_num, int key);
	bool console_key_event(int local_client_num, int key, int down);

	void execute(const char* cmd);
}