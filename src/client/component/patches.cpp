#include <std_include.hpp>
#include "loader/component_loader.hpp"

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

namespace patches
{
	namespace
	{
		const char* live_get_local_client_name()
		{
			return game::Dvar_FindVar("name")->current.string;
		}

		utils::hook::detour sv_kick_client_num_hook;

		void sv_kick_client_num(const int client_num, const char* reason)
		{
			// Don't kick bot to equalize team balance.
			if (reason == "EXE_PLAYERKICKED_BOT_BALANCE"s)
			{
				return;
			}
			return sv_kick_client_num_hook.invoke<void>(client_num, reason);
		}

		std::string get_login_username()
		{
			char username[UNLEN + 1];
			DWORD username_len = UNLEN + 1;
			if (!GetUserNameA(username, &username_len))
			{
				return "Unknown Soldier";
			}

			return std::string{username, username_len - 1};
		}

		utils::hook::detour com_register_dvars_hook;

		void com_register_dvars_stub()
		{
			if (game::environment::is_mp())
			{
				// Make name save
				dvars::register_string("name", get_login_username().data(), game::DVAR_FLAG_SAVED, "Player name.");
			}

			return com_register_dvars_hook.invoke<void>();
		}

		utils::hook::detour cg_set_client_dvar_from_server_hook;

		void cg_set_client_dvar_from_server_stub(void* clientNum, void* cgameGlob, const char* dvar_hash, const char* value)
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
				game::Dvar_RegisterString(hash, "", value, game::DVAR_FLAG_EXTERNAL);
				return;
			}

			// only set if dvar has no flags or has cheat flag or has external flag
			if (dvar->flags == game::DVAR_FLAG_NONE || 
				(dvar->flags & game::DVAR_FLAG_CHEAT) != 0 || 
				(dvar->flags & game::DVAR_FLAG_EXTERNAL) != 0)
			{
				game::Dvar_SetFromStringFromSource(dvar, value, game::DvarSetSource::DVAR_SOURCE_EXTERNAL);
			}

			// original code
			int index = 0;
			auto result = utils::hook::invoke<bool>(0x4745E0_b, dvar, &index); // NetConstStrings_SV_GetNetworkDvarIndex
			if (result)
			{
				std::string index_str = std::to_string(index);
				return cg_set_client_dvar_from_server_hook.invoke<void>(clientNum, cgameGlob, index_str.data(), value);
			}
		}

		game::dvar_t* get_client_dvar(const char* name)
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

		bool get_client_dvar_hash(game::dvar_t* dvar, int* hash)
		{
			*hash = dvar->hash;
			return true;
		}

		const char* db_read_raw_file_stub(const char* filename, char* buf, const int size)
		{
			std::string file_name = filename;
			if (file_name.find(".cfg") == std::string::npos)
			{
				file_name.append(".cfg");
			}

			std::string buffer{};
			if (filesystem::read_file(file_name, &buffer))
			{
				snprintf(buf, size, "%s\n", buffer.data());
				return buf;
			}

			// DB_ReadRawFile
			return utils::hook::invoke<const char*>(SELECT_VALUE(0x1F4D00_b, 0x3994B0_b), filename, buf, size);
		}

		void bsp_sys_error_stub(const char* error, const char* arg1)
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

		utils::hook::detour cmd_lui_notify_server_hook;
		void cmd_lui_notify_server_stub(game::mp::gentity_s* ent)
		{
			const auto svs_clients = *game::mp::svs_clients;
			if (svs_clients == nullptr)
			{
				return;
			}

			command::params_sv params{};
			const auto menu_id = atoi(params.get(1));
			const auto client = &svs_clients[ent->s.number];

			// 13 => change class
			if (menu_id == 13 && ent->client->team == game::mp::TEAM_SPECTATOR)
			{
				return;
			}

			// 32 => "end_game"
			if (menu_id == 32 && client->header.remoteAddress.type != game::NA_LOOPBACK)
			{
				game::SV_DropClient_Internal(client, "PLATFORM_STEAM_KICK_CHEAT", true);
				return;
			}

			cmd_lui_notify_server_hook.invoke<void>(ent);
		}

		void sv_execute_client_message_stub(game::mp::client_t* client, game::msg_t* msg)
		{
			if ((client->reliableSequence - client->reliableAcknowledge) < 0)
			{
				client->reliableAcknowledge = client->reliableSequence;
				console::info("Negative reliableAcknowledge from %s - cl->reliableSequence is %i, reliableAcknowledge is %i\n",
					client->name, client->reliableSequence, client->reliableAcknowledge);
				network::send(client->header.remoteAddress, "error", "EXE_LOSTRELIABLECOMMANDS", '\n');
				return;
			}

			utils::hook::invoke<void>(0x54EC50_b, client, msg);
		}

		void aim_assist_add_to_target_list(void* aaGlob, void* screenTarget)
		{
			if (!dvars::aimassist_enabled->current.enabled)
			{
				return;
			}

			game::AimAssist_AddToTargetList(aaGlob, screenTarget);
		}

		void missing_content_error_stub(int, const char*)
		{
			game::Com_Error(game::ERR_DROP, utils::string::va("MISSING FILE\n%s.ff",
				fastfiles::get_current_fastfile().data()));
		}

		utils::hook::detour init_network_dvars_hook;
		void init_network_dvars_stub(game::dvar_t* dvar)
		{
			constexpr auto hash = dvars::generate_hash("r_tonemapHighlightRange");
			if (dvar->hash == hash)
			{
				init_network_dvars_hook.invoke<void>(dvar);
			}
		}

		int ui_draw_crosshair()
		{
			return 1;
		}

		utils::hook::detour cl_gamepad_scrolling_buttons_hook;
		void cl_gamepad_scrolling_buttons_stub(int local_client_num, int a2)
		{
			if (local_client_num <= 3)
			{
				cl_gamepad_scrolling_buttons_hook.invoke<void>(local_client_num, a2);
			}
		}

		void create_2d_texture_stub_1(const char* fmt, ...)
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

		void create_2d_texture_stub_2(game::errorParm code, const char* fmt, ...)
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

		void swap_chain_stub(game::errorParm code, const char* fmt, ...)
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
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Register dvars
			com_register_dvars_hook.create(SELECT_VALUE(0x385BE0_b, 0x15BB60_b), &com_register_dvars_stub);

			// Unlock fps in main menu
			utils::hook::set<BYTE>(SELECT_VALUE(0x1B1EAB_b, 0x34396B_b), 0xEB);

			if (!game::environment::is_dedi())
			{
				// Fix mouse lag
				utils::hook::nop(SELECT_VALUE(0x4631F9_b, 0x5BFF89_b), 6);
				scheduler::loop([]()
				{
					SetThreadExecutionState(ES_DISPLAY_REQUIRED);
				}, scheduler::pipeline::main);
			}

			// Set compassSize dvar minimum to 0.1
			dvars::override::register_float("compassSize", 1.0f, 0.1f, 50.0f, game::DVAR_FLAG_SAVED);

			// Make cg_fov and cg_fovscale saved dvars
			dvars::override::register_float("cg_fov", 65.f, 40.f, 200.f, game::DvarFlags::DVAR_FLAG_SAVED);
			dvars::override::register_float("cg_fovScale", 1.f, 0.1f, 2.f, game::DvarFlags::DVAR_FLAG_SAVED);
			dvars::override::register_float("cg_fovMin", 1.f, 1.0f, 90.f, game::DvarFlags::DVAR_FLAG_SAVED);

			// Enable Marketing Comms
			dvars::override::register_int("marketing_active", 1, 1, 1, game::DVAR_FLAG_WRITE);

			// Makes com_maxfps saved dvar
			dvars::override::register_int("com_maxfps", 0, 0, 1000, game::DVAR_FLAG_SAVED);

			// Makes mis_cheat saved dvar
			dvars::override::register_bool("mis_cheat", 0, game::DVAR_FLAG_SAVED);

			// Fix speaker config bug
			dvars::override::register_int("snd_detectedSpeakerConfig", 0, 0, 100, 0);

			// Allow kbam input when gamepad is enabled
			utils::hook::nop(SELECT_VALUE(0x1AC0CE_b, 0x135EFB_b), 2);
			utils::hook::nop(SELECT_VALUE(0x1A9DDC_b, 0x13388F_b), 6);

			// Show missing fastfiles
			utils::hook::call(SELECT_VALUE(0x1F588B_b, 0x39A78E_b), missing_content_error_stub);

			// Allow executing custom cfg files with the "exec" command
			utils::hook::call(SELECT_VALUE(0x376EB5_b, 0x156D41_b), db_read_raw_file_stub);

			// Remove useless information from errors + add additional help to common errors
			utils::hook::call(SELECT_VALUE(0x55E919_b, 0x681A69_b), create_2d_texture_stub_1); 	// Sys_Error for "Create2DTexture( %s, %i, %i, %i, %i ) failed"
			utils::hook::call(SELECT_VALUE(0x55EACB_b, 0x681C1B_b), create_2d_texture_stub_2); 	// Com_Error for ^
			utils::hook::call(SELECT_VALUE(0x5B35BA_b, 0x6CB1BC_b), swap_chain_stub); 			// Com_Error for "IDXGISwapChain::Present failed: %s"

			// Uncheat protect gamepad-related dvars
			dvars::override::register_float("gpad_button_deadzone", 0.13f, 0, 1, game::DVAR_FLAG_SAVED);
			dvars::override::register_float("gpad_stick_deadzone_min", 0.2f, 0, 1, game::DVAR_FLAG_SAVED);
			dvars::override::register_float("gpad_stick_deadzone_max", 0.01f, 0, 1, game::DVAR_FLAG_SAVED);
			dvars::override::register_float("gpad_stick_pressed", 0.4f, 0, 1, game::DVAR_FLAG_SAVED);
			dvars::override::register_float("gpad_stick_pressed_hysteresis", 0.1f, 0, 1, game::DVAR_FLAG_SAVED);

			if (!game::environment::is_sp())
			{
				patch_mp();
			}
		}

		static void patch_mp()
		{
			// fix vid_restart crash
			utils::hook::set<uint8_t>(0x139680_b, 0xC3);

			utils::hook::jump(0x5BB9C0_b, &live_get_local_client_name);

			// Disable data validation error popup
			dvars::override::register_int("data_validation_allow_drop", 0, 0, 0, game::DVAR_FLAG_NONE);

			// Patch SV_KickClientNum
			sv_kick_client_num_hook.create(game::SV_KickClientNum, &sv_kick_client_num);

			// block changing name in-game
			utils::hook::set<uint8_t>(0x54CFF0_b, 0xC3);

			// client side aim assist dvar
			dvars::aimassist_enabled = dvars::register_bool("aimassist_enabled", true,
				game::DvarFlags::DVAR_FLAG_SAVED,
				"Enables aim assist for controllers");
			utils::hook::call(0xE857F_b, aim_assist_add_to_target_list);

			// patch "Couldn't find the bsp for this map." error to not be fatal in mp
			utils::hook::call(0x39465B_b, bsp_sys_error_stub);

			// isProfanity
			utils::hook::set(0x361AA0_b, 0xC3C033);

			// disable elite_clan
			dvars::override::register_int("elite_clan_active", 0, 0, 0, game::DVAR_FLAG_NONE);
			utils::hook::set<uint8_t>(0x62D2F0_b, 0xC3); // don't register commands

			// disable codPointStore
			dvars::override::register_int("codPointStore_enabled", 0, 0, 0, game::DVAR_FLAG_NONE);

			// don't register every replicated dvar as a network dvar (only r_tonemapHighlightRange, fixes red dots)
			init_network_dvars_hook.create(0x4740C0_b, init_network_dvars_stub);

			// patch "Server is different version" to show the server client version
			utils::hook::inject(0x54DCE5_b, VERSION);

			// prevent servers overriding our fov
			utils::hook::nop(0x17DA96_b, 0x16);
			utils::hook::nop(0xE00BE_b, 0x17);
			utils::hook::nop(0x307F90_b, 0x5); // don't change cg_fov when toggling third person spectating

			// make setclientdvar behave like older games
			cg_set_client_dvar_from_server_hook.create(0x11AA90_b, cg_set_client_dvar_from_server_stub);
			utils::hook::call(0x407EC5_b, get_client_dvar_hash); // setclientdvar
			utils::hook::call(0x4087C1_b, get_client_dvar_hash); // setclientdvars
			utils::hook::call(0x407E8E_b, get_client_dvar); // setclientdvar
			utils::hook::call(0x40878A_b, get_client_dvar); // setclientdvars
			utils::hook::set<uint8_t>(0x407EB6_b, 0xEB); // setclientdvar
			utils::hook::set<uint8_t>(0x4087B2_b, 0xEB); // setclientdvars

			// some [data validation] anti tamper thing that kills performance
			dvars::override::register_int("dvl", 0, 0, 0, game::DVAR_FLAG_READ);

			// unlock safeArea_*
			utils::hook::jump(0x347BC5_b, 0x347BD3_b);
			utils::hook::jump(0x347BEC_b, 0x347C17_b);
			dvars::override::register_float("safeArea_adjusted_horizontal", 1, 0, 1, game::DVAR_FLAG_SAVED);
			dvars::override::register_float("safeArea_adjusted_vertical", 1, 0, 1, game::DVAR_FLAG_SAVED);
			dvars::override::register_float("safeArea_horizontal", 1, 0, 1, game::DVAR_FLAG_SAVED);
			dvars::override::register_float("safeArea_vertical", 1, 0, 1, game::DVAR_FLAG_SAVED);

			// allow servers to check for new packages more often
			dvars::override::register_int("sv_network_fps", 1000, 20, 1000, game::DVAR_FLAG_SAVED);

			// Massively increate timeouts
			dvars::override::register_int("cl_timeout", 90, 90, 1800, game::DVAR_FLAG_NONE); // Seems unused
			dvars::override::register_int("sv_timeout", 90, 90, 1800, game::DVAR_FLAG_NONE); // 30 - 0 - 1800
			dvars::override::register_int("cl_connectTimeout", 120, 120, 1800, game::DVAR_FLAG_NONE); // Seems unused
			dvars::override::register_int("sv_connectTimeout", 120, 120, 1800, game::DVAR_FLAG_NONE); // 60 - 0 - 1800

			dvars::register_int("scr_game_spectatetype", 1, 0, 99, game::DVAR_FLAG_REPLICATED, "");

			dvars::override::register_bool("ui_drawCrosshair", true, game::DVAR_FLAG_WRITE);
			utils::hook::jump(0x1E6010_b, ui_draw_crosshair);

			// Prevent clients from ending the game as non host by sending 'end_game' lui notification
			cmd_lui_notify_server_hook.create(0x412D50_b, cmd_lui_notify_server_stub);

			// Prevent clients from sending invalid reliableAcknowledge
			utils::hook::call(0x1CBD06_b, sv_execute_client_message_stub);

			// Change default hostname and make it replicated
			dvars::override::register_string("sv_hostname", "^2H1-Mod^7 Default Server", game::DVAR_FLAG_REPLICATED);

			// Dont free server/client memory on asset loading (fixes crashing on map rotation)
			utils::hook::nop(0x132474_b, 5);

			// Fix gamepad related crash
			cl_gamepad_scrolling_buttons_hook.create(0x133210_b, cl_gamepad_scrolling_buttons_stub);

			// Prevent the game from modifying Windows microphone volume (since voice chat isn't used)
			utils::hook::set<uint8_t>(0x5BEEA0_b, 0xC3); // Mixer_SetWaveInRecordLevels

			utils::hook::set<uint8_t>(0x556250_b, 0xC3); // disable host migration
		}
	};
}

REGISTER_COMPONENT(patches::component)
