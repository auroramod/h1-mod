#include <std_include.hpp>
#include "launcher/launcher.hpp"
#include "loader/loader.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "component/arxan.hpp"

#include <utils/flags.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/properties.hpp>

DECLSPEC_NORETURN void WINAPI exit_hook(const int code)
{
	component_loader::pre_destroy();
	exit(code);
}

BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
{
	component_loader::post_unpack();
	return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

launcher::mode detect_mode_from_arguments()
{
	if (utils::flags::has_flag("dedicated"))
	{
		return launcher::mode::server;
	}

	if (utils::flags::has_flag("multiplayer"))
	{
		return launcher::mode::multiplayer;
	}

	if (utils::flags::has_flag("singleplayer"))
	{
		return launcher::mode::singleplayer;
	}

	return launcher::mode::none;
}

void apply_aslr_patch(std::string* data)
{
	// mp binary, sp binary
	if (data->size() != 0x1B97788 && data->size() != 0x1346D88)
	{
		throw std::runtime_error("File size mismatch, bad game files");
	}

	auto* dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(&data->at(0));
	auto* nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(&data->at(dos_header->e_lfanew));
	auto* optional_header = &nt_headers->OptionalHeader;

	if (optional_header->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE)
	{
		optional_header->DllCharacteristics &= ~(IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE);
	}
}

void get_aslr_patched_binary(std::string* binary, std::string* data)
{
	const auto patched_binary = (utils::properties::get_appdata_path() / "bin" / *binary).generic_string();

	try
	{
		apply_aslr_patch(data);
		if (!utils::io::file_exists(patched_binary) && !utils::io::write_file(patched_binary, *data, false))
		{
			throw std::runtime_error("Could not write file");
		}
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(
			utils::string::va("Could not create aslr patched binary for %s! %s", 
				binary->data(), e.what())
		);
	}

	*binary = patched_binary;
}

FARPROC load_binary(const launcher::mode mode, uint64_t* base_address)
{
	loader loader;
	utils::nt::library self;

	loader.set_import_resolver([self](const std::string& library, const std::string& function) -> void*
	{
		if (library == "steam_api64.dll"
			&& function != "SteamAPI_GetSteamInstallPath") // Arxan requires one valid steam api import - maybe SteamAPI_Shutdown is better?
		{
			static bool check_for_steam_install = false;
			if (!check_for_steam_install && !arxan::is_wine())
			{
				HKEY key;
				if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
				{
					RegCloseKey(key);
				}
				else
				{
					throw std::runtime_error("Could not find Steam in the registry. If Steam is not installed, you must install it for H1-Mod to work.");
				}

				check_for_steam_install = true;
			}

			return self.get_proc<FARPROC>(function);
		}
		else if (function == "ExitProcess")
		{
			return exit_hook;
		}
		else if (function == "SystemParametersInfoA")
		{
			return system_parameters_info_a;
		}

		return component_loader::load_import(library, function);
	});

	std::string binary;
	switch (mode)
	{
	case launcher::mode::server:
	case launcher::mode::multiplayer:
		binary = "h1_mp64_ship.exe";
		break;
	case launcher::mode::singleplayer:
		binary = "h1_sp64_ship.exe";
		break;
	case launcher::mode::none:
	default:
		throw std::runtime_error("Invalid game mode!");
	}

	std::string data;
	if (!utils::io::read_file(binary, &data))
	{
		throw std::runtime_error(utils::string::va(
			"Failed to read game binary (%s)!\nPlease copy the h1-mod.exe into your Call of Duty: Modern Warfare Remastered installation folder and run it from there.",
			binary.data()));
	}

	get_aslr_patched_binary(&binary, &data);
 
#ifdef INJECT_HOST_AS_LIB
	return loader.load_library(binary, base_address);
#else
	*base_address = 0x140000000;
	return loader.load(self, data);
#endif
}

void remove_crash_file()
{
	utils::io::remove_file("__h1Exe");
	utils::io::remove_file("h1-mod\\h1_mp64_ship.exe"); // remove this at some point
}

void enable_dpi_awareness()
{
	const utils::nt::library user32{"user32.dll"};
	const auto set_dpi = user32
		                     ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>("SetProcessDpiAwarenessContext")
		                     : nullptr;
	if (set_dpi)
	{
		set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}
}

void limit_parallel_dll_loading()
{
	const utils::nt::library self;
	const auto registry_path = R"(Software\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\)" + self.
		get_name();

	HKEY key = nullptr;
	if (RegCreateKeyA(HKEY_LOCAL_MACHINE, registry_path.data(), &key) == ERROR_SUCCESS)
	{
		RegCloseKey(key);
	}

	key = nullptr;
	if (RegOpenKeyExA(
		HKEY_LOCAL_MACHINE, registry_path.data(), 0,
		KEY_ALL_ACCESS, &key) != ERROR_SUCCESS)
	{
		return;
	}

	DWORD value = 1;
	RegSetValueExA(key, "MaxLoaderThreads", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));

	RegCloseKey(key);
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	FARPROC entry_point;

	// leaving these for Windows only for now, need to test to see if we can have for Wine -mikey
	if (!arxan::is_wine())
	{
		enable_dpi_awareness();
	
		// This requires admin privilege, but I suppose many
		// people will start with admin rights if it crashes.
		limit_parallel_dll_loading();
	}

	srand(uint32_t(time(nullptr)));
	remove_crash_file();

	{
		auto premature_shutdown = true;
		const auto _ = gsl::finally([&premature_shutdown]()
		{
			if (premature_shutdown)
			{
				component_loader::pre_destroy();
			}
		});

		try
		{
			if (!component_loader::post_start()) return 0;

			auto mode = detect_mode_from_arguments();
			if (mode == launcher::mode::none)
			{
				const launcher launcher;
				mode = launcher.run();
				if (mode == launcher::mode::none) return 0;
			}

			game::environment::set_mode(mode);

			uint64_t base_address{};
			entry_point = load_binary(mode, &base_address);
			if (!entry_point)
			{
				throw std::runtime_error("Unable to load binary into memory");
			}

			game::base_address = base_address;

			if (!component_loader::post_load()) return 0;

			premature_shutdown = false;
		}
		catch (std::exception& e)
		{
			MSG_BOX_ERROR(e.what());
			return 1;
		}
	}

	return static_cast<int>(entry_point());
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	return main();
}
