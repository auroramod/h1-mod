#include <std_include.hpp>
#include "context.hpp"
#include "error.hpp"
#include "value_conversion.hpp"

#include "game/scripting/execution.hpp"

#include "component/command.hpp"
#include "component/logfile.hpp"
#include "component/scripting.hpp"
#include "component/fastfiles.hpp"
#include "component/scheduler.hpp"

#include "component/gsc/script_loading.hpp"

#include <utils/string.hpp>
#include <utils/io.hpp>
#include <utils/http.hpp>

namespace scripting::lua
{
	namespace
	{
		struct http_request
		{
			sol::protected_function on_error;
			sol::protected_function on_progress;
			sol::protected_function on_load;
		};

		std::unordered_map<uint64_t, http_request> http_requests;

		std::string get_as_string(const sol::this_state s, sol::object o)
		{
			sol::state_view state(s);
			return state["tostring"](o);
		}

		vector normalize_vector(const vector& vec)
		{
			const auto length = sqrt(
				(vec.get_x() * vec.get_x()) + 
				(vec.get_y() * vec.get_y()) + 
				(vec.get_z() * vec.get_z())
			);

			return vector(
				vec.get_x() / length,
				vec.get_y() / length,
				vec.get_z() / length
			);
		}

		void setup_io(sol::state& state)
		{
			state["io"]["fileexists"] = utils::io::file_exists;
			state["io"]["writefile"] = utils::io::write_file;
			state["io"]["remove_file"] = utils::io::remove_file;
			state["io"]["filesize"] = utils::io::file_size;
			state["io"]["createdirectory"] = utils::io::create_directory;
			state["io"]["directoryexists"] = utils::io::directory_exists;
			state["io"]["directoryisempty"] = utils::io::directory_is_empty;
			state["io"]["listfiles"] = utils::io::list_files;
			state["io"]["copyfolder"] = utils::io::copy_folder;
			state["io"]["readfile"] = static_cast<std::string(*)(const std::string&)>(utils::io::read_file);
		}

		void setup_vector_type(sol::state& state)
		{
			auto vector_type = state.new_usertype<vector>("vector", sol::constructors<vector(float, float, float)>());
			vector_type["x"] = sol::property(&vector::get_x, &vector::set_x);
			vector_type["y"] = sol::property(&vector::get_y, &vector::set_y);
			vector_type["z"] = sol::property(&vector::get_z, &vector::set_z);

			vector_type["r"] = sol::property(&vector::get_x, &vector::set_x);
			vector_type["g"] = sol::property(&vector::get_y, &vector::set_y);
			vector_type["b"] = sol::property(&vector::get_z, &vector::set_z);

			vector_type[sol::meta_function::addition] = sol::overload(
				[](const vector& a, const vector& b)
				{
					return vector(
						a.get_x() + b.get_x(),
						a.get_y() + b.get_y(),
						a.get_z() + b.get_z()
					);
				},
				[](const vector& a, const int value)
				{
					return vector(
						a.get_x() + value,
						a.get_y() + value,
						a.get_z() + value
					);
				}
			);

			vector_type[sol::meta_function::subtraction] = sol::overload(
				[](const vector& a, const vector& b)
				{
					return vector(
						a.get_x() - b.get_x(),
						a.get_y() - b.get_y(),
						a.get_z() - b.get_z()
					);
				},
				[](const vector& a, const int value)
				{
					return vector(
						a.get_x() - value,
						a.get_y() - value,
						a.get_z() - value
					);
				}
			);

			vector_type[sol::meta_function::multiplication] = sol::overload(
				[](const vector& a, const vector& b)
				{
					return vector(
						a.get_x() * b.get_x(),
						a.get_y() * b.get_y(),
						a.get_z() * b.get_z()
					);
				},
				[](const vector& a, const int value)
				{
					return vector(
						a.get_x() * value,
						a.get_y() * value,
						a.get_z() * value
					);
				}
			);

			vector_type[sol::meta_function::division] = sol::overload(
				[](const vector& a, const vector& b)
				{
					return vector(
						a.get_x() / b.get_x(),
						a.get_y() / b.get_y(),
						a.get_z() / b.get_z()
					);
				},
				[](const vector& a, const int value)
				{
					return vector(
						a.get_x() / value,
						a.get_y() / value,
						a.get_z() / value
					);
				}
			);

			vector_type[sol::meta_function::equal_to] = [](const vector& a, const vector& b)
			{
				return a.get_x() == b.get_x() &&
					   a.get_y() == b.get_y() &&
					   a.get_z() == b.get_z();
			};

			vector_type[sol::meta_function::length] = [](const vector& a)
			{
				return sqrt((a.get_x() * a.get_x()) + (a.get_y() * a.get_y()) + (a.get_z() * a.get_z()));
			};

			vector_type[sol::meta_function::to_string] = [](const vector& a)
			{
				return utils::string::va("{x: %f, y: %f, z: %f}", a.get_x(), a.get_y(), a.get_z());
			};

			vector_type["normalize"] = [](const vector& a)
			{
				return normalize_vector(a);
			};

			vector_type["toangles"] = [](const vector& a)
			{
				return call("vectortoangles", {a}).as<vector>();
			};

			vector_type["toyaw"] = [](const vector& a)
			{
				return call("vectortoyaw", {a}).as<vector>();
			};

			vector_type["tolerp"] = [](const vector& a)
			{
				return call("vectortolerp", {a}).as<vector>();
			};

			vector_type["toup"] = [](const vector& a)
			{
				return call("anglestoup", {a}).as<vector>();
			};

			vector_type["toright"] = [](const vector& a)
			{
				return call("anglestoright", {a}).as<vector>();
			};

			vector_type["toforward"] = [](const vector& a)
			{
				return call("anglestoforward", {a}).as<vector>();
			};
		}

		void setup_entity_type(sol::state& state, event_handler& handler, scheduler& scheduler)
		{
			state["level"] = entity{*game::levelEntityId};

			if (game::environment::is_sp())
			{
				state["player"] = call("getentbynum", {0}).as<entity>();
			}

			auto entity_type = state.new_usertype<entity>("entity");

			for (auto const& func : gsc::gsc_ctx->meth_map())
			{
				const auto name = std::string(func.first);
				entity_type[name] = [name](const entity& entity, const sol::this_state s, sol::variadic_args va)
				{
					std::vector<script_value> arguments{};

					for (auto arg : va)
					{
						arguments.push_back(convert({s, arg}));
					}

					return convert(s, entity.call(name, arguments));
				};
			}

			entity_type["set"] = [](const entity& entity, const std::string& field,
			                        const sol::lua_value& value)
			{
				entity.set(field, convert(value));
			};

			entity_type["get"] = [](const entity& entity, const sol::this_state s, const std::string& field)
			{
				return convert(s, entity.get(field));
			};

			entity_type["notify"] = [](const entity& entity, const sol::this_state s, const std::string& event, 
									   sol::variadic_args va)
			{
				std::vector<script_value> arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				notify(entity, event, arguments);
			};

			entity_type["onnotify"] = [&handler](const entity& entity, const std::string& event,
			                                     const event_callback& callback)
			{
				event_listener listener{};
				listener.callback = callback;
				listener.entity = entity;
				listener.event = event;
				listener.is_volatile = false;

				return handler.add_event_listener(std::move(listener));
			};

			entity_type["onnotifyonce"] = [&handler](const entity& entity, const std::string& event,
			                                         const event_callback& callback)
			{
				event_listener listener{};
				listener.callback = callback;
				listener.entity = entity;
				listener.event = event;
				listener.is_volatile = true;

				return handler.add_event_listener(std::move(listener));
			};

			entity_type["call"] = [](const entity& entity, const sol::this_state s, const std::string& function,
			                         sol::variadic_args va)
			{
				std::vector<script_value> arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				return convert(s, entity.call(function, arguments));
			};

			entity_type[sol::meta_function::new_index] = [](const entity& entity, const std::string& field,
															const sol::lua_value& value)
			{
				entity.set(field, convert(value));
			};

			entity_type[sol::meta_function::index] = [](const entity& entity, const sol::this_state s, const std::string& field)
			{
				return convert(s, entity.get(field));
			};

			entity_type["getstruct"] = [](const entity& entity, const sol::this_state s)
			{
				const auto id = entity.get_entity_id();
				return entity_to_struct(s, id);
			};

			entity_type["struct"] = sol::property([](const entity& entity, const sol::this_state s)
			{
				const auto id = entity.get_entity_id();
				return entity_to_struct(s, id);
			});

			entity_type["scriptcall"] = [](const entity& entity, const sol::this_state s, const std::string& filename,
				const std::string function, sol::variadic_args va)
			{
				std::vector<script_value> arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				return convert(s, call_script_function(entity, filename, function, arguments));
			};

			struct game
			{
			};
			auto game_type = state.new_usertype<game>("game_");
			state["game"] = game();

			for (auto const& func : gsc::gsc_ctx->func_map())
			{
				const auto name = std::string(func.first);
				game_type[name] = [name](const game&, const sol::this_state s, sol::variadic_args va)
				{
					std::vector<script_value> arguments{};

					for (auto arg : va)
					{
						arguments.push_back(convert({s, arg}));
					}

					return convert(s, call(name, arguments));
				};
			}

			game_type["call"] = [](const game&, const sol::this_state s, const std::string& function,
			                       sol::variadic_args va)
			{
				std::vector<script_value> arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				return convert(s, call(function, arguments));
			};

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

			game_type["executecommand"] = [](const game&, const std::string& command)
			{
				command::execute(command, false);
			};

			game_type["onplayerdamage"] = [](const game&, const sol::protected_function& callback)
			{
				logfile::add_player_damage_callback(callback);
			};

			game_type["onplayerkilled"] = [](const game&, const sol::protected_function& callback)
			{
				logfile::add_player_killed_callback(callback);
			};

			game_type["getgamevar"] = [](const sol::this_state s)
			{
				const auto value = ::game::scr_VarGlob->childVariableValue[*::game::gameEntityId];

				::game::VariableValue variable{};
				variable.type = value.type;
				variable.u.uintValue = value.u.u.uintValue;

				return convert(s, variable);
			};

			game_type["getfunctions"] = [entity_type](const game&, const sol::this_state s, const std::string& filename)
			{
				if (!script_function_table.contains(filename))
				{
					throw std::runtime_error("File '" + filename + "' not found");
				}

				auto functions = sol::table::create(s.lua_state());

				for (const auto& function : scripting::script_function_table[filename])
				{
					functions[function.first] = sol::overload(
						[filename, function](const entity& entity, const sol::this_state s, sol::variadic_args va)
						{
							std::vector<script_value> arguments{};

							for (auto arg : va)
							{
								arguments.push_back(convert({s, arg}));
							}

							const auto _0 = gsl::finally(&logfile::enable_vm_execute_hook);
							logfile::disable_vm_execute_hook();

							return convert(s, call_script_function(entity, filename, function.first, arguments));
						},
						[filename, function](const sol::this_state s, sol::variadic_args va)
						{
							std::vector<script_value> arguments{};

							for (auto arg : va)
							{
								arguments.push_back(convert({s, arg}));
							}

							const auto _0 = gsl::finally(&logfile::enable_vm_execute_hook);
							logfile::disable_vm_execute_hook();

							return convert(s, call_script_function(*::game::levelEntityId, filename, function.first, arguments));
						}
					);
				}

				return functions;
			};

			game_type["scriptcall"] = [](const game&, const sol::this_state s, const std::string& filename,
				const std::string function, sol::variadic_args va)
			{
				std::vector<script_value> arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				const auto _0 = gsl::finally(&logfile::enable_vm_execute_hook);
				logfile::disable_vm_execute_hook();

				return convert(s, call_script_function(*::game::levelEntityId, filename, function, arguments));
			};

			game_type["detour"] = [](const game&, const sol::this_state s, const std::string& filename,
				const std::string function_name, const sol::protected_function& function)
			{
				const auto pos = get_function_pos(filename, function_name);
				logfile::set_lua_hook(pos, function);

				auto detour = sol::table::create(function.lua_state());

				detour["disable"] = [pos]()
				{
					logfile::clear_hook(pos);
				};

				detour["enable"] = [&]()
				{
					logfile::set_lua_hook(pos, function);
				};

				detour["invoke"] = sol::overload(
					[filename, function_name](const entity& entity, const sol::this_state s, sol::variadic_args va)
					{
						std::vector<script_value> arguments{};

						for (auto arg : va)
						{
							arguments.push_back(convert({s, arg}));
						}

						const auto _0 = gsl::finally(&logfile::enable_vm_execute_hook);
						logfile::disable_vm_execute_hook();

						return convert(s, call_script_function(entity, filename, function_name, arguments));
					},
					[filename, function_name](const sol::this_state s, sol::variadic_args va)
					{
						std::vector<script_value> arguments{};

						for (auto arg : va)
						{
							arguments.push_back(convert({s, arg}));
						}

						const auto _0 = gsl::finally(&logfile::enable_vm_execute_hook);
						logfile::disable_vm_execute_hook();

						return convert(s, call_script_function(*::game::levelEntityId, filename, function_name, arguments));
					}
				);

				return detour;
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

			game_type["getentbyref"] = [](const game&, const sol::this_state s, 
				const unsigned int entnum, const unsigned int classnum)
			{
				const auto id = ::game::Scr_GetEntityId(entnum, classnum);
				if (id)
				{
					return convert(s, scripting::entity{id});
				}
				else
				{
					return sol::lua_value{s, sol::lua_nil};
				}
			};

			static uint64_t task_id = 0;

			state["http"] = sol::table::create(state.lua_state());

			state["http"]["get"] = [](const sol::this_state, const std::string& url,
				const sol::protected_function& callback, sol::variadic_args va)
			{
				bool async = false;

				if (va.size() >= 1 && va[0].get_type() == sol::type::boolean)
				{
					async = va[0].as<bool>();
				}

				const auto cur_task_id = task_id++;
				auto request_callbacks = &http_requests[cur_task_id];
				request_callbacks->on_load = callback;

				::scheduler::once([url, cur_task_id]()
				{
					if (http_requests.find(cur_task_id) == http_requests.end())
					{
						return;
					}

					const auto data = utils::http::get_data(url);
					::scheduler::once([data, cur_task_id]()
					{
						if (http_requests.find(cur_task_id) == http_requests.end())
						{
							return;
						}

						const auto& request_callbacks_ = http_requests[cur_task_id];
						const auto has_value = data.has_value();
						handle_error(request_callbacks_.on_load(has_value ? data.value().buffer : "", has_value));
						http_requests.erase(cur_task_id);
					}, ::scheduler::pipeline::server);
				}, ::scheduler::pipeline::async);
			};

			state["http"]["request"] = [](const sol::this_state s, const std::string& url, sol::variadic_args va)
			{
				auto request = sol::table::create(s.lua_state());

				std::string buffer{};
				std::string fields_string{};
				std::unordered_map<std::string, std::string> headers_map;

				if (va.size() >= 1 && va[0].get_type() == sol::type::table)
				{
					const auto options = va[0].as<sol::table>();

					const auto fields = options["parameters"];
					const auto body = options["body"];
					const auto headers = options["headers"];

					if (fields.get_type() == sol::type::table)
					{
						const auto _fields = fields.get<sol::table>();

						for (const auto& field : _fields)
						{
							const auto key = field.first.as<std::string>();
							const auto value = get_as_string(s, field.second);

							fields_string += key + "=" + value + "&";
						}
					}

					if (body.get_type() == sol::type::string)
					{
						fields_string = body.get<std::string>();
					}

					if (headers.get_type() == sol::type::table)
					{
						const auto _headers = headers.get<sol::table>();

						for (const auto& header : _headers)
						{
							const auto key = header.first.as<std::string>();
							const auto value = get_as_string(s, header.second);

							headers_map[key] = value;
						}
					}
				}

				request["onerror"] = []() {};
				request["onprogress"] = []() {};
				request["onload"] = []() {};

				request["send"] = [url, fields_string, request, headers_map]()
				{
					const auto cur_task_id = task_id++;
					auto request_callbacks = &http_requests[cur_task_id];
					request_callbacks->on_error = request["onerror"];
					request_callbacks->on_progress = request["onprogress"];
					request_callbacks->on_load = request["onload"];

					::scheduler::once([url, fields_string, cur_task_id, headers_map]()
					{
						if (http_requests.find(cur_task_id) == http_requests.end())
						{
							return;
						}

						const auto result = utils::http::get_data(url, fields_string, headers_map, [cur_task_id](size_t total, size_t value)
						{
							::scheduler::once([cur_task_id, value]()
							{
								if (http_requests.find(cur_task_id) == http_requests.end())
								{
									return;
								}

								const auto& request_callbacks_ = http_requests[cur_task_id];
								handle_error(request_callbacks_.on_progress(value));
							}, ::scheduler::pipeline::server);

							return 0;
						});

						::scheduler::once([cur_task_id, result]()
						{
							if (http_requests.find(cur_task_id) == http_requests.end())
							{
								return;
							}

							const auto& request_callbacks_ = http_requests[cur_task_id];

							if (!result.has_value())
							{
								request_callbacks_.on_error("Unknown", -1);
								return;
							}

							const auto& http_result = result.value();

							if (http_result.code == CURLE_OK)
							{
								handle_error(request_callbacks_.on_load(http_result.buffer));
							}
							else
							{
								handle_error(request_callbacks_.on_error(curl_easy_strerror(http_result.code), http_result.code));
							}

							http_requests.erase(cur_task_id);
						}, ::scheduler::pipeline::server);
					}, ::scheduler::pipeline::async);
				};

				return request;
			};
		}
	}

	context::context(std::string folder)
		: folder_(std::move(folder))
		  , scheduler_(state_)
		  , event_handler_(state_)

	{
		this->state_.open_libraries(sol::lib::base,
		                            sol::lib::package,
		                            sol::lib::io,
		                            sol::lib::string,
		                            sol::lib::os,
		                            sol::lib::math,
		                            sol::lib::table);

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

		setup_io(this->state_);
		setup_vector_type(this->state_);
		setup_entity_type(this->state_, this->event_handler_, this->scheduler_);

		printf("Loading script '%s'\n", this->folder_.data());
		this->load_script("__init__");
	}

	context::~context()
	{
		this->collect_garbage();
		this->scheduler_.clear();
		this->event_handler_.clear();
		this->state_ = {};
	}

	void context::run_frame()
	{
		this->scheduler_.run_frame();
		this->collect_garbage();
	}

	void context::notify(const event& e)
	{
		this->scheduler_.dispatch(e);
		this->event_handler_.dispatch(e);
	}

	void context::collect_garbage()
	{
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
