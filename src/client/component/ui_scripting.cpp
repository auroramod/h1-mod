#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "scheduler.hpp"
#include "command.hpp"

#include "localized_strings.hpp"
#include "console.hpp"
#include "game_module.hpp"
#include "fps.hpp"
#include "server_list.hpp"
#include "filesystem.hpp"

#include "game/ui_scripting/execution.hpp"
#include "game/scripting/execution.hpp"

#include "ui_scripting.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/binary_resource.hpp>

namespace ui_scripting
{
	namespace
	{
		std::unordered_map<game::hks::cclosure*, std::function<arguments(const function_arguments& args)>> converted_functions;

		utils::hook::detour hks_start_hook;
		utils::hook::detour hks_shutdown_hook;
		utils::hook::detour hks_package_require_hook;

		utils::hook::detour hks_load_hook;
		utils::hook::detour db_find_xasset_header_hook;

		const auto lui_common = utils::nt::load_resource(LUI_COMMON);

		struct script
		{
			std::string name;
			std::string root;
		};

		struct globals_t
		{
			std::string in_require_script;
			std::vector<script> loaded_scripts;
			bool load_raw_script{};
			std::string raw_script_name{};
		};

		globals_t globals{};

		bool is_loaded_script(const std::string& name)
		{
			for (auto i = globals.loaded_scripts.begin(); i != globals.loaded_scripts.end(); ++i)
			{
				if (i->name == name)
				{
					return true;
				}
			}

			return false;
		}

		std::string get_root_script(const std::string& name)
		{
			for (auto i = globals.loaded_scripts.begin(); i != globals.loaded_scripts.end(); ++i)
			{
				if (i->name == name)
				{
					return i->root;
				}
			}

			return {};
		}

		table get_globals()
		{
			const auto state = *game::hks::lua_state;
			return state->globals.v.table;
		}

		void print_error(const std::string& error)
		{
			console::error("************** LUI script execution error **************\n");
			console::error("%s\n", error.data());
			console::error("********************************************************\n");
		}

		void print_loading_script(const std::string& name)
		{
			console::info("Loading LUI script '%s'\n", name.data());
		}

		std::string get_current_script()
		{
			const auto state = *game::hks::lua_state;
			game::hks::lua_Debug info{};
			game::hks::hksi_lua_getstack(state, 1, &info);
			game::hks::hksi_lua_getinfo(state, "nSl", &info);
			return info.short_src;
		}

		int load_buffer(const std::string& name, const std::string& data)
		{
			const auto state = *game::hks::lua_state;
			const auto sharing_mode = state->m_global->m_bytecodeSharingMode;
			state->m_global->m_bytecodeSharingMode = game::hks::HKS_BYTECODE_SHARING_ON;
			const auto _0 = gsl::finally([&]()
			{
				state->m_global->m_bytecodeSharingMode = sharing_mode;
			});

			game::hks::HksCompilerSettings compiler_settings{};
			return game::hks::hksi_hksL_loadbuffer(state, &compiler_settings, data.data(), data.size(), name.data());
		}

		void load_script(const std::string& name, const std::string& data)
		{
			globals.loaded_scripts.push_back({name, name});

			const auto lua = get_globals();
			const auto load_results = lua["loadstring"](data, name);

			if (load_results[0].is<function>())
			{
				const auto results = lua["pcall"](load_results);
				if (!results[0].as<bool>())
				{
					print_error(results[1].as<std::string>());
				}
			}
			else if (load_results[1].is<std::string>())
			{
				print_error(load_results[1].as<std::string>());
			}
		}

		void load_scripts(const std::string& script_dir)
		{
			if (!utils::io::directory_exists(script_dir))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir);

			for (const auto& script : scripts)
			{
				std::string data{};
				if (std::filesystem::is_directory(script) && utils::io::read_file(script + "/__init__.lua", &data))
				{
					print_loading_script(script);
					load_script(script + "/__init__.lua", data);
				}
			}
		}

		void setup_functions()
		{
			const auto lua = get_globals();

			lua["io"]["fileexists"] = utils::io::file_exists;
			lua["io"]["writefile"] = utils::io::write_file;
			lua["io"]["movefile"] = utils::io::move_file;
			lua["io"]["filesize"] = utils::io::file_size;
			lua["io"]["createdirectory"] = utils::io::create_directory;
			lua["io"]["directoryexists"] = utils::io::directory_exists;
			lua["io"]["directoryisempty"] = utils::io::directory_is_empty;
			lua["io"]["listfiles"] = utils::io::list_files;
			lua["io"]["removefile"] = utils::io::remove_file;
			lua["io"]["readfile"] = static_cast<std::string(*)(const std::string&)>(utils::io::read_file);

			using game = table;
			auto game_type = game();
			lua["game"] = game_type;

			game_type["getfps"] = [](const game&)
			{
				return fps::get_fps();
			};

			game_type["getping"] = [](const game&)
			{
				return *::game::mp::ping;
			};

			game_type["issingleplayer"] = [](const game&)
			{
				return ::game::environment::is_sp();
			};

			game_type["ismultiplayer"] = [](const game&)
			{
				return ::game::environment::is_mp();
			};

			game_type["addlocalizedstring"] = [](const game&, const std::string& string,
				const std::string& value)
			{
				localized_strings::override(string, value);
			};
		}

		void start()
		{
			globals = {};

			const auto lua = get_globals();
			lua["EnableGlobals"]();

			setup_functions();

			lua["print"] = function(reinterpret_cast<game::hks::lua_function>(SELECT_VALUE(0x93490_b, 0x209EB0_b)));
			lua["table"]["unpack"] = lua["unpack"];
			lua["luiglobals"] = lua;

			load_script("lui_common", lui_common);

			for (const auto& path : filesystem::get_search_paths())
			{
				load_scripts(path + "/ui_scripts/");
				if (game::environment::is_sp())
				{
					load_scripts(path + "/ui_scripts/sp/");
				}
				else
				{
					load_scripts(path + "/ui_scripts/mp/");
				}
			}

		}

		void try_start()
		{
			try
			{
				start();
			}
			catch (const std::exception& e)
			{
				console::error("Failed to load LUI scripts: %s\n", e.what());
			}
		}

		void* hks_start_stub(char a1)
		{
			const auto _0 = gsl::finally(&try_start);
			return hks_start_hook.invoke<void*>(a1);
		}

		void hks_shutdown_stub()
		{
			converted_functions.clear();
			globals = {};
			return hks_shutdown_hook.invoke<void>();
		}

		void* hks_package_require_stub(game::hks::lua_State* state)
		{
			const auto script = get_current_script();
			const auto root = get_root_script(script);
			globals.in_require_script = root;
			return hks_package_require_hook.invoke<void*>(state);
		}

		game::XAssetHeader db_find_xasset_header_stub(game::XAssetType type, const char* name, int allow_create_default)
		{
			if (type != 0x3D || !is_loaded_script(globals.in_require_script))
			{
				return db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, name, allow_create_default);
			}

			const auto folder = globals.in_require_script.substr(0, globals.in_require_script.find_last_of("/\\"));
			const std::string name_ = name;
			const std::string target_script = folder + "/" + name_ + ".lua";

			if (utils::io::file_exists(target_script))
			{
				globals.load_raw_script = true;
				globals.raw_script_name = target_script;
				return static_cast<game::XAssetHeader>(reinterpret_cast<game::LuaFile*>(1));
			}
			else if (name_.starts_with("ui/LUI/"))
			{
				return db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, name, allow_create_default);
			}

			return static_cast<game::XAssetHeader>(nullptr);
		}

		int hks_load_stub(game::hks::lua_State* state, void* compiler_options, 
			void* reader, void* reader_data, const char* chunk_name)
		{
			if (globals.load_raw_script)
			{
				globals.load_raw_script = false;
				globals.loaded_scripts.push_back({globals.raw_script_name, globals.in_require_script});
				return load_buffer(globals.raw_script_name, utils::io::read_file(globals.raw_script_name));
			}
			else
			{
				return hks_load_hook.invoke<int>(state, compiler_options, reader,
					reader_data, chunk_name);
			}
		}

		int main_handler(game::hks::lua_State* state)
		{
			const auto value = state->m_apistack.base[-1];
			if (value.t != game::hks::TCFUNCTION)
			{
				return 0;
			}

			const auto closure = value.v.cClosure;
			if (converted_functions.find(closure) == converted_functions.end())
			{
				return 0;
			}

			const auto& function = converted_functions[closure];

			try
			{
				const auto args = get_return_values();
				const auto results = function(args);

				for (const auto& result : results)
				{
					push_value(result);
				}

				return static_cast<int>(results.size());
			}
			catch (const std::exception& e)
			{
				game::hks::hksi_luaL_error(state, e.what());
			}

			return 0;
		}
	}

	template <typename F>
	game::hks::cclosure* convert_function(F f)
	{
		const auto state = *game::hks::lua_state;
		const auto closure = game::hks::cclosure_Create(state, main_handler, 0, 0, 0);
		converted_functions[closure] = wrap_function(f);
		return closure;
	}

	bool lui_running()
	{
		return *game::hks::lua_state != nullptr;
	}

	class component final : public component_interface
	{
	public:

		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			db_find_xasset_header_hook.create(game::DB_FindXAssetHeader, db_find_xasset_header_stub);
			hks_load_hook.create(SELECT_VALUE(0xB46F0_b, 0x22C180_b), hks_load_stub);

			hks_package_require_hook.create(SELECT_VALUE(0x90070_b, 0x214040_b), hks_package_require_stub);
			hks_start_hook.create(SELECT_VALUE(0x103C50_b, 0x27A790_b), hks_start_stub);
			hks_shutdown_hook.create(SELECT_VALUE(0xFB370_b, 0x2707C0_b), hks_shutdown_stub);

			command::add("lui_restart", []()
			{
				utils::hook::invoke<void>(SELECT_VALUE(0xFB370_b, 0x2707C0_b));
				utils::hook::invoke<void>(SELECT_VALUE(0x1052C0_b, 0x27BEC0_b));
			});
		}
	};
}

REGISTER_COMPONENT(ui_scripting::component)
