#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "scheduler.hpp"
#include "command.hpp"

#include "localized_strings.hpp"
#include "console.hpp"
#include "discord.hpp"
#include "download.hpp"
#include "game_module.hpp"
#include "fps.hpp"
#include "server_list.hpp"
#include "filesystem.hpp"
#include "mods.hpp"
#include "fastfiles.hpp"
#include "scripting.hpp"
#include "updater.hpp"
#include "server_list.hpp"
#include "party.hpp"

#include "game/ui_scripting/execution.hpp"
#include "game/scripting/execution.hpp"

#include "ui_scripting.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/binary_resource.hpp>

#include "steam/steam.hpp"

#include <discord_rpc.h>

namespace ui_scripting
{
	namespace
	{
		std::unordered_map<game::hks::cclosure*, std::function<arguments(const function_arguments& args)>> converted_functions;

		utils::hook::detour hks_start_hook;
		utils::hook::detour hks_shutdown_hook;
		utils::hook::detour hks_package_require_hook;

		utils::hook::detour hks_load_hook;

		const auto lui_common = utils::nt::load_resource(LUI_COMMON);
		const auto lui_updater = utils::nt::load_resource(LUI_UPDATER);
		const auto lua_json = utils::nt::load_resource(LUA_JSON);
		const auto lui_compat_mp = utils::nt::load_resource(LUI_COMPAT_MP);

		struct globals_t
		{
			std::string in_require_script;
			std::unordered_map<std::string, std::string> loaded_scripts;
			bool load_raw_script{};
			std::string raw_script_name{};
		};

		globals_t globals{};

		bool is_loaded_script(const std::string& name)
		{
			return globals.loaded_scripts.contains(name);
		}

		std::string get_root_script(const std::string& name)
		{
			const auto itr = globals.loaded_scripts.find(name);
			return itr == globals.loaded_scripts.end() ? std::string() : itr->second;
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
			globals.loaded_scripts[name] = name;

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

		std::unordered_set<std::string> list_scripts(const std::string& script_dir, bool use_rawfiles)
		{
			std::unordered_set<std::string> list;

			if (use_rawfiles)
			{
				fastfiles::enum_assets(game::ASSET_TYPE_RAWFILE, [&](const game::XAssetHeader header)
				{
					std::string name = header.rawfile->name;
					if (name.starts_with(script_dir) && name.ends_with("/__init__.lua"))
					{
						const auto idx = name.find("/__init__.lua");
						const auto script = name.substr(0, idx);
						list.insert(script);
					}
				}, true);
			}
			else
			{
				if (utils::io::directory_exists(script_dir))
				{
					const auto scripts = utils::io::list_files(script_dir);
					for (const auto& script : scripts)
					{
						if (std::filesystem::is_directory(script) && utils::io::file_exists(script + "/__init__.lua"))
						{
							list.insert(script);
						}
					}
				}
			}

			return list;
		}

		bool script_exists(const std::string& script, bool use_fs)
		{
			return (use_fs ? filesystem::exists(script) : utils::io::file_exists(script)) || game::DB_XAssetExists(game::ASSET_TYPE_RAWFILE, script.data());
		}

		bool read_script(const std::string& script, std::string* data, bool use_fs)
		{
			if (use_fs)
			{
				if (filesystem::read_file(script, data))
				{
					return true;
				}
			}
			else
			{
				if (utils::io::read_file(script, data))
				{
					return true;
				}
			}

			if (game::DB_XAssetExists(game::ASSET_TYPE_RAWFILE, script.data()))
			{
				const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_RAWFILE, script.data(), 0);
				const auto len = game::DB_GetRawFileLen(asset.rawfile);
				data->resize(len);
				game::DB_GetRawBuffer(asset.rawfile, data->data(), len);
				data->pop_back();
				return true;
			}

			return false;
		}

		void load_scripts(const std::string& script_dir, bool use_rawfiles = false)
		{
			const auto scripts = list_scripts(script_dir, use_rawfiles);

			for (const auto& script : scripts)
			{
				const auto init_file = script + "/__init__.lua";
				std::string data;
				if (read_script(init_file, &data, false))
				{
					print_loading_script(script);
					load_script(init_file, data);
				}
				else
				{
					console::error("Failed to read script '%s'\n", init_file.data());
				}
			}
		}

		script_value json_to_lua(const nlohmann::json& json)
		{
			if (json.is_object())
			{
				table object;
				for (const auto& [key, value] : json.items())
				{
					object[key] = json_to_lua(value);
				}
				return object;
			}

			if (json.is_array())
			{
				table array;
				auto index = 1;
				for (const auto& value : json)
				{
					array[index++] = json_to_lua(value);
				}
				return array;
			}

			if (json.is_boolean())
			{
				return json.get<bool>();
			}

			if (json.is_number_integer())
			{
				return json.get<int>();
			}

			if (json.is_number_float())
			{
				return json.get<float>();
			}

			if (json.is_string())
			{
				return json.get<std::string>();
			}

			return {};
		}

		void setup_functions()
		{
			const auto lua = get_globals();

			using game = table;
			auto game_type = game();
			lua["game"] = game_type;

			game_type["getfps"] = [](const game&)
			{
				return fps::get_fps();
			};

			if (::game::environment::is_mp())
			{
				game_type["getping"] = [](const game&)
				{
					if (!::game::CL_IsCgameInitialized())
					{
						return 0;
					}

					return ::game::mp::client_state->ping;
				};
			}

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
			
			game_type["sharedset"] = [](const game&, const std::string& key, const std::string& value)
			{
				scripting::shared_table.access([key, value](scripting::shared_table_t& table)
				{
					table[key] = value;
				});
			};

			game_type["sharedget"] = [](const game&, const std::string& key)
			{
				std::string result;
				scripting::shared_table.access([key, &result](scripting::shared_table_t& table)
				{
					result = table[key];
				});
				return result;
			};

			game_type["sharedclear"] = [](const game&)
			{
				scripting::shared_table.access([](scripting::shared_table_t& table)
				{
					table.clear();
				});
			};

			game_type["assetlist"] = [](const game&, const std::string& type_string)
			{
				auto table_ = table();
				auto index = 1;
				auto type_index = -1;

				for (auto i = 0; i < ::game::XAssetType::ASSET_TYPE_COUNT; i++)
				{
					if (type_string == ::game::g_assetNames[i])
					{
						type_index = i;
					}
				}

				if (type_index == -1)
				{
					throw std::runtime_error("Asset type does not exist");
				}

				const auto type = static_cast<::game::XAssetType>(type_index);
				fastfiles::enum_assets(type, [type, &table_, &index](const ::game::XAssetHeader header)
				{
					const auto asset = ::game::XAsset{type, header};
					const std::string asset_name = ::game::DB_GetXAssetName(&asset);
					table_[index++] = asset_name;
				}, true);

				return table_;
			};

			game_type["getweapondisplayname"] = [](const game&, const std::string& name)
			{
				const auto alternate = name.starts_with("alt_");
				const auto weapon = ::game::G_GetWeaponForName(name.data());

				char buffer[0x400] = {0};
				::game::CG_GetWeaponDisplayName(weapon, alternate, buffer, 0x400);

				return std::string(buffer);
			};

			game_type["getloadedmod"] = [](const game&)
			{
				const auto& path = mods::get_mod();
				return path.value_or("");
			};

			if (::game::environment::is_sp())
			{
				using player = table;
				auto player_type = player();
				lua["player"] = player_type;

				player_type["notify"] = [](const player&, const std::string& name, const variadic_args& va)
				{
					if (!::game::CL_IsCgameInitialized() || !::game::SV_Loaded())
					{
						throw std::runtime_error("Not in game");
					}

					const auto to_string = get_globals()["tostring"];
					const auto arguments = get_return_values();
					std::vector<std::string> args{};
					for (const auto& value : va)
					{
						const auto value_str = to_string(value);

						args.push_back(value_str[0].as<std::string>());
					}

					::scheduler::once([name, args]()
					{
						try
						{
							std::vector<scripting::script_value> arguments{};

							for (const auto& arg : args)
							{
								arguments.push_back(arg);
							}

							const auto player = scripting::call("getentbynum", {0}).as<scripting::entity>();
							scripting::notify(player, name, arguments);
						}
						catch (...)
						{
						}
					}, ::scheduler::pipeline::server);
				};
			}

			game_type["virtuallobbypresentable"] = [](const game&)
			{
				::game::Dvar_SetFromStringByNameFromSource("virtualLobbyPresentable", "1", ::game::DVAR_SOURCE_INTERNAL);
			};

			game_type["getcurrentgamelanguage"] = [](const game&)
			{
				return steam::SteamApps()->GetCurrentGameLanguage();
			};

			game_type["isdefaultmaterial"] = [](const game&, const std::string& material)
			{
				return static_cast<bool>(::game::DB_IsXAssetDefault(::game::ASSET_TYPE_MATERIAL,
					material.data()));
			};

			game_type["getcommandbind"] = [](const game&, const std::string& cmd)
			{
				const auto binding = ::game::Key_GetBindingForCmd(cmd.data());
				auto key = -1;
				for (auto i = 0; i < 256; i++)
				{
					if (::game::playerKeys[0].keys[i].binding == binding)
					{
						key = i;
					}
				}

				if (key == -1)
				{
					return ::game::UI_SafeTranslateString("KEY_UNBOUND");
				}
				else
				{
					const auto loc_string = ::game::Key_KeynumToString(key, 1, 0);
					return ::game::UI_SafeTranslateString(loc_string);
				}
			};

			static std::unordered_set<std::string> available_languages =
			{
				"english",
				"english_safe",
				"french",
				"german",
				"italian",
				"polish",
				"portuguese",
				"russian",
				"spanish",
				"simplified_chinese",
				"traditional_chinese",
				"japanese_partial",
				"korean"
			};

			game_type["setlanguage"] = [](const game&, const std::string& language)
			{
				if (available_languages.contains(language))
				{
					utils::io::write_file("players2/default/language", language);
				}
			};

			game_type["islanguageavailable"] = [](const game&, const std::string& language)
			{
				if (!available_languages.contains(language))
				{
					return false;
				}

				return utils::io::directory_exists("zone/" + language);
			};

			game_type["zoneexists"] = [](const game&, const std::string& zone)
			{
				if (fastfiles::exists(zone, false))
				{
					return true;
				}

				return utils::io::file_exists(utils::string::va("usermaps/%s/%s.ff", zone.data(), zone.data()));
			};

			auto mods_table = table();
			lua["mods"] = mods_table;

			mods_table["getloaded"] = []() -> script_value
			{
				const auto& mod = mods::get_mod();
				if (mod.has_value())
				{
					return mod.value();
				}

				return {};
			};

			mods_table["getlist"] = mods::get_mod_list;
			mods_table["getinfo"] = [](const std::string& mod)
			{
				table info_table{};
				const auto info = mods::get_mod_info(mod);
				const auto has_value = info.has_value();
				info_table["isvalid"] = has_value;

				if (!has_value)
				{
					return info_table;
				}

				const auto& map = info.value();
				for (const auto& [key, value] : map.items())
				{
					info_table[key] = json_to_lua(value);
				}

				return info_table;
			};

			mods_table["load"] = [](const std::string& mod)
			{
				scheduler::once([=]()
				{
					mods::load(mod);
				}, scheduler::main);
			};

			mods_table["unload"] = []
			{
				scheduler::once([]()
				{
					mods::unload();
				}, scheduler::main);
			};

			auto depot_table = table();
			lua["customdepot"] = depot_table;

			static const std::string depot_file_path = "players2/user/depot.json";

			depot_table["save"] = [](const std::string& data)
			{
				return utils::io::write_file(depot_file_path, data, false);
			};

			depot_table["load"] = []() -> script_value
			{
				std::string data;
				if (!utils::io::read_file(depot_file_path, &data))
				{
					return {};
				}

				return data;
			};

			auto server_list_table = table();
			lua["serverlist"] = server_list_table;

			server_list_table["getplayercount"] = server_list::get_player_count;
			server_list_table["getservercount"] = server_list::get_server_count;

			auto updater_table = table();
			lua["updater"] = updater_table;

			updater_table["relaunch"] = updater::relaunch;

			updater_table["sethastriedupdate"] = updater::set_has_tried_update;
			updater_table["gethastriedupdate"] = updater::get_has_tried_update;
			updater_table["autoupdatesenabled"] = updater::auto_updates_enabled;

			updater_table["startupdatecheck"] = updater::start_update_check;
			updater_table["isupdatecheckdone"] = updater::is_update_check_done;
			updater_table["getupdatecheckstatus"] = updater::get_update_check_status;
			updater_table["isupdateavailable"] = updater::is_update_available;

			updater_table["startupdatedownload"] = updater::start_update_download;
			updater_table["isupdatedownloaddone"] = updater::is_update_download_done;
			updater_table["getupdatedownloadstatus"] = updater::get_update_download_status;
			updater_table["cancelupdate"] = updater::cancel_update;
			updater_table["isrestartrequired"] = updater::is_restart_required;

			updater_table["getlasterror"] = updater::get_last_error;
			updater_table["getcurrentfile"] = updater::get_current_file;

			auto download_table = table();
			lua["download"] = download_table;

			download_table["abort"] = download::stop_download;

			download_table["userdownloadresponse"] = party::user_download_response;
			download_table["getwwwurl"] = []
			{
				const auto state = party::get_server_connection_state();
				return state.base_url;
			};

			auto discord_table = table();
			lua["discord"] = discord_table;

			discord_table["respond"] = discord::respond;

			discord_table["getavatarmaterial"] = [](const std::string& id)
				-> script_value
			{
				const auto material = discord::get_avatar_material(id);
				if (material == nullptr)
				{
					return {};
				}

				return lightuserdata(material);
			};

			lua["string"]["escapelocalization"] = [](const std::string& str)
			{
				return "\x1F"s.append(str);
			};

			lua["string"]["el"] = lua["string"]["escapelocalization"];

			discord_table["reply"] = table();
			discord_table["reply"]["yes"] = DISCORD_REPLY_YES;
			discord_table["reply"]["ignore"] = DISCORD_REPLY_IGNORE;
			discord_table["reply"]["no"] = DISCORD_REPLY_NO;

			auto bits_table = table();
			lua["bits"] = bits_table;

			bits_table["lshift"] = [](const int a, const int b)
			{
				return a << b;
			};

			bits_table["rshift"] = [](const int a, const int b)
			{
				return a >> b;
			};

			bits_table["andbits"] = [](const int a, const int b)
			{
				return a & b;
			};

			bits_table["orbits"] = [](const int a, const int b)
			{
				return a | b;
			};

			bits_table["neg"] = [](const int a)
			{
				return ~a;
			};

			if (::game::environment::is_mp() && lua["Lobby"].is<table>())
			{
				lua["Lobby"]["GetMapCustomField"] = [](const std::string& key) -> std::string
				{
					const auto* mapname = ::game::Dvar_FindVar("ui_mapname");
					if (!mapname || !mapname->current.string)
					{
						return {};
					}

					const auto* value = ::game::UI_GetMapCustomField(key.data(), mapname->current.string);
					return value ? value : "";
				};
			}
		}

		void start()
		{
			globals = {};

			const auto lua = get_globals();
			lua["EnableGlobals"]();

			setup_functions();

			lua["print"] = [](const variadic_args& va)
			{
				std::string buffer{};
				const auto to_string = get_globals()["tostring"];

				for (auto i = 0; i < va.size(); i++)
				{
					const auto& arg = va[i];
					const auto str = to_string(arg)[0].as<std::string>();
					buffer.append(str);

					if (i < va.size() - 1)
					{
						buffer.append("\t");
					}
				}

				console::info("%s\n", buffer.data());
			};

			lua["table"]["unpack"] = lua["unpack"];
			lua["luiglobals"] = lua;

			load_script("lui_common", lui_common);
			load_script("lui_updater", lui_updater);
			load_script("lua_json", lua_json);

			if (game::environment::is_mp())
			{
				load_script("lui_compat_mp", lui_compat_mp);
			}

			auto search_paths = filesystem::get_search_paths_rev();
			search_paths.emplace_back("");

			for (const auto& path : search_paths)
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

			load_scripts("ui_scripts/", true);
			if (game::environment::is_sp())
			{
				load_scripts("ui_scripts/sp/", true);
			}
			else
			{
				load_scripts("ui_scripts/mp/", true);
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
			if (game::environment::is_sp())
			{
			return hks_start_hook.invoke<void*>(a1);
		}

			return utils::hook::invoke<void*>(0x140176A40, a1); // hks_start
		}

		void hks_shutdown_stub()
		{
			converted_functions.clear();
			globals = {};
			if (game::environment::is_sp())
			{
			return hks_shutdown_hook.invoke<void>();
		}

			return utils::hook::invoke<void>(0x14016CA80); // hks_shutdown
		}

		void* hks_package_require_stub(game::hks::lua_State* state)
		{
			const auto script = get_current_script();
			const auto root = get_root_script(script);
			globals.in_require_script = root;
			if (game::environment::is_sp())
			{
			return hks_package_require_hook.invoke<void*>(state);
		}

			return utils::hook::invoke<void*>(0x140115730, state); // package_require
		}

		bool read_lua_as_rawfile(const std::string& name, std::string* data)
		{
			if (filesystem::read_file(name, data))
			{
				return true;
			}

			const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_RAWFILE, name.data(), 0);
			if (asset.rawfile != nullptr)
			{
				const auto len = game::DB_GetRawFileLen(asset.rawfile);
				data->resize(len);
				game::DB_GetRawBuffer(asset.rawfile, data->data(), len);
				return true;
			}

			return false;
		}

		game::XAssetHeader db_find_x_asset_header_stub(game::XAssetType type, const char* name, int allow_create_default)
		{
			game::XAssetHeader header{.luaFile = nullptr};

			if (!is_loaded_script(globals.in_require_script))
			{
				header = game::DB_FindXAssetHeader(type, name, allow_create_default);
				if (header.luaFile == nullptr && script_exists(name, true))
				{
					header.luaFile = reinterpret_cast<game::LuaFile*>(1);
				}

				return header;
			}

			const auto folder = globals.in_require_script.substr(0, globals.in_require_script.find_last_of("/\\"));
			const std::string name_ = name;
			const std::string target_script = folder + "/" + name_ + ".lua";

			if (script_exists(target_script, false))
			{
				globals.load_raw_script = true;
				globals.raw_script_name = target_script;
				header.luaFile = reinterpret_cast<game::LuaFile*>(1);
			}
			else if (name_.starts_with("ui/LUI/"))
			{
				return game::DB_FindXAssetHeader(type, name, allow_create_default);
			}

			return header;
		}

		int hks_load_stub(game::hks::lua_State* state, void* compiler_options, 
			void* reader, void* reader_data, const char* chunk_name)
		{
			if (globals.load_raw_script)
			{
				globals.load_raw_script = false;
				globals.loaded_scripts[globals.raw_script_name] = globals.in_require_script;

				std::string data;
				if (read_script(globals.raw_script_name, &data, false))
				{
					return load_buffer(globals.raw_script_name, data);
				}

				return 0;
			}

			std::string name = chunk_name;
			name = name.substr(1);

			std::string data;
			if (read_script(name, &data, true))
			{
				console::info("Overriding lua file %s\n", name.data());
				return load_buffer(chunk_name, data);
			}
			else
			{
				if (game::environment::is_sp())
			{
				return hks_load_hook.invoke<int>(state, compiler_options, reader,
					reader_data, chunk_name);
			}

				return utils::hook::invoke<int>(0x14012BC20, state, compiler_options, reader,
					reader_data, chunk_name); // hks_load
			}
		}

		std::string current_error;
		int main_handler(game::hks::lua_State* state)
		{
			bool error = false;

			try
			{
				const auto value = state->m_apistack.base[-1];
				if (value.t != game::hks::TCFUNCTION)
				{
					return 0;
				}

				const auto closure = value.v.cClosure;
				if (!converted_functions.contains(closure))
				{
					return 0;
				}

				const auto& function = converted_functions[closure];

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
				current_error = e.what();
				error = true;
			}

			if (error)
			{
				game::hks::hksi_luaL_error(state, current_error.data());
			}

			return 0;
		}

		int removed_function_stub(game::hks::lua_State* /*state*/)
		{
			return 0;
		}
	}

	table get_globals()
	{
		const auto state = *game::hks::lua_state;
		return state->globals.v.table;
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

			dvars::register_bool("r_preloadShadersFrontendAllow", true, game::DVAR_ARCHIVE, "Allow shader popup on startup");

			utils::hook::call(SELECT_VALUE(0x1400E7419, 0x14015B3C9), db_find_x_asset_header_stub);
			utils::hook::call(SELECT_VALUE(0x1400E72CB, 0x14015B27B), db_find_x_asset_header_stub);

			if (game::environment::is_sp())
			{
				hks_load_hook.create(0x1400B46F0, hks_load_stub);

				hks_package_require_hook.create(0x140090070, hks_package_require_stub);
				hks_start_hook.create(0x140103C50, hks_start_stub);
				hks_shutdown_hook.create(0x1400FB370, hks_shutdown_stub);
			}
			else
			{
				// hks_load
				utils::hook::call(0x14015B454, hks_load_stub);
				utils::hook::call(0x14015B679, hks_load_stub);

				// package_require
				utils::hook::set(reinterpret_cast<void**>(0x1408192A8), reinterpret_cast<void*>(hks_package_require_stub));

				// hks_start
				utils::hook::call(0x140178164, hks_start_stub);
				utils::hook::call(0x14025C148, hks_start_stub);

				// hks_shutdown
				utils::hook::call(0x140176133, hks_shutdown_stub);
				utils::hook::call(0x140178125, hks_shutdown_stub);
				utils::hook::call(0x140178407, hks_shutdown_stub);
			}

			command::add("lui_restart", []
			{
				utils::hook::invoke<void>(SELECT_VALUE(0x1401052C0, 0x1401780D0));
			});

			// remove unsafe functions
			if (game::environment::is_mp())
			{
				utils::hook::nop(0x14012AE3A, 1); // int3 in traceback handler
				utils::hook::jump(0x14016AE20, 0x14012ACE0); // lua panic handler -> traceback handler

				utils::hook::jump(0x1401142F0, removed_function_stub); // io
				utils::hook::jump(0x140114870, removed_function_stub); // os
				utils::hook::jump(0x1401155A0, removed_function_stub); // serialize
				utils::hook::jump(0x140115570, removed_function_stub); // hks
				utils::hook::jump(0x140114CE0, removed_function_stub); // debug
				utils::hook::nop(0x14011425A, 5); // coroutine

				// profile
				utils::hook::jump(0x140108F00, removed_function_stub); // profile_tick
				utils::hook::jump(0x140108F10, removed_function_stub); // profile_stats
				utils::hook::jump(0x140108F20, removed_function_stub);
				utils::hook::jump(0x140108FE0, removed_function_stub); // profile_graphheap

				utils::hook::jump(0x14010ABE0, removed_function_stub); // base_loadfile
				utils::hook::jump(0x14010A850, removed_function_stub); // base_dofile
				utils::hook::jump(0x14010D9A0, removed_function_stub);

				utils::hook::jump(0x140115E40, removed_function_stub);
				utils::hook::jump(0x1401148A0, removed_function_stub);
				utils::hook::jump(0x1401155D0, removed_function_stub); // all_in_one_loader

				utils::hook::jump(0x140109C00, removed_function_stub);
				utils::hook::jump(0x140110740, removed_function_stub);
			}
		}
	};
}

REGISTER_COMPONENT(ui_scripting::component)
