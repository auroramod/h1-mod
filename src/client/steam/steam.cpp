#include <std_include.hpp>
#include "steam.hpp"

#include "component/arxan.hpp"
#include "component/console.hpp"

#include <utils/nt.hpp>
#include <utils/string.hpp>

#define MSG_BOX_ERROR(message) MessageBoxA(nullptr, message, "ERROR", MB_ICONERROR);

namespace steam
{
	namespace
	{
		std::string open_folder(HWND owner)
		{
			std::string temp_path{};

			IFileOpenDialog* dialog;
			HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));

			if (FAILED(result))
				return "";

			DWORD dw_options;
			if (SUCCEEDED(result = dialog->GetOptions(&dw_options)))
			{
				result = dialog->SetOptions(dw_options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);

				if (SUCCEEDED(result))
				{
					dialog->SetTitle(L"Select Steam installation path (should have steamclient64.dll in it)");
				}

				result = dialog->Show(nullptr);

				if (SUCCEEDED(result))
				{
					IShellItem* shell_item_result;
					result = dialog->GetResult(&shell_item_result);

					LPWSTR folder = 0;
					shell_item_result->GetDisplayName(SIGDN_FILESYSPATH, &folder);
					char path[MAX_PATH] = {0};
					wcstombs(path, folder, MAX_PATH);
					temp_path = path;

					if (folder)
					{
						delete[] folder;
					}

					CoTaskMemFree(folder);
					shell_item_result->Release();
				}
			}

			dialog->Release();

			return temp_path;
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
		static std::string install_path{};
		if (!install_path.empty())
		{
			return install_path.data();
		}

#ifndef _DEBUG
		HKEY reg_key;

		// check if Steam contains information in registry for the install path
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\WOW6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE,
		                  &reg_key) == ERROR_SUCCESS)
		{
			char path[MAX_PATH] = {0};
			DWORD length = sizeof(path);
			RegQueryValueExA(reg_key, "InstallPath", nullptr, nullptr, reinterpret_cast<BYTE*>(path),
			                 &length);
			RegCloseKey(reg_key);

			install_path = path;
		}
		else
		{
#endif
			// if we can't find Steam in the registry, let's check if we are on Wine or not.
			// to add onto this, Steam has a Linux-specific build and it obviously doesn't register in the Wine registry. 
			// the above if statement *could* work if the user emulated Steam via Wine but who knows
			HKEY steam_install_reg;
			
#ifndef _DEBUG
			if (arxan::is_wine())
#endif
			{
				char path[MAX_PATH] = {};
				DWORD length = sizeof(path);

				// let's check the registry to see if the user has already manually selected the Steam installation path
				auto result = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\h1-mod", 0, KEY_QUERY_VALUE, &steam_install_reg);
				if (result != ERROR_SUCCESS)
				{
					MSG_BOX_ERROR("Could not find a pre-existing key in 'Software\\h1-mod' registry, creating...\n");

					result = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\h1-mod", 0, nullptr, 0, KEY_WRITE, nullptr, &steam_install_reg, nullptr);
					if (result != ERROR_SUCCESS)
					{
						MSG_BOX_ERROR("Could not create registry for Steam install path.");
						return install_path.data();
					}

					auto temp_path = open_folder(nullptr);
					MSG_BOX_ERROR(::utils::string::va("the path is: '%s'", temp_path.data()));
					while (temp_path != "")
					{
						temp_path = open_folder(nullptr);
					}

					strcpy(path, temp_path.data());

					result = RegSetKeyValueA(steam_install_reg, nullptr, "steam_install", REG_SZ, path, length);
					if (result != ERROR_SUCCESS)
					{
						MSG_BOX_ERROR("Could not set value in registry for Steam install path.");
						return install_path.data();
					}
				}

				RegQueryValueExA(steam_install_reg, "steam_install", nullptr, nullptr, reinterpret_cast<BYTE*>(path), &length);

				install_path = path;

				RegCloseKey(steam_install_reg);

				// use SHBrowseForFolderA?
			}
#ifndef _DEBUG
		}
#endif

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