#pragma once
#include "loader/component_loader.hpp"

#include <utils/nt.hpp>

class game_module final : public component_interface
{
public:
	void post_start() override;
	void post_load() override;

	static utils::nt::library get_game_module();
	static utils::nt::library get_host_module();

private:
	static void hook_module_resolving();

	static HMODULE __stdcall get_module_handle_a(LPCSTR module_name);
	static HMODULE __stdcall get_module_handle_w(LPWSTR module_name);
	static BOOL __stdcall get_module_handle_ex_a(DWORD flags, LPCSTR module_name, HMODULE* hmodule);
	static BOOL __stdcall get_module_handle_ex_w(DWORD flags, LPCWSTR module_name, HMODULE* hmodule);
	static DWORD __stdcall get_module_file_name_a(HMODULE hmodule, LPSTR filename, DWORD size);
	static DWORD __stdcall get_module_file_name_w(HMODULE hmodule, LPWSTR filename, DWORD size);
};
