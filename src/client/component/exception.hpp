#pragma once
#include "loader/component_loader.hpp"

class exception_component final : public component_interface
{
public:
	exception_component();

	void post_load() override;
	void post_unpack() override;

private:
	static bool is_game_thread();
	static bool is_exception_interval_too_short();
	static bool too_many_exceptions_occured();
	static volatile bool& is_initialized();
	static bool is_recoverable();
	static void show_mouse_cursor();
	static void display_error_dialog();
	static void reset_state();
	static size_t get_reset_state_stub();
	static std::string get_timestamp();
	static std::string generate_crash_info(LPEXCEPTION_POINTERS exceptioninfo);
	static const char* get_exception_string(DWORD exception);
	static std::string get_memory_registers(LPEXCEPTION_POINTERS exceptioninfo);
	static void write_minidump(LPEXCEPTION_POINTERS exceptioninfo);
	static bool is_harmless_error(LPEXCEPTION_POINTERS exceptioninfo);
	static LONG WINAPI exception_filter(LPEXCEPTION_POINTERS exceptioninfo);
	static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI set_unhandled_exception_filter_stub(LPTOP_LEVEL_EXCEPTION_FILTER);
};
