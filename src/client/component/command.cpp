#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "command.hpp"
#include "console.hpp"
#include "game_console.hpp"


#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>
#include "utils/io.hpp"

namespace command
{
	namespace
	{
		utils::hook::detour dvar_command_hook;

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

		void client_command(const int client_num, void* a2)
		{
			params_sv params = {};

			const auto command = utils::string::to_lower(params[0]);
			if (handlers_sv.find(command) != handlers_sv.end())
			{
				handlers_sv[command](client_num, params);
			}

			dvar_command_hook.invoke<void>(client_num, a2);
		}

		void enum_assets(const game::XAssetType type, const std::function<void(game::XAssetHeader)>& callback, const bool includeOverride)
		{
			game::DB_EnumXAssets_Internal(type, static_cast<void(*)(game::XAssetHeader, void*)>([](game::XAssetHeader header, void* data)
				{
					const auto& cb = *static_cast<const std::function<void(game::XAssetHeader)>*>(data);
					cb(header);
				}), &callback, includeOverride);
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
					const auto current = game::Dvar_ValueToString(dvar, dvar->current, 0);
					const auto reset = game::Dvar_ValueToString(dvar, dvar->reset, 0);

					game_console::print(game_console::con_type_info, "\"%s\" is: \"%s\" default: \"%s\" hash: %i",
						args[0], current, reset, dvar->name);

					game_console::print(game_console::con_type_info, "   %s\n",
						dvars::dvar_get_domain(dvar->type, dvar->domain).data());
				}
				//else
				//{
				//	char command[0x1000] = { 0 };
				//	game::Dvar_GetCombinedString(command, 1);
				//	game::Dvar_SetCommand(dvar->name, command);
				//}

				return dvar;
			}

			return 0;
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

	void add_raw(const char* name, void (*callback)())
	{
		game::Cmd_AddCommandInternal(name, callback, utils::memory::get_allocator()->allocate<game::cmd_function_s>());
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
			game::Cbuf_AddText(0, command.data());
		}
	}

	void parse_command_line()
	{
		static auto parsed = false;
		if (parsed)
		{
			return;
		}

		static std::string comand_line_buffer = GetCommandLineA();
		auto* command_line = comand_line_buffer.data();

		auto& com_num_console_lines = *reinterpret_cast<int*>(0x142623FB4);
		auto* com_console_lines = reinterpret_cast<char**>(0x142623FC0);

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
		reinterpret_cast<void(*)()>(0x1400D8210)(); // mwr: test
	}

	void read_startup_variable(const std::string& dvar)
	{
		// parse the commandline if it's not parsed
		parse_command_line();

		auto& com_num_console_lines = *reinterpret_cast<int*>(0x142623FB4);
		auto* com_console_lines = reinterpret_cast<char**>(0x142623FC0);

		for (int i = 0; i < com_num_console_lines; i++)
		{
			game::Cmd_TokenizeString(com_console_lines[i]);

			// only +set dvar value
			if (game::Cmd_Argc() >= 3 && game::Cmd_Argv(0) == "set"s && game::Cmd_Argv(1) == dvar)
			{
				game::Dvar_SetCommand(game::Cmd_Argv(1), game::Cmd_Argv(2));
			}

			game::Cmd_EndTokenizeString();
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			//utils::hook::jump(game::base_address + 0x000000, dvar_command_stub, true); // H1

			static game::cmd_function_s cmd_test;

			game::Cmd_AddCommandInternal("quit", game::Com_Quit_f, &cmd_test);
			/*game::Cmd_AddCommandInternal("connect", []() {

				}, game::cmd_function_s);*/
			//add_raw("quit", main_handler);

			//add("quit", game::Com_Quit_f);

			//add("startmap", [](const params& params)
			//{
			//	const auto map = params.get(1);

			//	const auto exists = utils::hook::invoke<bool>(game::base_address + 0x412B50, map, 0);

			//	if (!exists)
			//	{
			//		game_console::print(game_console::con_type_error, "map '%s' not found\n", map);
			//		return;
			//	}

			//	// SV_SpawnServer
			//	utils::hook::invoke<void>(game::base_address + 0x6B3AA0, map, 0, 0, 0, 0);
			//});

			/*add("say", [](const params& params)
			{
				chat::print(params.join(1));
			});*/

			//add("listassetpool", [](const params& params)
			//{
			//	if (params.size() < 2)
			//	{
			//		game_console::print(game_console::con_type_info, "listassetpool <poolnumber>: list all the assets in the specified pool\n");

			//		for (auto i = 0; i < game::XAssetType::ASSET_TYPE_COUNT; i++)
			//		{
			//			game_console::print(game_console::con_type_info, "%d %s\n", i, game::g_assetNames[i]);
			//		}
			//	}
			//	else
			//	{
			//		const auto type = static_cast<game::XAssetType>(atoi(params.get(1)));

			//		if (type < 0 || type >= game::XAssetType::ASSET_TYPE_COUNT)
			//		{
			//			game_console::print(game_console::con_type_info, "Invalid pool passed must be between [%d, %d]\n", 0, game::XAssetType::ASSET_TYPE_COUNT - 1);
			//			return;
			//		}

			//		game_console::print(game_console::con_type_info, "Listing assets in pool %s\n", game::g_assetNames[type]);

			//		enum_assets(type, [type](const game::XAssetHeader header)
			//		{
			//			const auto asset = game::XAsset{type, header};
			//			const auto* const asset_name = game::DB_GetXAssetName(&asset);
			//			//const auto entry = game::DB_FindXAssetEntry(type, asset_name);
			//			//TODO: display which zone the asset is from
			//			game_console::print(game_console::con_type_info, "%s\n", asset_name);
			//		}, true);
			//	}
			//});

			//add("commandDump", []()
			//{
			//	printf("======== Start command dump =========\n");

			//	game::cmd_function_s* cmd = (*game::cmd_functions);

			//	while (cmd)
			//	{
			//		if (cmd->name)
			//		{
			//			game_console::print(game_console::con_type_info, "%s\n", cmd->name);
			//		}

			//		cmd = cmd->next;
			//	}

			//	printf("======== End command dump =========\n");
			//});


			/*add("god", []()
				{
					if (!game::SV_Loaded())
					{
						return;
					}

					game::mp::g_entities[0].flags ^= game::FL_GODMODE;
					game::CG_GameMessage(0, utils::string::va("godmode %s",
						game::g_entities[0].flags & game::FL_GODMODE
						? "^2on"
						: "^1off"));
				});*/



			//add("notarget", []()
			//{
			//	if (!game::SV_Loaded())
			//	{
			//		return;
			//	}

			//	game::g_entities[0].flags ^= game::FL_NOTARGET;
			//	game::CG_GameMessage(0, utils::string::va("notarget %s",
			//		game::g_entities[0].flags & game::FL_NOTARGET
			//		? "^2on"
			//		: "^1off"));
			//});

			//add("noclip", []()
			//{
			//	if (!game::SV_Loaded())
			//	{
			//		return;
			//	}

			//	game::g_entities[0].client->flags ^= 1;
			//	game::CG_GameMessage(0, utils::string::va("noclip %s",
			//		game::g_entities[0].client->flags & 1
			//		? "^2on"
			//		: "^1off"));
			//});
		}
	};
}

REGISTER_COMPONENT(command::component)
