#pragma once
#include "loader/component_loader.hpp"

class driver_profile final : public component_interface
{
public:
	void post_load() override;

	static bool get_module_file_name(void* return_address, HMODULE module, LPSTR filename, DWORD size, DWORD* result);
	static bool get_module_file_name(void* return_address, HMODULE module, LPWSTR filename, DWORD size, DWORD* result);

private:
	static bool is_driver_module(const HMODULE module);
	static bool is_driver_caller(void* return_address);
	static bool is_main_module(const HMODULE module);
	static bool is_current_process(const HANDLE process);
	template <typename T>
	static DWORD copy_string(const std::basic_string<T>& source, T* buffer, const DWORD size);
	static DWORD WINAPI get_module_base_name_w_stub(const HANDLE process, const HMODULE module, const LPWSTR base_name, const DWORD size);
	static DWORD WINAPI get_module_file_name_ex_a_stub(const HANDLE process, const HMODULE module, const LPSTR filename, const DWORD size);
	static LPSTR WINAPI get_command_line_a_stub();
	static LPWSTR WINAPI get_command_line_w_stub();
	static std::wstring build_fake_command_line(const std::wstring& command_line);
	static void setup_fake_names();
};
