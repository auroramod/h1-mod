#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game_module.hpp"
#include "driver_profile.hpp"

#include <utils/hook.hpp>
#include <game/game.hpp>

static utils::hook::detour handle_a_hook;
static utils::hook::detour handle_w_hook;
static utils::hook::detour handle_ex_a_hook;
static utils::hook::detour handle_ex_w_hook;
static utils::hook::detour file_name_a_hook;
static utils::hook::detour file_name_w_hook;

static decltype(&GetModuleHandleA) orig_get_module_handle_a = &GetModuleHandleA;
static decltype(&GetModuleHandleW) orig_get_module_handle_w = &GetModuleHandleW;
static decltype(&GetModuleHandleExA) orig_get_module_handle_ex_a = &GetModuleHandleExA;
static decltype(&GetModuleHandleExW) orig_get_module_handle_ex_w = &GetModuleHandleExW;
static decltype(&GetModuleFileNameA) orig_get_module_file_name_a = &GetModuleFileNameA;
static decltype(&GetModuleFileNameW) orig_get_module_file_name_w = &GetModuleFileNameW;

void game_module::post_start()
{
	get_host_module();
}

void game_module::post_load()
{
#ifdef INJECT_HOST_AS_LIB
	hook_module_resolving();
#else
	assert(get_host_module() == get_game_module());
#endif
}

utils::nt::library game_module::get_game_module()
{
	static utils::nt::library game{HMODULE(game::base_address)};
	return game;
}

utils::nt::library game_module::get_host_module()
{
	static utils::nt::library host{};
	return host;
}

void game_module::hook_module_resolving()
{
	if (game::environment::is_sp())
	{
		// SP is still the 1.15 binary, so we keep global detours there
		handle_a_hook.create(&GetModuleHandleA, &get_module_handle_a);
		handle_w_hook.create(&GetModuleHandleW, &get_module_handle_w);
		handle_ex_a_hook.create(&GetModuleHandleExA, &get_module_handle_ex_a);
		handle_ex_w_hook.create(&GetModuleHandleExW, &get_module_handle_ex_w);
		file_name_a_hook.create(&GetModuleFileNameA, &get_module_file_name_a);
		file_name_w_hook.create(&GetModuleFileNameW, &get_module_file_name_w);

		orig_get_module_handle_a = handle_a_hook.get<std::remove_pointer_t<decltype(orig_get_module_handle_a)>>();
		orig_get_module_handle_w = handle_w_hook.get<std::remove_pointer_t<decltype(orig_get_module_handle_w)>>();
		orig_get_module_handle_ex_a = handle_ex_a_hook.get<std::remove_pointer_t<decltype(orig_get_module_handle_ex_a)>>();
		orig_get_module_handle_ex_w = handle_ex_w_hook.get<std::remove_pointer_t<decltype(orig_get_module_handle_ex_w)>>();
		orig_get_module_file_name_a = file_name_a_hook.get<std::remove_pointer_t<decltype(orig_get_module_file_name_a)>>();
		orig_get_module_file_name_w = file_name_w_hook.get<std::remove_pointer_t<decltype(orig_get_module_file_name_w)>>();
		return;
	}

	// patch the game's IAT only (GetModuleHandleExA is not imported by 1.04 mp)
	utils::hook::set(0x14080F4C0, get_module_handle_a);
	utils::hook::set(0x14080F5A0, get_module_handle_w);
	utils::hook::set(0x14080F230, get_module_handle_ex_w);
	utils::hook::set(0x14080F4B8, get_module_file_name_a);
	utils::hook::set(0x14080F5E8, get_module_file_name_w);
}

HMODULE __stdcall game_module::get_module_handle_a(const LPCSTR module_name)
{
	if (!module_name)
	{
		return get_game_module();
	}

	return orig_get_module_handle_a(module_name);
}

HMODULE __stdcall game_module::get_module_handle_w(const LPWSTR module_name)
{
	if (!module_name)
	{
		return get_game_module();
	}

	return orig_get_module_handle_w(module_name);
}

BOOL __stdcall game_module::get_module_handle_ex_a(const DWORD flags, const LPCSTR module_name, HMODULE* hmodule)
{
	if (!module_name)
	{
		*hmodule = get_game_module();
		return TRUE;
	}

	return orig_get_module_handle_ex_a(flags, module_name, hmodule);
}

BOOL __stdcall game_module::get_module_handle_ex_w(const DWORD flags, const LPCWSTR module_name, HMODULE* hmodule)
{
	if (!module_name)
	{
		*hmodule = get_game_module();
		return TRUE;
	}

	return orig_get_module_handle_ex_w(flags, module_name, hmodule);
}

DWORD __stdcall game_module::get_module_file_name_a(HMODULE hmodule, const LPSTR filename, const DWORD size)
{
	DWORD result{};
	if (driver_profile::get_module_file_name(_ReturnAddress(), hmodule, filename, size, &result))
	{
		return result;
	}

	if (!hmodule || utils::nt::library(hmodule) == get_game_module())
	{
		hmodule = get_host_module();
	}

	return orig_get_module_file_name_a(hmodule, filename, size);
}

DWORD __stdcall game_module::get_module_file_name_w(HMODULE hmodule, const LPWSTR filename, const DWORD size)
{
	DWORD result{};
	if (driver_profile::get_module_file_name(_ReturnAddress(), hmodule, filename, size, &result))
	{
		return result;
	}

	if (!hmodule || utils::nt::library(hmodule) == get_game_module())
	{
		hmodule = get_host_module();
	}

	return orig_get_module_file_name_w(hmodule, filename, size);
}

REGISTER_COMPONENT(game_module)
