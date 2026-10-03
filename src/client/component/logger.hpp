#pragma once
#include "loader/component_loader.hpp"

class logger final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void print_error(const char* msg, ...);
	static void print_com_error(int, const char* msg, ...);
	static void com_error_stub(const int error, const char* msg, ...);
	static void print_warning(const char* msg, ...);
	static void print(const char* msg, ...);
	static void print_dev(const char* msg, ...);
	static void r_warn_once_per_frame_vsnprintf_stub(char* buffer, size_t buffer_length, char* msg, va_list va);
};
