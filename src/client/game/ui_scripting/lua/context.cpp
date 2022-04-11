#include <std_include.hpp>
#include "context.hpp"
#include "error.hpp"
#include "value_conversion.hpp"
#include "../../scripting/execution.hpp"
#include "../script_value.hpp"
#include "../execution.hpp"

#include "../../../component/ui_scripting.hpp"
#include "../../../component/command.hpp"
#include "../../../component/updater.hpp"
#include "../../../component/fps.hpp"
#include "../../../component/localized_strings.hpp"
#include "../../../component/fastfiles.hpp"
#include "../../../component/scripting.hpp"
#include "../../../component/mods.hpp"
#include "../../../component/discord.hpp"

#include "component/game_console.hpp"
#include "component/scheduler.hpp"

#include <discord_rpc.h>

#include <utils/string.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>

namespace ui_scripting::lua
{
	namespace
	{
		const auto json_script = utils::nt::load_resource(LUA_JSON);

		void setup_json(sol::state& state)
		{
			const auto json = state.safe_script(json_script, &sol::script_pass_on_error);
			handle_error(json);
			state["json"] = json;
		}

		void setup_io(sol::state& state)
		{
			state["io"]["fileexists"] = utils::io::file_exists;
			state["io"]["writefile"] = utils::io::write_file;
			state["io"]["movefile"] = utils::io::move_file;
			state["io"]["filesize"] = utils::io::file_size;
			state["io"]["createdirectory"] = utils::io::create_directory;
			state["io"]["directoryexists"] = utils::io::directory_exists;
			state["io"]["directoryisempty"] = utils::io::directory_is_empty;
			state["io"]["listfiles"] = utils::io::list_files;
			state["io"]["copyfolder"] = utils::io::copy_folder;
			state["io"]["removefile"] = utils::io::remove_file;
			state["io"]["removedirectory"] = utils::io::remove_directory;
			state["io"]["readfile"] = static_cast<std::string(*)(const std::string&)>(utils::io::read_file);
		}

		void setup_types(sol::state& state, scheduler& scheduler)
		{
			struct game
			{
			};
			auto game_type = state.new_usertype<game>("game_");
			state["game"] = game();

			game_type["ontimeout"] = [&scheduler](const game&, const sol::protected_function& callback,
			                                      const long long milliseconds)
			{
				return scheduler.add(callback, milliseconds, true);
			};

			game_type["oninterval"] = [&scheduler](const game&, const sol::protected_function& callback,
			                                       const long long milliseconds)
			{
				return scheduler.add(callback, milliseconds, false);
			};

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

			game_type["assetlist"] = [](const game&, const sol::this_state s, const std::string& type_string)
			{
				auto table = sol::table::create(s.lua_state());
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
				fastfiles::enum_assets(type, [type, &table, &index](const ::game::XAssetHeader header)
				{
					const auto asset = ::game::XAsset{type, header};
					const std::string asset_name = ::game::DB_GetXAssetName(&asset);
					table[index++] = asset_name;
				}, true);

				return table;
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
				return mods::mod_path;
			};

			auto userdata_type = state.new_usertype<userdata>("userdata_");

			userdata_type["new"] = sol::property(
				[](const userdata& userdata, const sol::this_state s)
				{
					return convert(s, userdata.get("new"));
				},
				[](const userdata& userdata, const sol::this_state s, const sol::lua_value& value)
				{
					userdata.set("new", convert({s, value}));
				}
			);
			
			userdata_type["get"] = [](const userdata& userdata, const sol::this_state s,
				const sol::lua_value& key)
			{
				return convert(s, userdata.get(convert({s, key})));
			};

			userdata_type["set"] = [](const userdata& userdata, const sol::this_state s,
				const sol::lua_value& key, const sol::lua_value& value)
			{
				userdata.set(convert({s, key}), convert({s, value}));
			};

			userdata_type[sol::meta_function::index] = [](const userdata& userdata, const sol::this_state s, 
				const sol::lua_value& key)
			{
				return convert(s, userdata.get(convert({s, key})));
			};

			userdata_type[sol::meta_function::new_index] = [](const userdata& userdata, const sol::this_state s, 
				const sol::lua_value& key, const sol::lua_value& value)
			{
				userdata.set(convert({s, key}), convert({s, value}));
			};

			auto table_type = state.new_usertype<table>("table_");

			table_type["new"] = sol::property(
				[](const table& table, const sol::this_state s)
				{
					return convert(s, table.get("new"));
				},
				[](const table& table, const sol::this_state s, const sol::lua_value& value)
				{
					table.set("new", convert({s, value}));
				}
			);

			table_type["get"] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key)
			{
				return convert(s, table.get(convert({s, key})));
			};

			table_type["set"] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key, const sol::lua_value& value)
			{
				table.set(convert({s, key}), convert({s, value}));
			};

			table_type[sol::meta_function::index] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key)
			{
				return convert(s, table.get(convert({s, key})));
			};

			table_type[sol::meta_function::new_index] = [](const table& table, const sol::this_state s,
				const sol::lua_value& key, const sol::lua_value& value)
			{
				table.set(convert({s, key}), convert({s, value}));
			};

			auto function_type = state.new_usertype<function>("function_");

			function_type[sol::meta_function::call] = [](const function& function, const sol::this_state s, sol::variadic_args va)
			{
				arguments arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				const auto values = function.call(arguments);
				std::vector<sol::lua_value> returns;

				for (const auto& value : values)
				{
					returns.push_back(convert(s, value));
				}

				return sol::as_returns(returns);
			};

			if (::game::environment::is_sp())
			{
				struct player
				{
				};
				auto player_type = state.new_usertype<player>("player_");
				state["player"] = player();

				player_type["notify"] = [](const player&, const sol::this_state s, const std::string& name, sol::variadic_args va)
				{
					if (!::game::CL_IsCgameInitialized() || !::game::sp::g_entities[0].client)
					{
						throw std::runtime_error("Not in game");
					}

					const sol::state_view view{s};
					const auto to_string = view["tostring"].get<sol::protected_function>();

					std::vector<std::string> args{};
					for (auto arg : va)
					{
						args.push_back(to_string.call(arg).get<std::string>());
					}

					::scheduler::once([s, name, args]()
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

			state["luiglobals"] = table((*::game::hks::lua_state)->globals.v.table);
			state["CoD"] = state["luiglobals"]["CoD"];
			state["LUI"] = state["luiglobals"]["LUI"];
			state["Engine"] = state["luiglobals"]["Engine"];
			state["Game"] = state["luiglobals"]["Game"];

			auto updater_table = sol::table::create(state.lua_state());

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
			
			state["updater"] = updater_table;

			auto discord_table = sol::table::create(state.lua_state());

			discord_table["respond"] = discord::respond;
			discord_table["getavatarmaterial"] = discord::get_avatar_material;
			discord_table["reply"] = sol::table::create(state.lua_state());
			discord_table["reply"]["yes"] = DISCORD_REPLY_YES;
			discord_table["reply"]["ignore"] = DISCORD_REPLY_IGNORE;
			discord_table["reply"]["no"] = DISCORD_REPLY_NO;

			state["discord"] = discord_table;
		}
	}

	context::context(std::string data, script_type type)
		: scheduler_(state_)
	{
		this->state_.open_libraries(sol::lib::base,
		                            sol::lib::package,
		                            sol::lib::io,
		                            sol::lib::string,
		                            sol::lib::os,
		                            sol::lib::math,
		                            sol::lib::table);

		setup_json(this->state_);
		setup_io(this->state_);
		setup_types(this->state_, this->scheduler_);

		if (type == script_type::file)
		{
			this->folder_ = data;

			this->state_["include"] = [this](const std::string& file)
			{
				this->load_script(file);
			};

			sol::function old_require = this->state_["require"];
			auto base_path = utils::string::replace(this->folder_, "/", ".") + ".";
			this->state_["require"] = [base_path, old_require](const std::string& path)
			{
				return old_require(base_path + path);
			};

			this->state_["scriptdir"] = [this]()
			{
				return this->folder_;
			};

			printf("Loading ui script '%s'\n", this->folder_.data());
			this->load_script("__init__");
		}

		if (type == script_type::code)
		{
			handle_error(this->state_.safe_script(data, &sol::script_pass_on_error));
		}
	}

	context::~context()
	{
		this->state_.collect_garbage();
		this->scheduler_.clear();
		this->state_ = {};
	}

	void context::run_frame()
	{
		this->scheduler_.run_frame();
		this->state_.collect_garbage();
	}

	void context::load_script(const std::string& script)
	{
		if (!this->loaded_scripts_.emplace(script).second)
		{
			return;
		}

		const auto file = (std::filesystem::path{this->folder_} / (script + ".lua")).generic_string();
		handle_error(this->state_.safe_script_file(file, &sol::script_pass_on_error));
	}
}
