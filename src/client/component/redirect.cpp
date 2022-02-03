#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include <utils/nt.hpp>
#include <utils/string.hpp>
#include "game_module.hpp"

namespace redirect
{
	namespace
	{
		void launch_complementary_game(const bool singleplayer, const std::string& mode = "")
		{
			const auto self = game_module::get_host_module();

			STARTUPINFOA startup_info;
			PROCESS_INFORMATION process_info;

			ZeroMemory(&startup_info, sizeof(startup_info));
			ZeroMemory(&process_info, sizeof(process_info));
			startup_info.cb = sizeof(startup_info);

			auto* arguments = const_cast<char*>(utils::string::va("%s%s%s", self.get_path().data(),
				(singleplayer ? " -singleplayer" : " -multiplayer"),
				(mode.empty() ? "" : (" +"s + mode).data())));
			CreateProcessA(self.get_path().data(), arguments, nullptr, nullptr, false, NULL, nullptr, nullptr,
				&startup_info, &process_info);

			if (process_info.hThread && process_info.hThread != INVALID_HANDLE_VALUE)
			{
				CloseHandle(process_info.hThread);
			}

			if (process_info.hProcess && process_info.hProcess != INVALID_HANDLE_VALUE)
			{
				CloseHandle(process_info.hProcess);
			}
		}

		HINSTANCE shell_execute_a(const HWND hwnd, const LPCSTR operation, const LPCSTR file, const LPCSTR parameters,
			const LPCSTR directory, const INT show_cmd)
		{
			if (utils::string::starts_with(file, "steam://run/393080/"))
			{
				launch_complementary_game(true);
				return HINSTANCE(33);
			}
			else if (utils::string::starts_with(file, "steam://run/393100/"))
			{
				std::string mode(file);
				mode.erase(0, 20);
				if (!mode.empty())
				{
					mode = utils::string::replace(mode, "%2b", ""); // '+'
					mode = utils::string::replace(mode, "%2", " "); // ' '
				}

				launch_complementary_game(false, mode);
				return HINSTANCE(33);
			}

			return ShellExecuteA(hwnd, operation, file, parameters, directory, show_cmd);
		}
	}

	class component final : public component_interface
	{
	public:
		void* load_import(const std::string& library, const std::string& function) override
		{
			if (library == "SHELL32.dll")
			{
				if (function == "ShellExecuteA")
				{
					return shell_execute_a;
				}
			}

			return nullptr;
		}
	};
}

REGISTER_COMPONENT(redirect::component)
