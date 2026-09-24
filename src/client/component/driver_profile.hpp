#pragma once

namespace driver_profile
{
	// returns true and fills filename if the caller is an AMD driver module asking for the main executable name
	bool get_module_file_name(void* return_address, HMODULE module, LPSTR filename, DWORD size, DWORD* result);
	bool get_module_file_name(void* return_address, HMODULE module, LPWSTR filename, DWORD size, DWORD* result);
}
