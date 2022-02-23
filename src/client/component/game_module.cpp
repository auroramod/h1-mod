#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game_module.hpp"

#include <utils/hook.hpp>

namespace game_module
{
	namespace
	{
		utils::hook::detour handle_a_hook;
		utils::hook::detour handle_w_hook;
		utils::hook::detour handle_ex_a_hook;
		utils::hook::detour handle_ex_w_hook;
		utils::hook::detour file_name_a_hook;
		utils::hook::detour file_name_w_hook;

		HMODULE __stdcall get_module_handle_a(const LPCSTR module_name)
		{
			if (!module_name)
			{
				return get_game_module();
			}

			return handle_a_hook.invoke<HMODULE>(module_name);
		}

		HMODULE __stdcall get_module_handle_w(const LPWSTR module_name)
		{
			if (!module_name)
			{
				return get_game_module();
			}

			return handle_w_hook.invoke<HMODULE>(module_name);
		}

		BOOL __stdcall get_module_handle_ex_a(const DWORD flags, const LPCSTR module_name, HMODULE* hmodule)
		{
			if (!module_name)
			{
				*hmodule = get_game_module();
				return TRUE;
			}

			return handle_ex_a_hook.invoke<BOOL>(flags, module_name, hmodule);
		}

		BOOL __stdcall get_module_handle_ex_w(const DWORD flags, const LPCWSTR module_name, HMODULE* hmodule)
		{
			if (!module_name)
			{
				*hmodule = get_game_module();
				return TRUE;
			}

			return handle_ex_w_hook.invoke<BOOL>(flags, module_name, hmodule);
		}

		DWORD __stdcall get_module_file_name_a(HMODULE hmodule, const LPSTR filename, const DWORD size)
		{
			if (!hmodule)
			{
				hmodule = get_game_module();
			}

			return file_name_a_hook.invoke<DWORD>(hmodule, filename, size);
		}

		DWORD __stdcall get_module_file_name_w(HMODULE hmodule, const LPWSTR filename, const DWORD size)
		{
			if (!hmodule)
			{
				hmodule = get_game_module();
			}

			return file_name_w_hook.invoke<DWORD>(hmodule, filename, size);
		}

		void hook_module_resolving()
		{
			handle_a_hook.create(&GetModuleHandleA, &get_module_handle_a);
			handle_w_hook.create(&GetModuleHandleW, &get_module_handle_w);
			handle_ex_w_hook.create(&GetModuleHandleExA, &get_module_handle_ex_a);
			handle_ex_w_hook.create(&GetModuleHandleExW, &get_module_handle_ex_w);
			file_name_a_hook.create(&GetModuleFileNameA, &get_module_file_name_a);
			file_name_w_hook.create(&GetModuleFileNameW, &get_module_file_name_w);
		}
	}

	utils::nt::library get_game_module()
	{
		static utils::nt::library game{HMODULE(0x140000000)};
		return game;
	}

	utils::nt::library get_host_module()
	{
		static utils::nt::library host{};
		return host;
	}

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			get_host_module();
		}

		void post_load() override
		{
			hook_module_resolving();
		}
	};
}

REGISTER_COMPONENT(game_module::component)
