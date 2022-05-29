#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "game_console.hpp"
#include "fastfiles.hpp"
#include "scheduler.hpp"
#include "logfile.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "game/scripting/execution.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>
#include <utils/io.hpp>

namespace command
{
	namespace
	{
		utils::hook::detour client_command_hook;
		utils::hook::detour parse_commandline_hook;

		std::unordered_map<std::string, std::function<void(params&)>> handlers;
		std::unordered_map<std::string, std::function<void(int, params_sv&)>> handlers_sv;

		void main_handler()
		{
			params params = {};

			const auto command = utils::string::to_lower(params[0]);
			if (handlers.find(command) != handlers.end())
			{
				handlers[command](params);
			}
		}

		void client_command(const int client_num)
		{
			if (!logfile::client_command_stub(client_num))
			{
				return;
			}

			params_sv params = {};

			const auto command = utils::string::to_lower(params[0]);
			if (handlers_sv.find(command) != handlers_sv.end())
			{
				handlers_sv[command](client_num, params);
			}

			client_command_hook.invoke<void>(client_num);
		}

		// Shamelessly stolen from Quake3
		// https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/qcommon/common.c#L364
		void parse_command_line()
		{
			static auto parsed = false;
			if (parsed)
			{
				return;
			}

			static std::string comand_line_buffer = GetCommandLineA();
			auto* command_line = comand_line_buffer.data();

			auto& com_num_console_lines = *reinterpret_cast<int*>(0x35634B8_b);
			auto* com_console_lines = reinterpret_cast<char**>(0x35634C0_b);

			auto inq = false;
			com_console_lines[0] = command_line;
			com_num_console_lines = 0;

			while (*command_line)
			{
				if (*command_line == '"')
				{
					inq = !inq;
				}
				// look for a + separating character
				// if commandLine came from a file, we might have real line seperators
				if ((*command_line == '+' && !inq) || *command_line == '\n' || *command_line == '\r')
				{
					if (com_num_console_lines == 0x20) // MAX_CONSOLE_LINES
					{
						break;
					}
					com_console_lines[com_num_console_lines] = command_line + 1;
					com_num_console_lines++;
					*command_line = '\0';
				}
				command_line++;
			}
			parsed = true;
		}

		void parse_commandline_stub()
		{
			parse_command_line();
			parse_commandline_hook.invoke<void>();
		}

		game::dvar_t* dvar_command_stub()
		{
			const params args;

			if (args.size() <= 0)
			{
				return 0;
			}

			const auto dvar = game::Dvar_FindVar(args[0]);

			if (dvar)
			{
				if (args.size() == 1)
				{
					const auto current = game::Dvar_ValueToString(dvar, true, dvar->current);
					const auto reset = game::Dvar_ValueToString(dvar, true, dvar->reset);

					console::info("\"%s\" is: \"%s\" default: \"%s\" hash: 0x%08lX type: %i\n",
						args[0], current, reset, dvar->hash, dvar->type);

					const auto dvar_info = dvars::dvar_get_description(args[0]);

					console::info("%s\n", dvar_info.data());
					console::info("   %s\n", dvars::dvar_get_domain(dvar->type, dvar->domain).data());
				}
				else
				{
					char command[0x1000] = {0};
					game::Dvar_GetCombinedString(command, 1);
					game::Dvar_SetCommand(dvar->hash, "", command);
				}

				return dvar;
			}

			return 0;
		}

		void client_println(int client_num, const std::string& text)
		{
			if (game::environment::is_sp())
			{
				game::CG_GameMessage(0, text.data());
			}
			else
			{
				game::SV_GameSendServerCommand(client_num, game::SV_CMD_RELIABLE,
					utils::string::va("f \"%s\"", text.data()));
			}
		}

		bool check_cheats(int client_num)
		{
			if (!game::Dvar_FindVar("sv_cheats")->current.enabled)
			{
				client_println(client_num, "Cheats are not enabled on this server");
				return false;
			}

			return true;
		}

		void cmd_give_weapon(const int client_num, const std::vector<std::string>& params)
		{
			if (params.size() < 2)
			{
				client_println(client_num, "You did not specify a weapon name");
				return;
			}

			try
			{
				const auto& arg = params[1];
				const auto player = scripting::entity({static_cast<uint16_t>(client_num), 0});
				auto ps = game::SV_GetPlayerstateForClientNum(client_num);

				if (arg == "ammo")
				{
					const auto weapon = player.call("getcurrentweapon").as<std::string>();
					player.call("givemaxammo", {weapon});
				}
				else if (arg == "allammo")
				{
					const auto weapons = player.call("getweaponslistall").as<scripting::array>();
					for (auto i = 0; i < weapons.size(); i++)
					{
						player.call("givemaxammo", {weapons[i]});
					}
				}
				else if (arg == "health")
				{
					if (params.size() > 2)
					{
						const auto amount = atoi(params[2].data());
						const auto health = player.get("health").as<int>();
						player.set("health", {health + amount});
					}
					else
					{
						const auto amount = SELECT_VALUE(
							game::Dvar_FindVar("g_player_maxhealth")->current.integer,
							atoi(game::Dvar_FindVar("scr_player_maxhealth")->current.string)
						);
						player.set("health", {amount});
					}
				}
				else if (arg == "all")
				{
					const auto type = game::XAssetType::ASSET_TYPE_WEAPON;
					fastfiles::enum_assets(type, [&player, type](const game::XAssetHeader header)
					{
						const auto asset = game::XAsset{type, header};
						const auto asset_name = game::DB_GetXAssetName(&asset);

						player.call("giveweapon", {asset_name});
					}, true);
				}
				else
				{
					const auto wp = game::G_GetWeaponForName(arg.data());
					if (wp)
					{
						if (game::G_GivePlayerWeapon(ps, wp, 0, 0, 0, 0, 0, 0))
						{
							game::G_InitializeAmmo(ps, wp, 0);
							game::G_SelectWeapon(0, wp);
						}
					}
					else
					{
						client_println(client_num, "Weapon does not exist");
					}
				}
			}
			catch (...)
			{
			}
		}

		void cmd_drop_weapon(int client_num)
		{
			try
			{
				const auto player = scripting::entity({static_cast<uint16_t>(client_num), 0});
				const auto weapon = player.call("getcurrentweapon");
				player.call("dropitem", {weapon});
			}
			catch (...)
			{
			}
		}

		void cmd_take_weapon(int client_num, const std::vector<std::string>& params)
		{
			if (params.size() < 2)
			{
				client_println(client_num, "You did not specify a weapon name");
				return;
			}

			const auto& weapon = params[1];

			try
			{
				const auto player = scripting::entity({static_cast<uint16_t>(client_num), 0});
				if (weapon == "all"s)
				{
					player.call("takeallweapons");
				}
				else
				{
					player.call("takeweapon", {weapon});
				}
			}
			catch (...)
			{
			}
		}

		void cmd_kill(int client_num)
		{
			scheduler::once([client_num]()
			{
				try
				{
					const auto player = scripting::entity({static_cast<uint16_t>(client_num), 0});
					player.call(SELECT_VALUE("kill", "suicide"));
				}
				catch (...)
				{
				}
			}, scheduler::pipeline::server);
		}

		void toggle_entity_flag(int client_num, int value, const std::string& name)
		{
			game::mp::g_entities[client_num].flags ^= value;
			client_println(client_num, utils::string::va("%s %s",
				name.data(),
				game::mp::g_entities[client_num].flags & value
					? "^2on"
					: "^1off"));
		}

		void toggle_entity_flag(int value, const std::string& name)
		{
			game::sp::g_entities[0].flags ^= value;
			client_println(0, utils::string::va("%s %s",
				name.data(),
				game::sp::g_entities[0].flags & value
					? "^2on"
					: "^1off"));
		}

		void toggle_client_flag(int client_num, int value, const std::string& name)
		{
			game::mp::g_entities[client_num].client->flags ^= value;
			client_println(client_num, utils::string::va("%s %s",
				name.data(),
				game::mp::g_entities[client_num].client->flags & value
					? "^2on"
					: "^1off"));
		}

		void toggle_client_flag(int value, const std::string& name)
		{
			game::sp::g_entities[0].client->flags ^= value;
			client_println(0, utils::string::va("%s %s",
				name.data(),
				game::sp::g_entities[0].client->flags & value
					? "^2on"
					: "^1off"));
		}
	}

	void read_startup_variable(const std::string& dvar)
	{
		// parse the commandline if it's not parsed
		parse_command_line();

		auto& com_num_console_lines = *reinterpret_cast<int*>(0x35634B8_b);
		auto* com_console_lines = reinterpret_cast<char**>(0x35634C0_b);

		for (int i = 0; i < com_num_console_lines; i++)
		{
			game::Cmd_TokenizeString(com_console_lines[i]);

			// only +set dvar value
			if (game::Cmd_Argc() >= 3 && game::Cmd_Argv(0) == "set"s && game::Cmd_Argv(1) == dvar)
			{
				game::Dvar_SetCommand(game::generateHashValue(game::Cmd_Argv(1)), "", game::Cmd_Argv(2));
			}

			game::Cmd_EndTokenizeString();
		}
	}

	params::params()
		: nesting_(game::cmd_args->nesting)
	{
	}

	int params::size() const
	{
		return game::cmd_args->argc[this->nesting_];
	}

	const char* params::get(const int index) const
	{
		if (index >= this->size())
		{
			return "";
		}

		return game::cmd_args->argv[this->nesting_][index];
	}

	std::string params::join(const int index) const
	{
		std::string result = {};

		for (auto i = index; i < this->size(); i++)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}
		return result;
	}

	std::vector<std::string> params::get_all() const
	{
		std::vector<std::string> params_;
		for (auto i = 0; i < this->size(); i++)
		{
			params_.push_back(this->get(i));
		}
		return params_;
	}

	params_sv::params_sv()
		: nesting_(game::sv_cmd_args->nesting)
	{
	}

	int params_sv::size() const
	{
		return game::sv_cmd_args->argc[this->nesting_];
	}

	const char* params_sv::get(const int index) const
	{
		if (index >= this->size())
		{
			return "";
		}

		return game::sv_cmd_args->argv[this->nesting_][index];
	}

	std::string params_sv::join(const int index) const
	{
		std::string result = {};

		for (auto i = index; i < this->size(); i++)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}
		return result;
	}

	std::vector<std::string> params_sv::get_all() const
	{
		std::vector<std::string> params_;
		for (auto i = 0; i < this->size(); i++)
		{
			params_.push_back(this->get(i));
		}
		return params_;
	}

	void add_raw(const char* name, void (*callback)())
	{
		game::Cmd_AddCommandInternal(name, callback, utils::memory::get_allocator()->allocate<game::cmd_function_s>());
	}

	void add_test(const char* name, void (*callback)())
	{
		static game::cmd_function_s cmd_test;
		return game::Cmd_AddCommandInternal(name, callback, &cmd_test);
	}

	void add(const char* name, const std::function<void(const params&)>& callback)
	{
		const auto command = utils::string::to_lower(name);

		if (handlers.find(command) == handlers.end())
			add_raw(name, main_handler);

		handlers[command] = callback;
	}

	void add(const char* name, const std::function<void()>& callback)
	{
		add(name, [callback](const params&)
		{
			callback();
		});
	}

	void add_sv(const char* name, std::function<void(int, const params_sv&)> callback)
	{
		// doing this so the sv command would show up in the console
		add_raw(name, nullptr);

		const auto command = utils::string::to_lower(name);

		if (handlers_sv.find(command) == handlers_sv.end())
			handlers_sv[command] = std::move(callback);
	}

	void execute(std::string command, const bool sync)
	{
		command += "\n";

		if (sync)
		{
			game::Cmd_ExecuteSingleCommand(0, 0, command.data());
		}
		else
		{
			game::Cbuf_AddText(0, 0, command.data());
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				add_commands_sp();
			}
			else
			{
				parse_commandline_hook.create(0x157D50_b, parse_commandline_stub);
				add_commands_mp();
			}

			utils::hook::jump(SELECT_VALUE(0x3A7C80_b, 0x4E9F40_b), dvar_command_stub, true);

			add_commands_generic();
		}

	private:
		static void add_commands_generic()
		{
			add("quit", game::Quit);
			add("crash", []()
			{
				*reinterpret_cast<int*>(1) = 0;
			});

			add("commandDump", [](const params& argument)
			{
				console::info("================================ COMMAND DUMP =====================================\n");
				game::cmd_function_s* cmd = (*game::cmd_functions);
				std::string filename;
				if (argument.size() == 2)
				{
					filename = "h1-mod/";
					filename.append(argument[1]);
					if (!filename.ends_with(".txt"))
					{
						filename.append(".txt");
					}
				}
				int i = 0;
				while (cmd)
				{
					if (cmd->name)
					{
						if (!filename.empty())
						{
							const auto line = std::format("{}\r\n", cmd->name);
							utils::io::write_file(filename, line, i != 0);
						}
						console::info("%s\n", cmd->name);
						i++;
					}
					cmd = cmd->next;
				}
				console::info("\n%i commands\n", i);
				console::info("================================ END COMMAND DUMP =================================\n");
			});

			add("listassetpool", [](const params& params)
			{
				if (params.size() < 2)
				{
					console::info("listassetpool <poolnumber> [filter]: list all the assets in the specified pool\n");

					for (auto i = 0; i < game::XAssetType::ASSET_TYPE_COUNT; i++)
					{
						console::info("%d %s\n", i, game::g_assetNames[i]);
					}
				}
				else
				{
					const auto type = static_cast<game::XAssetType>(atoi(params.get(1)));

					if (type < 0 || type >= game::XAssetType::ASSET_TYPE_COUNT)
					{
						console::error("Invalid pool passed must be between [%d, %d]\n", 0, game::XAssetType::ASSET_TYPE_COUNT - 1);
						return;
					}

					console::info("Listing assets in pool %s\n", game::g_assetNames[type]);

					const std::string filter = params.get(2);
					fastfiles::enum_assets(type, [type, filter](const game::XAssetHeader header)
					{
						const auto asset = game::XAsset{type, header};
						const auto* const asset_name = game::DB_GetXAssetName(&asset);
						//const auto entry = game::DB_FindXAssetEntry(type, asset_name);
						//TODO: display which zone the asset is from

						if (!filter.empty() && !utils::string::match_compare(filter, asset_name, false))
						{
							return;
						}

						console::info("%s\n", asset_name);
					}, true);
				}
			});

			add("vstr", [](const params& params)
			{
				if (params.size() < 2)
				{
					console::info("vstr <variablename> : execute a variable command\n");
					return;
				}

				const auto name = params.get(1);
				const auto dvar = game::Dvar_FindVar(name);

				if (dvar == nullptr)
				{
					console::info("%s doesn't exist\n", name);
					return;
				}

				if (dvar->type != game::dvar_type::string
					&& dvar->type != game::dvar_type::enumeration)
				{
					console::info("%s is not a string-based dvar\n", name);
					return;
				}

				execute(dvar->current.string);
			});
		}

		static void add_commands_sp()
		{
			/*add("god", []()
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				toggle_entity_flag(1, "godmode");
			});

			add("demigod", []()
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				toggle_entity_flag(2, "demigod mode");
			});

			add("notarget", []()
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				toggle_entity_flag(4, "notarget");
			});

			add("noclip", []()
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				toggle_client_flag(1, "noclip");
			});

			add("ufo", []()
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				toggle_client_flag(2, "ufo");
			});

			add("dropweapon", [](const params& params)
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				cmd_drop_weapon(0);
			});

			add("take", [](const params& params)
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				cmd_take_weapon(0, params.get_all());
			});

			add("kill", [](const params& params)
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				cmd_kill(0);
			});
			
			add("give", [](const params& params)
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				cmd_give_weapon(0, params.get_all());
			});*/
		}

		static void add_commands_mp()
		{
			client_command_hook.create(0x4132E0_b, &client_command);

			/*add_sv("god", [](const int client_num, const params_sv&)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				toggle_entity_flag(client_num, 1, "godmode");
			});

			add_sv("demigod", [](const int client_num, const params_sv&)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				toggle_entity_flag(client_num, 2, "demigod mode");
			});

			add_sv("notarget", [](const int client_num, const params_sv&)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				toggle_entity_flag(client_num, 4, "notarget");
			});

			add_sv("noclip", [](const int client_num, const params_sv&)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				toggle_client_flag(client_num, 1, "noclip");
			});

			add_sv("ufo", [](const int client_num, const params_sv&)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				toggle_client_flag(client_num, 2, "noclip");
			});*/

			add_sv("give", [](const int client_num, const params_sv& params)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				cmd_give_weapon(client_num, params.get_all());
			});

			add_sv("dropweapon", [](const int client_num, const params_sv& params)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				cmd_drop_weapon(client_num);
			});

			add_sv("take", [](const int client_num, const params_sv& params)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				cmd_take_weapon(client_num, params.get_all());
			});

			add_sv("kill", [](const int client_num, const params_sv& params)
			{
				if (!check_cheats(client_num))
				{
					return;
				}

				cmd_kill(client_num);
			});
		}
	};
}

REGISTER_COMPONENT(command::component)
