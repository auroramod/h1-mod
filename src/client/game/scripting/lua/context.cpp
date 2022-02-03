#include <std_include.hpp>
#include "context.hpp"
#include "error.hpp"
#include "value_conversion.hpp"

#include "../execution.hpp"
#include "../functions.hpp"

#include "../../../component/command.hpp"
#include "../../../component/logfile.hpp"
#include "../../../component/scripting.hpp"

#include <utils/string.hpp>

namespace scripting::lua
{
	namespace
	{
		std::vector<std::string> load_game_constants()
		{
			std::vector<std::string> result{};

			const auto constants = game::GScr_LoadConsts.get();

			ud_t ud;
			ud_init(&ud);
			ud_set_mode(&ud, 64);
			ud_set_pc(&ud, uint64_t(constants));
			ud_set_input_buffer(&ud, reinterpret_cast<const uint8_t*>(constants), INT32_MAX);

			while (true)
			{
				ud_disassemble(&ud);

				if (ud_insn_mnemonic(&ud) == UD_Iret)
				{
					break;
				}

				if (ud_insn_mnemonic(&ud) == UD_Imov)
				{
					const auto* operand = ud_insn_opr(&ud, 0);
					if (operand && operand->type == UD_OP_REG && operand->base == UD_R_ECX)
					{
						operand = ud_insn_opr(&ud, 1);
						if (operand && operand->type == UD_OP_IMM && (operand->base == UD_R_RAX || operand->base == UD_R_EAX))
						{
							result.emplace_back(reinterpret_cast<const char**>(0x1409C1CE0)[operand->lval.udword]);
						}
					}
				}

				if (ud_insn_mnemonic(&ud) == UD_Ilea)
				{
					const auto* operand = ud_insn_opr(&ud, 0);
					if (!operand || operand->type != UD_OP_REG || operand->base != UD_R_RCX)
					{
						continue;
					}

					operand = ud_insn_opr(&ud, 1);
					if (operand && operand->type == UD_OP_MEM && operand->base == UD_R_RIP)
					{
						auto* operand_ptr = reinterpret_cast<char*>(ud_insn_len(&ud) + ud_insn_off(&ud) + operand->lval.
							sdword);
						if (!utils::memory::is_bad_read_ptr(operand_ptr) && utils::memory::is_rdata_ptr(operand_ptr) &&
							strlen(operand_ptr) > 0)
						{
							result.emplace_back(operand_ptr);
						}
					}
				}

				if (*reinterpret_cast<unsigned char*>(ud.pc) == 0xCC) break; // int 3
			}

			return result;
		}

		const std::vector<std::string>& get_game_constants()
		{
			static auto constants = load_game_constants();
			return constants;
		}

		void setup_entity_type(sol::state& state, event_handler& handler, scheduler& scheduler)
		{
			state["level"] = entity{*game::levelEntityId};

			auto vector_type = state.new_usertype<vector>("vector", sol::constructors<vector(float, float, float)>());
			vector_type["x"] = sol::property(&vector::get_x, &vector::set_x);
			vector_type["y"] = sol::property(&vector::get_y, &vector::set_y);
			vector_type["z"] = sol::property(&vector::get_z, &vector::set_z);

			vector_type["r"] = sol::property(&vector::get_x, &vector::set_x);
			vector_type["g"] = sol::property(&vector::get_y, &vector::set_y);
			vector_type["b"] = sol::property(&vector::get_z, &vector::set_z);

			auto entity_type = state.new_usertype<entity>("entity");

			for (const auto& func : method_map)
			{
				const auto name = utils::string::to_lower(func.first);
				entity_type[name.data()] = [name](const entity& entity, const sol::this_state s, sol::variadic_args va)
				{
					std::vector<script_value> arguments{};

					for (auto arg : va)
					{
						arguments.push_back(convert({s, arg}));
					}

					return convert(s, entity.call(name, arguments));
				};
			}

			for (const auto& constant : get_game_constants())
			{
				entity_type[constant] = sol::property(
					[constant](const entity& entity, const sol::this_state s)
					{
						return convert(s, entity.get(constant));
					},
					[constant](const entity& entity, const sol::this_state s, const sol::lua_value& value)
					{
						entity.set(constant, convert({s, value}));
					});
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
				return scripting::lua::entity_to_struct(s, id);
			};

			entity_type["struct"] = sol::property([](const entity& entity, const sol::this_state s)
			{
				const auto id = entity.get_entity_id();
				return scripting::lua::entity_to_struct(s, id);
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

			for (const auto& func : function_map)
			{
				const auto name = utils::string::to_lower(func.first);
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
				const auto id = *reinterpret_cast<unsigned int*>(0x14815DEB4);
				const auto value = ::game::scr_VarGlob->childVariableValue[id];

				::game::VariableValue variable{};
				variable.type = value.type;
				variable.u.uintValue = value.u.u.uintValue;

				return convert(s, variable);
			};

			game_type["getfunctions"] = [entity_type](const game&, const sol::this_state s, const std::string& filename)
			{
				if (scripting::script_function_table.find(filename) == scripting::script_function_table.end())
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

							gsl::finally(&logfile::enable_vm_execute_hook);
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

							gsl::finally(&logfile::enable_vm_execute_hook);
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

				gsl::finally(&logfile::enable_vm_execute_hook);
				logfile::disable_vm_execute_hook();

				return convert(s, call_script_function(*::game::levelEntityId, filename, function, arguments));
			};

			game_type["detour"] = [](const game&, const sol::this_state s, const std::string& filename,
				const std::string function_name, const sol::protected_function& function)
			{
				const auto pos = get_function_pos(filename, function_name);
				logfile::vm_execute_hooks[pos] = function;

				auto detour = sol::table::create(function.lua_state());

				detour["disable"] = [pos]()
				{
					logfile::vm_execute_hooks.erase(pos);
				};

				detour["enable"] = [pos, function]()
				{
					logfile::vm_execute_hooks[pos] = function;
				};

				detour["invoke"] = sol::overload(
					[filename, function_name](const entity& entity, const sol::this_state s, sol::variadic_args va)
					{
						std::vector<script_value> arguments{};

						for (auto arg : va)
						{
							arguments.push_back(convert({s, arg}));
						}

						gsl::finally(&logfile::enable_vm_execute_hook);
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

						gsl::finally(&logfile::enable_vm_execute_hook);
						logfile::disable_vm_execute_hook();

						return convert(s, call_script_function(*::game::levelEntityId, filename, function_name, arguments));
					}
				);

				return detour;
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
