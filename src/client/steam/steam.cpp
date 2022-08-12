#include <std_include.hpp>
#include "steam.hpp"

#include "component/arxan.hpp"
#include "component/console.hpp"

#include <utils/nt.hpp>
#include <utils/string.hpp>

namespace steam
{
	namespace
	{
		void open_folder_prompt(char* directory)
		{
#ifdef _DEBUG
			MSG_BOX_INFO(__FUNCTION__);
#endif
			if (CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK)
			{
				MSG_BOX_ERROR("CoInitializeEx failed. This could be because uninitialization failed, try again.");
				return;
			}

			IFileOpenDialog* dialog;
			if (CoCreateInstance(CLSID_FileOpenDialog, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog)) != S_OK)
			{
				return;
			}

			FILEOPENDIALOGOPTIONS dw_options;
			if (dialog->GetOptions(&dw_options) != S_OK)
			{
				return;
			}

			if (dialog->SetOptions(dw_options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST) != S_OK)
			{
				return;
			}

			if (dialog->SetTitle(L"Select a valid Steam install (contains libraries like 'steam_api64.dll')") != S_OK)
			{
				return;
			}

			if (dialog->Show(0) != S_OK) // doesn't returns S_OK when operation is cancelled
			{
				// uninitialize on error so we can use this function again
				dialog->Release();
				CoUninitialize();
				return;
			}

			IShellItem* shell_item_result;
			if (dialog->GetResult(&shell_item_result) != S_OK)
			{
				return;
			}

			PWSTR tmp = nullptr;
			if (shell_item_result->GetDisplayName(SIGDN_FILESYSPATH, &tmp) != S_OK)
			{
				return;
			}

			shell_item_result->Release();
			dialog->Release();
			CoUninitialize();

			if (tmp == nullptr)
			{
				MSG_BOX_ERROR("Failed to get temporary path from GetDisplayName.");
				return;
			}

			std::size_t i;
			wcstombs_s(&i, directory, MAX_PATH, tmp, MAX_PATH);
		}
	}

	uint64_t callbacks::call_id_ = 0;
	std::recursive_mutex callbacks::mutex_;
	std::map<uint64_t, bool> callbacks::calls_;
	std::map<uint64_t, callbacks::base*> callbacks::result_handlers_;
	std::vector<callbacks::result> callbacks::results_;
	std::vector<callbacks::base*> callbacks::callback_list_;

	uint64_t callbacks::register_call()
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		calls_[++call_id_] = false;
		return call_id_;
	}

	void callbacks::register_callback(base* handler, const int callback)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		handler->set_i_callback(callback);
		callback_list_.push_back(handler);
	}

	void callbacks::unregister_callback(base* handler)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		for (auto i = callback_list_.begin(); i != callback_list_.end();)
		{
			if (*i == handler)
			{
				i = callback_list_.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void callbacks::register_call_result(const uint64_t call, base* result)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		result_handlers_[call] = result;
	}

	void callbacks::unregister_call_result(const uint64_t call, base* /*result*/)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);
		const auto i = result_handlers_.find(call);
		if (i != result_handlers_.end())
		{
			result_handlers_.erase(i);
		}
	}

	void callbacks::return_call(void* data, const int size, const int type, const uint64_t call)
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);

		result result{};
		result.call = call;
		result.data = data;
		result.size = size;
		result.type = type;

		calls_[call] = true;

		results_.emplace_back(result);
	}

	void callbacks::run_callbacks()
	{
		std::lock_guard<std::recursive_mutex> _(mutex_);

		for (const auto& result : results_)
		{
			if (result_handlers_.find(result.call) != result_handlers_.end())
			{
				result_handlers_[result.call]->run(result.data, false, result.call);
			}

			/*for (const auto& callback : callback_list_)
			{
				if (callback && callback->get_i_callback() == result.type)
				{
					callback->run(result.data, false, 0);
				}
			}*/

			if (result.data)
			{
				free(result.data);
			}
		}

		results_.clear();
	}

	extern "C" {

	bool SteamAPI_RestartAppIfNecessary()
	{
		return false;
	}

	bool SteamAPI_Init()
	{
#ifdef _DEBUG
		MSG_BOX_INFO(__FUNCTION__);
#endif
		const std::filesystem::path steam_path = steam::SteamAPI_GetSteamInstallPath();
		if (steam_path.empty()) 
		{
			return true;
		}

		::utils::nt::library::load(steam_path / "tier0_s64.dll"); 				// not shipped with Steam on linux
		::utils::nt::library::load(steam_path / "vstdlib_s64.dll"); 			// ^
		::utils::nt::library::load(steam_path / "gameoverlayrenderer64.dll");	// however, this one is shipped
		::utils::nt::library::load(steam_path / "steamclient64.dll");			// and this one too

		return true;
	}

	void SteamAPI_RegisterCallResult(callbacks::base* result, const uint64_t call)
	{
		callbacks::register_call_result(call, result);
	}

	void SteamAPI_RegisterCallback(callbacks::base* handler, const int callback)
	{
		callbacks::register_callback(handler, callback);
	}

	void SteamAPI_RunCallbacks()
	{
		callbacks::run_callbacks();
	}

	void SteamAPI_Shutdown()
	{
	}

	void SteamAPI_UnregisterCallResult(callbacks::base* result, const uint64_t call)
	{
		callbacks::unregister_call_result(call, result);
	}

	void SteamAPI_UnregisterCallback(callbacks::base* handler)
	{
		callbacks::unregister_callback(handler);
	}

	// TODO: does the registry even work for Wine? i seriously doubt it does, so this may need to be fixed for Wine
	const char* SteamAPI_GetSteamInstallPath()
	{
#ifdef _DEBUG
		MSG_BOX_INFO(__FUNCTION__);
#endif
		static std::string install_path{};
		if (!install_path.empty())
		{
			return install_path.data();
		}

		char path[MAX_PATH] = {0};
		DWORD length = sizeof(path);

		HKEY reg_key;

		// check if Steam contains information in registry for the install path
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\WOW6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE,
		                  &reg_key) == ERROR_SUCCESS)
		{
#ifdef _DEBUG
			MSG_BOX_INFO("RegOpenKeyExA worked for steam");
			if (arxan::is_wine())
			{
				MSG_BOX_INFO("and it's on wine");
			}
#endif
			RegQueryValueExA(reg_key, "InstallPath", nullptr, nullptr, reinterpret_cast<BYTE*>(path),
			                 &length);
			RegCloseKey(reg_key);
#ifdef _DEBUG
			MSG_BOX_INFO(::utils::string::va("path is '%s'", path));
#endif
			install_path = path;
		}
		else
		{
#ifdef _DEBUG
			MSG_BOX_INFO("RegOpenKeyExA couldn't find Steam registry, continuing...");
#endif
			// if we can't find Steam in the registry, let's check if we are on Wine or not.
			// to add onto this, Steam has a Linux-specific build and it obviously doesn't register in the Wine registry. 
			// the above if statement *could* work if the user emulated Steam via Wine but pretty sure no one is gonna do that so... :P
			HKEY steam_install_reg;

			if (arxan::is_wine())
			{
#ifdef _DEBUG
				MSG_BOX_INFO("arxan::is_wine() is true, so we are continuing below!");
#endif
				// let's check the registry to see if the user has already manually selected the Steam installation path
				if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\h1-mod", 0, KEY_QUERY_VALUE, &steam_install_reg) 
					!= ERROR_SUCCESS)
				{
#ifdef _DEBUG
					MSG_BOX_INFO("creating key");
#endif
					// create a registry key
					if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\h1-mod", 0, nullptr, 0, KEY_WRITE, nullptr, &steam_install_reg, nullptr) 
						!= ERROR_SUCCESS)
					{
						MSG_BOX_ERROR("Could not create registry for Steam install path.");
						return "";
					}
#ifdef _DEBUG
					MSG_BOX_INFO("1");
#endif
					// create a pointer to our path variable to use in our open_folder function
					char* directory_ptr = path;

					// open a file explorer prompt to find the Steam directory (user input)
					open_folder_prompt(directory_ptr);
#ifdef _DEBUG
					MSG_BOX_INFO("2");
#endif
					while (!strcmp(directory_ptr, "")) // if this while statement goes, this means that the operation was cancelled
					{
#ifdef _DEBUG
						MSG_BOX_INFO("3");
#endif
						MSG_BOX_ERROR("You must select a valid Steam directory before you can continue.");
						open_folder_prompt(directory_ptr);
					}

#ifdef _DEBUG
					MSG_BOX_INFO("4");
#endif
					// if the directory pointer is defined, then we set "steam_install" inside "Software\\h1-mod" to the path
					if (RegSetKeyValueA(steam_install_reg, nullptr, "steam_install", REG_SZ, ::utils::string::va("\"%s\"", path), length) != ERROR_SUCCESS)
					{
						MSG_BOX_ERROR("Failed to set valid Steam install path in registry. Please try again.");
						return "";
					}
#ifdef _DEBUG
					MSG_BOX_INFO("5");
#endif
				}
#ifdef _DEBUG
				MSG_BOX_INFO("6");
#endif
				// query "steam_install" inside "Software\\h1-mod" and define it to our path variableand set install_path to path
				RegQueryValueExA(steam_install_reg, "steam_install", nullptr, nullptr, reinterpret_cast<BYTE*>(path), &length);

#ifdef MSG_BOX_INFO
				MSG_BOX_INFO(::utils::string::va("Path stored in registry is '%s'", path));
#endif

				install_path = path;
				RegCloseKey(steam_install_reg);
			}
			else
			{
				MSG_BOX_ERROR("Failed to find a Steam installation.");
			}
		}

		return install_path.data();
	}

	bool SteamGameServer_Init()
	{
		return true;
	}

	void SteamGameServer_RunCallbacks()
	{
	}

	void SteamGameServer_Shutdown()
	{
	}


	friends* SteamFriends()
	{
		static friends friends;
		return &friends;
	}

	matchmaking* SteamMatchmaking()
	{
		static matchmaking matchmaking;
		return &matchmaking;
	}

	game_server* SteamGameServer()
	{
		static game_server game_server;
		return &game_server;
	}

	networking* SteamNetworking()
	{
		static networking networking;
		return &networking;
	}

	remote_storage* SteamRemoteStorage()
	{
		static remote_storage remote_storage;
		return &remote_storage;
	}

	user* SteamUser()
	{
		static user user;
		return &user;
	}

	utils* SteamUtils()
	{
		static utils utils;
		return &utils;
	}

	apps* SteamApps()
	{
		static apps apps;
		return &apps;
	}

	user_stats* SteamUserStats()
	{
		static user_stats user_stats;
		return &user_stats;
	}

	}
}
