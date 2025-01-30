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

		const game::dvar_t* sv_lanOnly = nullptr;

		void init_dedicated_server()
		{
			static bool initialized = false;
			if (initialized) return;
			initialized = true;

			// R_LoadGraphicsAssets
			utils::hook::invoke<void>(0x686310_b);
		}

		void send_heartbeat()
		{
			if (sv_lanOnly && sv_lanOnly->current.enabled)
			{
				return;
			}

			game::netadr_s target{};
			if (server_list::get_master_server(target))
			{
				console::info("Sending heartbeat");
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

		void execute_console_command([[maybe_unused]] const int local_client_num, const char* command)
		{
			if (game::Live_SyncOnlineDataFlags(0) == 0)
			{
				command::execute(command);
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
				command::execute(command);
			}
		}

		void sync_gpu_stub()
		{
			const auto frame_time = *game::com_frameTime;
			const auto sys_msec = game::Sys_Milliseconds();
			const auto msec = frame_time - sys_msec;

			std::this_thread::sleep_for(std::chrono::milliseconds(msec));
		}

		void kill_server()
		{
			const auto* svs_clients = *game::mp::svs_clients;
			if (svs_clients != nullptr)
			{
				game::SV_GameSendServerCommand(-1, game::SV_CMD_CAN_IGNORE,
					utils::string::va("r \"%s\"", "EXE_ENDOFGAME"));
			}

			com_quit_f_hook.invoke<void>();
		}

		utils::hook::detour ui_set_active_menu_hook;
		void ui_set_active_menu_stub(void* localClientNum, int menu)
		{
			static auto done = false;
			if (done && (menu == 6 || menu == 7))
			{
				return;
			}

			if (menu == 6 || menu == 7)
			{
				done = true;
			}

			ui_set_active_menu_hook.invoke<void>(localClientNum, menu);
		}
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
			dvars::register_bool("dedicated", true, game::DVAR_FLAG_READ, "Dedicated server");

			// Add lanonly mode
			sv_lanOnly = dvars::register_bool("sv_lanOnly", false, game::DVAR_FLAG_NONE, "Don't send heartbeat");

			// Disable VirtualLobby
			dvars::override::register_bool("virtualLobbyEnabled", false, game::DVAR_FLAG_READ);

			// Disable r_preloadShaders
			dvars::override::register_bool("r_preloadShaders", false, game::DVAR_FLAG_READ);

			// Hook R_SyncGpu
			utils::hook::jump(0x688620_b, sync_gpu_stub, true);

			utils::hook::jump(0x135600_b, init_dedicated_server, true);

			// delay startup commands until the initialization is done
			utils::hook::jump(0x157DD3_b, utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.lea(r8, qword_ptr(rsp, 0x20));
				a.xor_(ecx, ecx);

				a.pushad64();
				a.call_aligned(execute_startup_command);
				a.popad64();

				a.jmp(0x157DDF_b);
			}), true);

			// delay console commands until the initialization is done // COULDN'T FOUND
			// utils::hook::call(0x1400D808C, execute_console_command);
			// utils::hook::nop(0x1400D80A4, 5);

			utils::hook::nop(0x189514_b, 248); // don't load config file
			utils::hook::nop(0x156C46_b, 5); // ^
			utils::hook::set<uint8_t>(0x17F470_b, 0xC3); // don't save config file
			utils::hook::set<uint8_t>(0x351AA0_b, 0xC3); // disable self-registration
			utils::hook::set<uint8_t>(0x5BF4E0_b, 0xC3); // init sound system (1)
			utils::hook::set<uint8_t>(0x701820_b, 0xC3); // init sound system (2)
			utils::hook::set<uint8_t>(0x701850_b, 0xC3); // init sound system (3)
			utils::hook::set<uint8_t>(0x6C9B10_b, 0xC3); // render thread
			utils::hook::set<uint8_t>(0x343950_b, 0xC3); // called from Com_Frame, seems to do renderer stuff
			utils::hook::set<uint8_t>(0x12CCA0_b, 0xC3); // CL_CheckForResend, which tries to connect to the local server constantly
			utils::hook::set<uint8_t>(0x67ADCE_b, 0x00); // r_loadForRenderer default to 0
			utils::hook::set<uint8_t>(0x5B7AF0_b, 0xC3); // recommended settings check
			utils::hook::set<uint8_t>(0x5BE850_b, 0xC3); // some mixer-related function called on shutdown
			utils::hook::set<uint8_t>(0x4DEA50_b, 0xC3); // dont load ui gametype stuff

			utils::hook::nop(0x54ED81_b, 6); // unknown check in SV_ExecuteClientMessage
			utils::hook::nop(0x54E337_b, 4); // allow first slot to be occupied
			utils::hook::nop(0x13ABCB_b, 2); // properly shut down dedicated servers
			utils::hook::nop(0x13AB8E_b, 2); // ^
			utils::hook::nop(0x13ABF0_b, 5); // don't shutdown renderer

			utils::hook::set<uint8_t>(0xAA290_b, 0xC3); // something to do with blendShapeVertsView
			utils::hook::nop(0x70465D_b, 8); // sound thing

			// (COULD NOT FIND IN H1)
			utils::hook::set<uint8_t>(0x1D8A20_b, 0xC3); // cpu detection stuff?
			utils::hook::set<uint8_t>(0x690F30_b, 0xC3); // gfx stuff during fastfile loading
			utils::hook::set<uint8_t>(0x690E00_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x690ED0_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x39B980_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x690E50_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x651BA0_b, 0xC3); // directx stuff
			utils::hook::set<uint8_t>(0x681950_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x6CE390_b, 0xC3); // ^ - mutex
			utils::hook::set<uint8_t>(0x681ED0_b, 0xC3); // ^

			utils::hook::set<uint8_t>(0x0A3CD0_b, 0xC3); // rendering stuff
			utils::hook::set<uint8_t>(0x682150_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x682260_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x6829C0_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x6834A0_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x683B40_b, 0xC3); // ^ 

			// shaders
			utils::hook::set<uint8_t>(0x0AA090_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x0A9FE0_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x6C38D0_b, 0xC3); // ^ - mutex

			utils::hook::set<uint8_t>(0x5BFD10_b, 0xC3); // idk
			utils::hook::set<uint8_t>(0x652E10_b, 0xC3); // ^

			utils::hook::set<uint8_t>(0x687D20_b, 0xC3); // R_Shutdown
			utils::hook::set<uint8_t>(0x652BA0_b, 0xC3); // shutdown stuff
			utils::hook::set<uint8_t>(0x687DF0_b, 0xC3); // ^
			utils::hook::set<uint8_t>(0x686DE0_b, 0xC3); // ^

			// utils::hook::set<uint8_t>(0x1404B67E0, 0xC3); // sound crashes (H1 - questionable, function looks way different)

			utils::hook::set<uint8_t>(0x556250_b, 0xC3); // disable host migration

			utils::hook::set<uint8_t>(0x4F7C10_b, 0xC3); // render synchronization lock
			utils::hook::set<uint8_t>(0x4F7B40_b, 0xC3); // render synchronization unlock

			utils::hook::set<uint8_t>(0x27AA9D_b, 0xEB); // LUI: Unable to start the LUI system due to errors in main.lua
			utils::hook::set<uint8_t>(0x27AAC5_b, 0xEB); // LUI: Unable to start the LUI system due to errors in depot.lua
			utils::hook::set<uint8_t>(0x27AADC_b, 0xEB); // ^

			utils::hook::nop(0x5B25BE_b, 5); // Disable sound pak file loading
			utils::hook::nop(0x5B25C6_b, 2); // ^
			utils::hook::set<uint8_t>(0x3A0BA0_b, 0xC3); // Disable image pak file loading

			// Reduce min required memory
			utils::hook::set<uint64_t>(0x5B7F37_b, 0x80000000);

			utils::hook::set<uint8_t>(0x399E10_b, 0xC3); // some loop
			utils::hook::set<uint8_t>(0x1D48B0_b, 0xC3); // related to shader caching / techsets / fastfilesc
			utils::hook::set<uint8_t>(0x3A1940_b, 0xC3); // DB_ReadPackedLoadedSounds

			// Workaround for server spamming 'exec default_xboxlive.cfg' when not running
			ui_set_active_menu_hook.create(0x1E4D80_b, ui_set_active_menu_stub);

			// initialize the game after onlinedataflags is 32 (workaround)
			scheduler::schedule([=]()
			{
				if (game::Live_SyncOnlineDataFlags(0) == 32 && game::Sys_IsDatabaseReady2())
				{
					scheduler::once([]
					{
						command::execute("xstartprivateparty", true);
						command::execute("disconnect", true); // 32 -> 0
					}, scheduler::pipeline::main, 1s);
					return scheduler::cond_end;
				}

				return scheduler::cond_continue;
			}, scheduler::pipeline::main, 1s);

			scheduler::on_game_initialized([]
			{
				command::execute("exec default_xboxlive.cfg", true);
				command::execute("onlinegame 1", true);
				command::execute("xblive_privatematch 1", true);

				console::info("==================================\n");
				console::info("Server started!\n");
				console::info("==================================\n");

				// remove disconnect command
				game::Cmd_RemoveCommand("disconnect");

				execute_startup_command_queue();
				execute_console_command_queue();

				// Send heartbeat to master
				scheduler::once(send_heartbeat, scheduler::pipeline::server);
				scheduler::loop(send_heartbeat, scheduler::pipeline::server, 10min);
				command::add("heartbeat", send_heartbeat);
			}, scheduler::pipeline::main, 1s);

			command::add("killserver", kill_server);
			com_quit_f_hook.create(0x17CD00_b, kill_server);
		}
	};
}

REGISTER_COMPONENT(dedicated::component)
