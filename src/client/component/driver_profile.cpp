/*

	AMD's DirectX11 driver amdxx64.dll applies game app profiles based on the process executable name, which includes weird visual 
	fixes and performance issues that do not apply otherwise. this component exists to pretty much spoof the name to AMD's driver 
	as h1_mp64_ship.exe & h1_sp64_ship.exe instead of h1-mod.exe

*/

#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "driver_profile.hpp"
#include "game_module.hpp"

#include "game/game.hpp"

#include <utils/flags.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace driver_profile
{
	namespace
	{
		utils::hook::detour get_module_base_name_w_hook;
		utils::hook::detour get_module_file_name_ex_a_hook;
		utils::hook::detour get_command_line_a_hook;
		utils::hook::detour get_command_line_w_hook;

		bool enabled = false;

		std::wstring fake_path_w;
		std::string fake_path_a;
		std::wstring fake_name_w;
		std::wstring fake_command_line_w;
		std::string fake_command_line_a;

		bool is_driver_module(const HMODULE module)
		{
			static std::mutex mutex;
			static std::unordered_map<HMODULE, bool> cache;

			std::lock_guard _(mutex);

			const auto entry = cache.find(module);
			if (entry != cache.end())
			{
				return entry->second;
			}

			wchar_t buffer[MAX_PATH]{};
			get_module_base_name_w_hook.invoke<DWORD>(GetCurrentProcess(), module, buffer, static_cast<DWORD>(std::size(buffer)));

			const auto name = utils::string::to_lower(utils::string::convert(buffer));
			const auto result = name.starts_with("amd") || name.starts_with("ati");

			cache[module] = result;
			return result;
		}

		bool is_driver_caller(void* return_address)
		{
			HMODULE module{};
			if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				static_cast<LPCSTR>(return_address), &module) || !module)
			{
				return false;
			}

			return is_driver_module(module);
		}

		bool is_main_module(const HMODULE module)
		{
			return module == nullptr || module == game_module::get_host_module().get_handle()
				|| module == game_module::get_game_module().get_handle();
		}

		bool is_current_process(const HANDLE process)
		{
			return process == GetCurrentProcess() || GetProcessId(process) == GetCurrentProcessId();
		}

		template <typename T>
		DWORD copy_string(const std::basic_string<T>& source, T* buffer, const DWORD size)
		{
			if (!buffer || size == 0)
			{
				SetLastError(ERROR_INSUFFICIENT_BUFFER);
				return 0;
			}

			const auto length = static_cast<DWORD>(source.size());
			if (length >= size)
			{
				std::memcpy(buffer, source.data(), (size - 1) * sizeof(T));
				buffer[size - 1] = 0;
				SetLastError(ERROR_INSUFFICIENT_BUFFER);
				return size;
			}

			std::memcpy(buffer, source.data(), length * sizeof(T));
			buffer[length] = 0;
			SetLastError(ERROR_SUCCESS);
			return length;
		}

		DWORD WINAPI get_module_base_name_w_stub(const HANDLE process, const HMODULE module, const LPWSTR base_name, const DWORD size)
		{
			if (is_current_process(process) && is_main_module(module) && is_driver_caller(_ReturnAddress()))
			{
				return copy_string(fake_name_w, base_name, size);
			}

			return get_module_base_name_w_hook.invoke<DWORD>(process, module, base_name, size);
		}

		DWORD WINAPI get_module_file_name_ex_a_stub(const HANDLE process, const HMODULE module, const LPSTR filename, const DWORD size)
		{
			if (is_current_process(process) && is_main_module(module) && is_driver_caller(_ReturnAddress()))
			{
				return copy_string(fake_path_a, filename, size);
			}

			return get_module_file_name_ex_a_hook.invoke<DWORD>(process, module, filename, size);
		}

		LPSTR WINAPI get_command_line_a_stub()
		{
			if (is_driver_caller(_ReturnAddress()))
			{
				return fake_command_line_a.data();
			}

			return get_command_line_a_hook.invoke<LPSTR>();
		}

		LPWSTR WINAPI get_command_line_w_stub()
		{
			if (is_driver_caller(_ReturnAddress()))
			{
				return fake_command_line_w.data();
			}

			return get_command_line_w_hook.invoke<LPWSTR>();
		}

		std::wstring build_fake_command_line(const std::wstring& command_line)
		{
			size_t args_start{};
			if (!command_line.empty() && command_line[0] == L'"')
			{
				const auto end = command_line.find(L'"', 1);
				args_start = end == std::wstring::npos ? command_line.size() : end + 1;
			}
			else
			{
				const auto end = command_line.find_first_of(L" \t");
				args_start = end == std::wstring::npos ? command_line.size() : end;
			}

			return L"\"" + fake_path_w + L"\"" + command_line.substr(args_start);
		}

		void setup_fake_names()
		{
			wchar_t buffer[MAX_PATH]{};
			GetModuleFileNameW(game_module::get_host_module(), buffer, static_cast<DWORD>(std::size(buffer)));

			std::filesystem::path path(buffer);
			path.replace_filename(game::environment::is_sp() ? "h1_sp64_ship.exe" : "h1_mp64_ship.exe");

			fake_path_w = path.wstring();
			fake_path_a = path.string();
			fake_name_w = path.filename().wstring();

			fake_command_line_w = build_fake_command_line(GetCommandLineW());
			fake_command_line_a = utils::string::convert(fake_command_line_w);
		}
	}

	bool get_module_file_name(void* return_address, const HMODULE module, const LPSTR filename, const DWORD size, DWORD* result)
	{
		if (!enabled || !is_main_module(module) || !is_driver_caller(return_address))
		{
			return false;
		}

		*result = copy_string(fake_path_a, filename, size);
		return true;
	}

	bool get_module_file_name(void* return_address, const HMODULE module, const LPWSTR filename, const DWORD size, DWORD* result)
	{
		if (!enabled || !is_main_module(module) || !is_driver_caller(return_address))
		{
			return false;
		}

		*result = copy_string(fake_path_w, filename, size);
		return true;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			if (game::environment::is_dedi() || utils::flags::has_flag("nodriverprofile"))
			{
				return;
			}

			setup_fake_names();

			get_module_base_name_w_hook.create(&K32GetModuleBaseNameW, &get_module_base_name_w_stub);
			get_module_file_name_ex_a_hook.create(&K32GetModuleFileNameExA, &get_module_file_name_ex_a_stub);
			get_command_line_a_hook.create(&GetCommandLineA, &get_command_line_a_stub);
			get_command_line_w_hook.create(&GetCommandLineW, &get_command_line_w_stub);

			enabled = true;
		}
	};
}

REGISTER_COMPONENT(driver_profile::component)
