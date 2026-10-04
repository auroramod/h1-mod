#pragma once

namespace driver_profile
{
	bool get_module_file_name(void* return_address, HMODULE module, LPSTR filename, DWORD size, DWORD* result);
	bool get_module_file_name(void* return_address, HMODULE module, LPWSTR filename, DWORD size, DWORD* result);
}
