#include <std_include.hpp>
#include "patches.hpp"

#include "dvars.hpp"
#include "fastfiles.hpp"
#include "version.h"
#include "command.hpp"
#include "console.hpp"
#include "network.hpp"
#include "scheduler.hpp"
#include "filesystem.hpp"
#include "menus.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>

static utils::hook::detour sv_kick_client_num_hook;
static utils::hook::detour com_register_dvars_hook;
static utils::hook::detour db_read_raw_file_hook;
static utils::hook::detour sv_shutdown_hook;
static utils::hook::detour com_quit_f_hook;

void patches::post_unpack()
{
	// Register dvars
	com_register_dvars_hook.create(SELECT_VALUE(0x140385BE0, 0x1400D9320), &com_register_dvars_stub); // Com_InitDvars

	// Unlock fps in main menu
	utils::hook::set<BYTE>(SELECT_VALUE(0x1401B1EAB, 0x14025B86B), 0xEB);

	if (!game::environment::is_dedi())
	{
		// Fix mouse lag
		utils::hook::nop(SELECT_VALUE(0x1404631F9, 0x1405162F9), 6);
		scheduler::loop([]()
		{
			SetThreadExecutionState(ES_DISPLAY_REQUIRED);
		}, scheduler::pipeline::main);
	}

	// Set compassSize dvar minimum to 0.1
	dvars_component::override::register_float("compassSize", 1.0f, 0.1f, 50.0f, game::DVAR_ARCHIVE);

	// Make cg_fov and cg_fovscale saved dvars
	dvars_component::override::register_float("cg_fov", 65.f, 40.f, 200.f, game::DvarFlags::DVAR_ARCHIVE);
	dvars_component::override::register_float("cg_fovScale", 1.f, 0.1f, 2.f, game::DvarFlags::DVAR_ARCHIVE);
	dvars_component::override::register_float("cg_fovMin", 1.f, 1.0f, 90.f, game::DvarFlags::DVAR_ARCHIVE);

	// Enable Marketing Comms
	dvars_component::override::register_int("marketing_active", 1, 1, 1, game::DVAR_ROM);

	// Makes com_maxfps saved dvar
	if (game::environment::is_dedi())
	{
		dvars_component::override::register_int("com_maxfps", 85, 0, 100, game::DVAR_NOFLAG);
	}
	else
	{
		dvars_component::override::register_int("com_maxfps", 0, 0, 1000, game::DVAR_ARCHIVE);
	}

	// Makes mis_cheat saved dvar
	dvars_component::override::register_bool("mis_cheat", false, game::DVAR_ARCHIVE);

	// Fix speaker config bug
	dvars_component::override::register_int("snd_detectedSpeakerConfig", 0, 0, 100, 0);

	// Allow kbam input when gamepad is enabled
	utils::hook::nop(SELECT_VALUE(0x1401AC0CE, 0x14024EF60), 2);
	utils::hook::nop(SELECT_VALUE(0x1401A9DDC, 0x14024C6B0), 6);

	// Show missing fastfiles
	utils::hook::call(SELECT_VALUE(0x1401F588B, 0x1402C0177), missing_content_error_stub);

	// Allow loading of rawfiles from disk
	db_read_raw_file_hook.create(game::DB_ReadRawFile, db_read_raw_file_stub);

	// Remove useless information from errors + add additional help to common errors
	utils::hook::call(SELECT_VALUE(0x14055E919, 0x1405DB25C), create_2d_texture_stub_1); // Sys_Error for "Create2DTexture( %s, %i, %i, %i, %i ) failed"
	utils::hook::call(SELECT_VALUE(0x14055EACB, 0x1405DB3E4), create_2d_texture_stub_2); // Com_Error for ^
	utils::hook::call(SELECT_VALUE(0x1405B35BA, 0x140622355), swap_chain_stub); // Com_Error for "IDXGISwapChain::Present failed: %s"

	// Uncheat protect gamepad-related dvars
	dvars_component::override::register_float("gpad_button_deadzone", 0.13f, 0, 1, game::DVAR_ARCHIVE);
	dvars_component::override::register_float("gpad_stick_deadzone_min", 0.2f, 0, 1, game::DVAR_ARCHIVE);
	dvars_component::override::register_float("gpad_stick_deadzone_max", 0.01f, 0, 1, game::DVAR_ARCHIVE);
	dvars_component::override::register_float("gpad_stick_pressed", 0.4f, 0, 1, game::DVAR_ARCHIVE);
	dvars_component::override::register_float("gpad_stick_pressed_hysteresis", 0.1f, 0, 1, game::DVAR_ARCHIVE);

	// Fix 'out of memory' error
	if (game::environment::is_sp())
	{
		utils::hook::call(0x140386664, sub_157FA0_stub);
	}
	else
	{
		utils::hook::call(0x1400D9F53, register_bool_stub);
		utils::hook::jump(0x1400D9F5F, 0x1400D9FC7);
	}

	if (!game::environment::is_sp())
	{
		patch_mp();
	}
}

void patches::patch_mp()
{
	// fix vid_restart crash
	utils::hook::set<uint8_t>(0x140255AE0, 0xC3);

	const std::uint8_t anim_items_limit[] = {0xC7, 0x07, 0x45, 0x00, 0x00, 0x00};
	utils::hook::copy(0x1401CB6F0, anim_items_limit, sizeof(anim_items_limit));
	utils::hook::nop(0x1401CB6F6, 11);

	// Use name dvar
	utils::hook::jump(0x14050FF90, live_get_local_client_name);

	// Disable data validation error popup
	dvars_component::override::register_int("data_validation_allow_drop", 0, 0, 0, game::DVAR_NOFLAG);

	// Patch SV_KickClientNum
	sv_kick_client_num_hook.create(game::SV_KickClientNum, &sv_kick_client_num);

	// block changing name in-game
	utils::hook::set<uint8_t>(0x14047FC90, 0xC3);

	// client side aim assist dvar
	dvars::aimassist_enabled = dvars::register_bool("aimassist_enabled", true,
		game::DvarFlags::DVAR_ARCHIVE,
		"Enables aim assist for controllers");
	utils::hook::call(0x14009EE9E, aim_assist_add_to_target_list);

	// patch "Couldn't find the bsp for this map." error to not be fatal in mp
	utils::hook::call(0x1402BA26B, bsp_sys_error_stub);

	// isProfanity
	utils::hook::set(0x1402877D0, 0xC3C033);

	// disable elite_clan
	dvars_component::override::register_int("elite_clan_active", 0, 0, 0, game::DVAR_NOFLAG);
	utils::hook::set<uint8_t>(0x140585680, 0xC3); // don't register commands

	// disable codPointStore
	dvars_component::override::register_int("codPointStore_enabled", 0, 0, 0, game::DVAR_NOFLAG);

	// don't register every replicated dvar as a network dvar (only r_tonemapHighlightRange, fixes red dots)
	utils::hook::call(0x14039E58E, init_network_dvars_stub);

	// patch "Server is different version" to show the server client version
	utils::hook::inject(0x140480955, VERSION);

	// prevent servers overriding our fov
	utils::hook::nop(0x1400DAF69, 5);
	utils::hook::nop(0x140190C16, 5);
	// The spectator FOV setter is a tail jump after stack cleanup. Return here
	// to preserve the player's FOV without falling through into a second cleanup.
	utils::hook::set<uint8_t>(0x14021D260, 0xC3); // don't change cg_fov when toggling third person spectating

	// make setclientdvar behave like older games
	utils::hook::call(0x14023279E, cg_set_client_dvar_from_server_stub);
	utils::hook::call(0x14032AAC3, get_client_dvar_hash); // setclientdvar
	utils::hook::call(0x14032B350, get_client_dvar_hash); // setclientdvars
	utils::hook::call(0x14032AA8C, get_client_dvar); // setclientdvar
	utils::hook::call(0x14032B318, get_client_dvar); // setclientdvars
	utils::hook::set<uint8_t>(0x14032AAB4, 0xEB); // setclientdvar
	utils::hook::set<uint8_t>(0x14032B341, 0xEB); // setclientdvars

	// some [data validation] anti tamper thing that kills performance
	dvars_component::override::register_int("dvl", 0, 0, 0, game::DVAR_INIT);

	// unlock safeArea_*
	utils::hook::jump(0x1402624F5, 0x140262503);
	utils::hook::jump(0x14026251C, 0x140262547);
	dvars_component::override::register_float("safeArea_adjusted_horizontal", 1, 0, 1, game::DVAR_ARCHIVE);
	dvars_component::override::register_float("safeArea_adjusted_vertical", 1, 0, 1, game::DVAR_ARCHIVE);
	dvars_component::override::register_float("safeArea_horizontal", 1, 0, 1, game::DVAR_ARCHIVE);
	dvars_component::override::register_float("safeArea_vertical", 1, 0, 1, game::DVAR_ARCHIVE);

	// allow servers to check for new packages more often
	dvars_component::override::register_int("sv_network_fps", 1000, 20, 1000, game::DVAR_ARCHIVE);

	// Massively increate timeouts
	dvars_component::override::register_int("cl_timeout", 90, 90, 1800, game::DVAR_NOFLAG); // Seems unused
	dvars_component::override::register_int("sv_timeout", 90, 90, 1800, game::DVAR_NOFLAG); // 30 - 0 - 1800
	dvars_component::override::register_int("cl_connectTimeout", 120, 120, 1800, game::DVAR_NOFLAG); // Seems unused
	dvars_component::override::register_int("sv_connectTimeout", 120, 120, 1800, game::DVAR_NOFLAG); // 60 - 0 - 1800

	dvars::register_int("scr_game_spectatetype", 1, 0, 99, game::DVAR_CODINFO, "");

	dvars_component::override::register_bool("ui_drawCrosshair", true, game::DVAR_ROM);
	utils::hook::jump(0x1404D1E50, ui_draw_crosshair);

	// Prevent clients from ending the game as non host by sending 'end_game' lui notification
	utils::hook::call(0x14033615D, cmd_lui_notify_server_stub);

	// Prevent clients from sending invalid reliableAcknowledge
	utils::hook::call(0x1404899C6, sv_execute_client_message_stub);

	// Change default hostname and make it replicated
	dvars_component::override::register_string("sv_hostname", "^2H1-Mod^7 Default Server", game::DVAR_CODINFO);

	// TODO(new): "Dont free server/client memory on asset loading (fixes crashing on map rotation)"
	// nopped the first call in 1.15 sub_132470 (called from DB_LoadXAssets). 1.04 DB_LoadXAssets has no
	// equivalent helper (it calls SV_BotFreeSystemMemory directly). Check if the map rotation crash exists on 1.04.

	// Fix gamepad related crash (1.15 sub_133210 is inlined into 1.04 sub_14024A360, only caller here)
	utils::hook::call(0x1402536EB, cl_gamepad_scrolling_buttons_stub);

	// Prevent the game from modifying Windows microphone volume (since voice chat isn't used)
	utils::hook::set<uint8_t>(0x140515250, 0xC3); // Mixer_SetWaveInRecordLevels

	utils::hook::set<uint8_t>(0x14048B660, 0xC3); // disable host migration

	// Re-implement dev prints
	com_quit_f_hook.create(0x1400DA640, com_quit_f_stub);
	sv_shutdown_hook.create(0x140486C40, sv_shutdown_stub); // SV_Shutdown

	// Allow using unauthorized clantags
	utils::hook::set<uint32_t>(0x1404E6D50, 0x90C301B0); // UI_ActivisionClanTagAllowedForGamerTag

	// 1.15 sub_12C5B0 (snapshot/omnvar memory size) was hooked to return 0x10 times more. 1.04 has no dynamic sizing function for this
}

const char* patches::live_get_local_client_name()
{
	return game::Dvar_FindVar("name")->current.string;
}

void patches::sv_kick_client_num(const int client_num, const char* reason)
{
	// Don't kick bot to equalize team balance.
	if (reason == "EXE_PLAYERKICKED_BOT_BALANCE"s)
	{
		return;
	}
	return sv_kick_client_num_hook.invoke<void>(client_num, reason);
}

std::string patches::get_login_username()
{
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	if (!GetUserNameA(username, &username_len))
	{
		return "Unknown Soldier";
	}

	return std::string{username, username_len - 1};
}

void patches::com_register_dvars_stub()
{
	if (game::environment::is_mp())
	{
		// Make name save
		dvars::register_string("name", get_login_username().data(), game::DVAR_ARCHIVE, "Player name.");
	}

	return com_register_dvars_hook.invoke<void>();
}

void patches::cg_set_client_dvar_from_server_stub(void* client_num, void* cgame_glob, const char* dvar_hash, const char* value)
{
	const auto hash = std::atoi(dvar_hash);
	auto* dvar = game::Dvar_FindMalleableVar(hash);

	if (hash == game::generateHashValue("cg_fov") ||
		hash == game::generateHashValue("cg_fovMin") ||
		hash == game::generateHashValue("cg_fovScale"))
	{
		return;
	}

	if (hash == game::generateHashValue("g_scriptMainMenu"))
	{
		menus::set_script_main_menu(value);
	}

	// register new dvar
	if (!dvar)
	{
		game::Dvar_RegisterString(hash, "", value, game::DVAR_EXTERNAL);
		return;
	}

	// only set if dvar has no flags or has cheat flag or has external flag
	if (dvar->flags == game::DVAR_NOFLAG ||
		(dvar->flags & game::DVAR_CHEAT) != 0 ||
		(dvar->flags & game::DVAR_EXTERNAL) != 0)
	{
		game::Dvar_SetFromStringFromSource(dvar, value, game::DvarSetSource::DVAR_SOURCE_EXTERNAL);
	}

	// original code
	int index = 0;
	auto result = utils::hook::invoke<bool>(0x14039EAE0, dvar, &index); // NetConstStrings_SV_GetNetworkDvarIndex
	if (result)
	{
		std::string index_str = std::to_string(index);
		utils::hook::invoke<void>(0x140236120, client_num, cgame_glob, index_str.data(), value); // CG_SetClientDvarFromServer
	}
}

game::dvar_t* patches::get_client_dvar(const char* name)
{
	game::dvar_t* dvar = game::Dvar_FindVar(name);
	if (!dvar)
	{
		static game::dvar_t dummy{0};
		dummy.hash = game::generateHashValue(name);
		return &dummy;
	}
	return dvar;
}

bool patches::get_client_dvar_hash(game::dvar_t* dvar, int* hash)
{
	*hash = dvar->hash;
	return true;
}

const char* patches::db_read_raw_file_stub(const char* filename, char* buf, const int size)
{
	std::string buffer{};
	if (filesystem::read_file(filename, &buffer))
	{
		snprintf(buf, size, "%s\n", buffer.data());
		return buf;
	}

	// DB_ReadRawFile
	return db_read_raw_file_hook.invoke<const char*>(filename, buf, size);
}

void patches::bsp_sys_error_stub(const char* error, const char* arg1)
{
	if (game::environment::is_dedi())
	{
		game::Sys_Error(error, arg1);
	}
	else
	{
		scheduler::once([]()
		{
			command::execute("reconnect");
		}, scheduler::pipeline::main, 1s);
		game::Com_Error(game::ERR_DROP, error, arg1);
	}
}

void patches::cmd_lui_notify_server_stub(game::gentity_s* ent)
{
	const auto svs_clients = game::mp::svs_clients.get();
	if (svs_clients == nullptr)
	{
		return;
	}

	command::params_sv params{};
	const auto menu_id = atoi(params.get(1));
	const auto client = &svs_clients[ent->s.number];

	// 13 => change class
	if (menu_id == 13 && ent->client->sess.team == game::TEAM_SPECTATOR)
	{
		return;
	}

	// 32 => "end_game"
	if (menu_id == 32 && client->header.remoteAddress.type != game::NA_LOOPBACK)
	{
		game::SV_DropClient_Internal(client, "PLATFORM_STEAM_KICK_CHEAT", true);
		return;
	}

	utils::hook::invoke<void>(0x140335A70, ent); // Cmd_LuiNotifyServer_f
}

void patches::sv_execute_client_message_stub(game::client_t* client, game::msg_t* msg)
{
	if ((client->reliableSequence - client->reliableAcknowledge) < 0)
	{
		client->reliableAcknowledge = client->reliableSequence;
		console::info("Negative reliableAcknowledge from %s - cl->reliableSequence is %i, reliableAcknowledge is %i\n",
			client->name, client->reliableSequence, client->reliableAcknowledge);
		network::send(client->header.remoteAddress, "error", "EXE_LOSTRELIABLECOMMANDS", '\n');
		return;
	}

	utils::hook::invoke<void>(0x140481A00, client, msg); // SV_ExecuteClientMessage
}

void patches::aim_assist_add_to_target_list(void* aa_glob, void* screen_target)
{
	if (!dvars::aimassist_enabled->current.enabled)
	{
		return;
	}

	game::AimAssist_AddToTargetList(aa_glob, screen_target);
}

void patches::missing_content_error_stub(int, const char*)
{
	game::Com_Error(game::ERR_DROP, utils::string::va("MISSING FILE\n%s.ff",
		fastfiles::get_current_fastfile().data()));
}

void patches::init_network_dvars_stub(game::dvar_t* /*dvar*/)
{
	static const auto* r_tonemapHighlightRange = game::Dvar_FindVar("r_tonemapHighlightRange");
	if (r_tonemapHighlightRange)
	{
		// NetConstStrings_InitNetworkDvars_Callback
		utils::hook::invoke<void>(0x14039E5C0, r_tonemapHighlightRange);
	}
}

int patches::ui_draw_crosshair()
{
	// The scoreboard and match-end UI temporarily disable this setting.
	const auto* draw_crosshair = game::Dvar_FindVar("ui_drawCrosshair");
	return !draw_crosshair || draw_crosshair->current.enabled;
}

void patches::create_2d_texture_stub_1(const char* fmt, ...)
{
	fmt = "Create2DTexture( %s, %i, %i, %i, %i ) failed\n\n"
		"Disable shader caching, lower graphic settings, free up RAM, or update your GPU drivers.";

	char buffer[2048];

	{
		va_list ap;
		va_start(ap, fmt);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, ap);

		va_end(ap);
	}

	game::Sys_Error("%s", buffer);
}

void patches::create_2d_texture_stub_2(game::errorParm code, const char* fmt, ...)
{
	fmt = "Create2DTexture( %s, %i, %i, %i, %i ) failed\n\n"
		"Disable shader caching, lower graphic settings, free up RAM, or update your GPU drivers.";

	char buffer[2048];

	{
		va_list ap;
		va_start(ap, fmt);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, ap);

		va_end(ap);
	}

	game::Com_Error(code, "%s", buffer);
}

void patches::swap_chain_stub(game::errorParm code, const char* fmt, ...)
{
	fmt = "IDXGISwapChain::Present failed: %s\n\n"
		"Disable shader caching, lower graphic settings, free up RAM, or update your GPU drivers.";

	char buffer[2048];

	{
		va_list ap;
		va_start(ap, fmt);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, ap);

		va_end(ap);
	}

	game::Com_Error(code, "%s", buffer);
}

void patches::dvar_set_bool(game::dvar_t* dvar, const bool value)
{
	game::dvar_value dvar_value{};
	dvar_value.enabled = value;
	game::Dvar_SetVariant(dvar, &dvar_value, game::DVAR_SOURCE_INTERNAL);
}

void patches::cl_gamepad_scrolling_buttons_stub(int local_client_num, int controller_index)
{
	if (local_client_num <= 3)
	{
		utils::hook::invoke<void>(0x14024A360, local_client_num, controller_index);
	}
}

// SP only
void patches::sub_157FA0_stub()
{
	const auto dvar_706663C2 = *reinterpret_cast<game::dvar_t**>(0x14B5C5338);
	const auto dvar_617FB3B4 = *reinterpret_cast<game::dvar_t**>(0x14B5C5340);

	if (!dvar_706663C2->current.enabled || utils::hook::invoke<bool>(0x140385A30))
	{
		utils::hook::invoke<void>(0x1403A5E90, 0, 0);
		dvar_set_bool(dvar_706663C2, true);
		dvar_set_bool(dvar_617FB3B4, true);
	}

	if (utils::hook::invoke<bool>(0x140439B50))
	{
		utils::hook::invoke<void>(0x1403A5E90, 0, 0);
		dvar_set_bool(dvar_617FB3B4, true);
	}
}

// MP only
game::dvar_t* patches::register_bool_stub(const int hash, __int64 /*name*/, const bool value, const unsigned int flags)
{
	const auto com_recommended_set = game::Dvar_RegisterBool(hash, "", value, flags);

	if (!com_recommended_set->current.enabled || utils::hook::invoke<bool>(0x1400D8A90))
	{
		utils::hook::invoke<void>(0x1400DADA0, 0, 0);
		dvar_set_bool(com_recommended_set, true);
	}

	if (utils::hook::invoke<bool>(0x14050C290))
	{
		utils::hook::invoke<void>(0x1400DADA0, 0, 0);
	}

	return com_recommended_set;
}

void patches::sv_shutdown_stub(const char* finalmsg)
{
	console::info("----- Server Shutdown -----\n");
	sv_shutdown_hook.invoke<void>(finalmsg);
}

void patches::com_quit_f_stub()
{
	console::info("quitting...\n");
	com_quit_f_hook.invoke<void>();
}

REGISTER_COMPONENT(patches)
