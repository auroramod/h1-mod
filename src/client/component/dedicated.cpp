#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "server_list.hpp"
#include "network.hpp"
#include "command.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"
#include "dvars.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace dedicated
{
	namespace
	{
		utils::hook::detour gscr_set_dynamic_dvar_hook;
		utils::hook::detour com_quit_f_hook;

		void init_dedicated_server()
		{
			static bool initialized = false;
			if (initialized) return;
			initialized = true;

			// R_LoadGraphicsAssets
			utils::hook::invoke<void>(0x1405DF4B0);
		}

		void send_heartbeat()
		{
			auto* const dvar = game::Dvar_FindVar("sv_lanOnly");
			if (dvar && dvar->current.enabled)
			{
				return;
			}

			game::netadr_s target{};
			if (server_list::get_master_server(target))
			{
				network::send(target, "heartbeat", "H1");
			}
		}

		std::vector<std::string>& get_startup_command_queue()
		{
			static std::vector<std::string> startup_command_queue;
			return startup_command_queue;
		}

		void execute_startup_command(int client, int /*controllerIndex*/, const char* command)
		{
			if (game::Live_SyncOnlineDataFlags(0) == 0)
			{
				game::Cbuf_ExecuteBufferInternal(0, 0, command, game::Cmd_ExecuteSingleCommand);
			}
			else
			{
				get_startup_command_queue().emplace_back(command);
			}
		}

		void execute_startup_command_queue()
		{
			const auto queue = get_startup_command_queue();
			get_startup_command_queue().clear();

			for (const auto& command : queue)
			{
				game::Cbuf_ExecuteBufferInternal(0, 0, command.data(), game::Cmd_ExecuteSingleCommand);
			}
		}

		std::vector<std::string>& get_console_command_queue()
		{
			static std::vector<std::string> console_command_queue;
			return console_command_queue;
		}

		void execute_console_command(const int client, const char* command)
		{
			if (game::Live_SyncOnlineDataFlags(0) == 0)
			{
				game::Cbuf_AddText(client, command);
				game::Cbuf_AddText(client, "\n");
			}
			else
			{
				get_console_command_queue().emplace_back(command);
			}
		}

		void execute_console_command_queue()
		{
			const auto queue = get_console_command_queue();
			get_console_command_queue().clear();

			for (const auto& command : queue)
			{
				game::Cbuf_AddText(0, command.data());
				game::Cbuf_AddText(0, "\n");
			}
		}

		void sync_gpu_stub()
		{
			std::this_thread::sleep_for(1ms);
		}

		game::dvar_t* gscr_set_dynamic_dvar()
		{
			/*
			auto s = game::Scr_GetString(0);
			auto* dvar = game::Dvar_FindVar(s);

			if (dvar && !strncmp("scr_", dvar->name, 4))
			{
				return dvar;
			}
			*/

			return gscr_set_dynamic_dvar_hook.invoke<game::dvar_t*>();
		}

		void kill_server()
		{
			for (auto i = 0; i < *game::mp::svs_numclients; ++i)
			{
				if (game::mp::svs_clients[i].header.state >= 3)
				{
					game::SV_GameSendServerCommand(i, game::SV_CMD_CAN_IGNORE,
					                               utils::string::va("r \"%s\"", "EXE_ENDOFGAME"));
				}
			}

			com_quit_f_hook.invoke<void>();
		}

		void sys_error_stub(const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			scheduler::once([]()
			{
				command::execute("map_rotate");
			}, scheduler::main, 3s);

			game::Com_Error(game::ERR_DROP, "%s", buffer);
		}
	}

	void initialize()
	{
		command::execute("exec default_xboxlive.cfg", true);
		command::execute("onlinegame 1", true);
		command::execute("xblive_privatematch 1", true);
	}

	class component final : public component_interface
	{
	public:
		void* load_import(const std::string& library, const std::string& function) override
		{
			return nullptr;
		}

		void post_unpack() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

#ifdef DEBUG
			printf("Starting dedicated server\n");
#endif

			// Register dedicated dvar
			dvars::register_bool("dedicated", true, game::DVAR_FLAG_READ);

			// Add lanonly mode
			dvars::register_bool("sv_lanOnly", false, game::DVAR_FLAG_NONE);

			// Disable VirtualLobby
			dvars::override::register_bool("virtualLobbyEnabled", false, game::DVAR_FLAG_READ);

			// Disable r_preloadShaders
			dvars::override::register_bool("r_preloadShaders", false, game::DVAR_FLAG_READ);

			// Stop crashing from sys_errors
			utils::hook::jump(0x140511520, sys_error_stub);

			// Hook R_SyncGpu
			utils::hook::jump(0x1405E12F0, sync_gpu_stub);

			utils::hook::jump(0x140254800, init_dedicated_server);

			// delay startup commands until the initialization is done
			utils::hook::call(0x1400D72D6, execute_startup_command);

			// delay console commands until the initialization is done
			utils::hook::call(0x1400D808C, execute_console_command);
			utils::hook::nop(0x1400D80A4, 5);

			// patch GScr_SetDynamicDvar to behave better
			gscr_set_dynamic_dvar_hook.create(0x14036B600, &gscr_set_dynamic_dvar);

			utils::hook::nop(0x1404ED90E, 5); // don't load config file
			utils::hook::nop(0x140403D92, 5); // ^
			utils::hook::set<uint8_t>(0x1400DC1D0, 0xC3); // don't save config file
			utils::hook::set<uint8_t>(0x140274710, 0xC3); // disable self-registration
			utils::hook::set<uint8_t>(0x140515890, 0xC3); // init sound system (1)
			utils::hook::set<uint8_t>(0x1406574F0, 0xC3); // init sound system (2)
			utils::hook::set<uint8_t>(0x140620D10, 0xC3); // render thread
			utils::hook::set<uint8_t>(0x14025B850, 0xC3); // called from Com_Frame, seems to do renderer stuff
			utils::hook::set<uint8_t>(0x1402507B0, 0xC3); // CL_CheckForResend, which tries to connect to the local server constantly
			utils::hook::set<uint8_t>(0x1405D5178, 0x00); // r_loadForRenderer default to 0
			utils::hook::set<uint8_t>(0x14050C2D0, 0xC3); // recommended settings check - TODO: Check hook
			utils::hook::set<uint8_t>(0x140514C00, 0xC3); // some mixer-related function called on shutdown
			utils::hook::set<uint8_t>(0x140409830, 0xC3); // dont load ui gametype stuff

			utils::hook::nop(0x140481B06, 6); // unknown check in SV_ExecuteClientMessage
			utils::hook::nop(0x140480FAC, 4); // allow first slot to be occupied
			utils::hook::nop(0x14025619B, 2); // properly shut down dedicated servers
			utils::hook::nop(0x14025615E, 2); // ^
			utils::hook::nop(0x1402561C0, 5); // don't shutdown renderer

			utils::hook::set<uint8_t>(0x140091840, 0xC3); // something to do with blendShapeVertsView
			utils::hook::nop(0x140659A0D, 8); // sound thing

			// (COULD NOT FIND IN H1)
			// utils::hook::set<uint8_t>(0x1404D6960, 0xC3); // cpu detection stuff?
			utils::hook::set<uint8_t>(0x1405E97F0, 0xC3); // gfx stuff during fastfile loading
			utils::hook::set<uint8_t>(0x1405E9700, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405E9790, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1402C1180, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405E9750, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405AD5B0, 0xC3); // directx stuff
			utils::hook::set<uint8_t>(0x1405DB150, 0xC3); // ^
			utils::hook::set<uint8_t>(0x140625220, 0xC3); // ^ - mutex
			utils::hook::set<uint8_t>(0x1405DB650, 0xC3); // ^

			utils::hook::set<uint8_t>(0x14008B5F0, 0xC3); // rendering stuff
			utils::hook::set<uint8_t>(0x1405DB8B0, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405DB9C0, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405DC050, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405DCBA0, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405DD240, 0xC3); // ^ 

			// shaders
			utils::hook::set<uint8_t>(0x1400916A0, 0xC3); // ^
			utils::hook::set<uint8_t>(0x140091610, 0xC3); // ^
			utils::hook::set<uint8_t>(0x14061ACC0, 0xC3); // ^ - mutex

			utils::hook::set<uint8_t>(0x140516080, 0xC3); // idk
			utils::hook::set<uint8_t>(0x1405AE5F0, 0xC3); // ^

			utils::hook::set<uint8_t>(0x1405E0B30, 0xC3); // R_Shutdown
			utils::hook::set<uint8_t>(0x1405AE400, 0xC3); // shutdown stuff
			utils::hook::set<uint8_t>(0x1405E0C00, 0xC3); // ^
			utils::hook::set<uint8_t>(0x1405DFE50, 0xC3); // ^

			// utils::hook::set<uint8_t>(0x1404B67E0, 0xC3); // sound crashes (H1 - questionable, function looks way different)

			utils::hook::set<uint8_t>(0x14048B660, 0xC3); // disable host migration

			utils::hook::set<uint8_t>(0x14042B2E0, 0xC3); // render synchronization lock
			utils::hook::set<uint8_t>(0x14042B210, 0xC3); // render synchronization unlock

			utils::hook::set<uint8_t>(0x140176D2D, 0xEB); // LUI: Unable to start the LUI system due to errors in main.lua

			utils::hook::nop(0x140506ECE, 5); // Disable sound pak file loading
			utils::hook::nop(0x140506ED6, 2); // ^
			utils::hook::set<uint8_t>(0x1402C5910, 0xC3); // Disable image pak file loading

			// Reduce min required memory
			utils::hook::set<uint64_t>(0x14050C717, 0x80000000);

			utils::hook::set(0x1402BF7F0, 0xC3); // some loop
			utils::hook::set(0x14007E150, 0xC3); // related to shader caching / techsets / fastfiles

			// initialize the game after onlinedataflags is 32 (workaround)
			scheduler::schedule([=]()
			{
				if (game::Live_SyncOnlineDataFlags(0) == 32 && game::Sys_IsDatabaseReady2())
				{
					scheduler::once([]()
					{
						command::execute("xstartprivateparty", true);
						command::execute("disconnect", true); // 32 -> 0
					}, scheduler::pipeline::main, 1s);
					return scheduler::cond_end;
				}

				return scheduler::cond_continue;
			}, scheduler::pipeline::main, 1s);

			scheduler::on_game_initialized([]()
			{
				initialize();

				console::info("==================================\n");
				console::info("Server started!\n");
				console::info("==================================\n");

				// remove disconnect command
				game::Cmd_RemoveCommand("disconnect");

				execute_startup_command_queue();
				execute_console_command_queue();

				// Send heartbeat to dpmaster
				scheduler::once(send_heartbeat, scheduler::pipeline::server);
				scheduler::loop(send_heartbeat, scheduler::pipeline::server, 10min);
				command::add("heartbeat", send_heartbeat);
			}, scheduler::pipeline::main, 1s);

			command::add("killserver", kill_server);
			com_quit_f_hook.create(0x1400DA640, &kill_server);
		}
	};
}

REGISTER_COMPONENT(dedicated::component)