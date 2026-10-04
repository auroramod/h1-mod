#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"
#include "game/scripting/array.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/functions.hpp"

#include "component/logfile.hpp"
#include "component/command.hpp"
#include "component/console.hpp"
#include "component/scripting.hpp"

#include "script_error.hpp"
#include "script_extension.hpp"
#include "script_loading.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>

using namespace gsc;

namespace gsc
{
	std::uint32_t function_args::size() const
	{
		return static_cast<std::uint32_t>(this->values_.size());
	}

	std::vector<scripting::script_value> function_args::get_raw() const
	{
		return this->values_;
	}

	function_args::function_args(std::vector<scripting::script_value> values)
		: values_(values)
	{
	}

	scripting::value_wrap function_args::get(const int index) const
	{
		if (index >= this->values_.size())
		{
			throw std::runtime_error(utils::string::va("parameter %d does not exist", index));
		}

		return {this->values_[index], index};
	}
}

namespace script_extension
{
	builtin_function func_table[0x1000];
	builtin_method meth_table[0x1000];
	const game::dvar_t* developer_script = nullptr;

	namespace
	{
		std::uint16_t function_id_start = 0;
		std::uint16_t method_id_start = 0x8586;

		std::unordered_map<std::uint16_t, script_function> functions;
		std::unordered_map<std::uint16_t, script_method> methods;

		bool force_error_print = false;
		std::optional<std::string> gsc_error_msg;
		game::scr_entref_t saved_ent_ref;

		thread_local std::uint16_t current_function_id;

		std::vector<devmap_entry> devmap_entries{};

		std::optional<devmap_entry> get_devmap_entry(const std::uint8_t* codepos)
		{
			const auto itr = std::ranges::find_if(devmap_entries, [codepos](const devmap_entry& entry) -> bool
			{
				return codepos >= entry.bytecode && codepos < entry.bytecode + entry.size;
			});

			if (itr != devmap_entries.end())
			{
				return *itr;
			}

			return {};
		}

		std::optional<std::pair<std::uint16_t, std::uint16_t>> get_line_and_col_for_codepos(const std::uint8_t* codepos)
		{
			const auto entry = get_devmap_entry(codepos);

			if (!entry.has_value())
			{
				return {};
			}

			std::optional<std::pair<std::uint16_t, std::uint16_t>> best_line_info{};
			std::uint32_t best_codepos = 0;

			assert(codepos >= entry->bytecode);
			const std::uint32_t codepos_offset = static_cast<std::uint32_t>(codepos - entry->bytecode);

			for (const auto& instruction : entry->devmap)
			{
				if (instruction.codepos > codepos_offset)
				{
					continue;
				}

				if (best_line_info.has_value() && codepos_offset - instruction.codepos > codepos_offset - best_codepos)
				{
					continue;
				}

				best_line_info = { { instruction.line, instruction.col } };
				best_codepos = instruction.codepos;
			}

			return best_line_info;
		}

		function_args get_arguments()
		{
			std::vector<scripting::script_value> args;

			for (auto i = 0; static_cast<std::uint32_t>(i) < game::scr_VmPub->outparamcount; ++i)
			{
				const auto value = game::scr_VmPub->top[-i];
				args.push_back(value);
			}

			return args;
		}

		void return_value(const scripting::script_value& value)
		{
			if (game::scr_VmPub->outparamcount)
			{
				game::Scr_ClearOutParams();
			}

			scripting::push_value(value);
		}

		std::uint16_t get_function_id()
		{
			if (!game::environment::is_sp())
			{
				return current_function_id;
			}

			const auto pos = game::scr_function_stack->pos;
			return *reinterpret_cast<std::uint16_t*>(
				reinterpret_cast<size_t>(pos - 2));
		}

		void set_function_id(std::uint32_t id)
		{
			current_function_id = static_cast<std::uint16_t>(id);
		}

		game::scr_entref_t get_entity_id_stub(std::uint32_t ent_id)
		{
			const auto ref = game::Scr_GetEntityIdRef(ent_id);
			saved_ent_ref = ref;
			return ref;
		}

		void execute_custom_function(const std::uint16_t id)
		{
			try
			{
				const auto& function = functions[id];
				const auto result = function(get_arguments());
				const auto type = result.get_raw().type;

				if (type)
				{
					return_value(result);
				}
			}
			catch (const std::exception& ex)
			{
				scr_error(ex.what());
			}
		}

		void vm_call_builtin_function_stub(builtin_function func)
		{
			const auto function_id = get_function_id();
			const auto custom = functions.contains(static_cast<std::uint16_t>(function_id));
			if (custom)
			{
				execute_custom_function(function_id);
				return;
			}

			if (func == nullptr)
			{
				scr_error(utils::string::va("builtin function \"%s\" doesn't exist", script_loading::gsc_ctx->func_name(function_id).data()), true);
				return;
			}

			func();
		}

		void vm_call_builtin_function_stub_mp()
		{
			const auto function_id = get_function_id();
			vm_call_builtin_function_stub(reinterpret_cast<builtin_function>(scripting::get_function_by_index(function_id)));
		}

		void execute_custom_method(const std::uint16_t id)
		{
			try
			{
				const auto& method = methods[id];
				const auto result = method(saved_ent_ref, get_arguments());
				const auto type = result.get_raw().type;

				if (type)
				{
					return_value(result);
				}
			}
			catch (const std::exception& ex)
			{
				scr_error(ex.what());
			}
		}

		void vm_call_builtin_method_stub(builtin_method meth)
		{
			const auto method_id = get_function_id();
			const auto custom = methods.contains(static_cast<std::uint16_t>(method_id));
			if (custom)
			{
				execute_custom_method(method_id);
				return;
			}

			if (meth == nullptr)
			{
				scr_error(utils::string::va("builtin method \"%s\" doesn't exist", script_loading::gsc_ctx->meth_name(method_id).data()), true);
				return;
			}

			meth(saved_ent_ref);
		}

		void vm_call_builtin_method_stub_mp()
		{
			const auto method_id = get_function_id();
			vm_call_builtin_method_stub(reinterpret_cast<builtin_method>(scripting::get_function_by_index(method_id)));
		}

		void builtin_call_error(const std::string& error)
		{
			const auto function_id = get_function_id();

			if (function_id > 0x1000)
			{
				console::warn("in call to builtin method \"%s\"%s", script_loading::gsc_ctx->meth_name(function_id).data(), error.data());
			}
			else
			{
				console::warn("in call to builtin function \"%s\"%s", script_loading::gsc_ctx->func_name(function_id).data(), error.data());
			}
		}

		std::optional<std::string> get_opcode_name(const std::uint8_t opcode)
		{
			try
			{
				const auto index = script_loading::gsc_ctx->opcode_enum(opcode);
				return { script_loading::gsc_ctx->opcode_name(index) };
			}
			catch (...)
			{
				return {};
			}
		}

		void print_callstack()
		{
			for (auto frame = game::scr_VmPub->function_frame; frame != game::scr_VmPub->function_frame_start; --frame)
			{
				const auto pos = frame == game::scr_VmPub->function_frame ? game::scr_function_stack->pos : frame->fs.pos;
				const auto function = script_error::find_function(frame->fs.pos);

				const char* location;
				if (function.has_value())
				{
					location = utils::string::va("function \"%s\" in file \"%s\"", function.value().first.data(), function.value().second.data());
				}
				else
				{
					location = utils::string::va("unknown location %p", pos);
				}

				const auto line_info = get_line_and_col_for_codepos(reinterpret_cast<const std::uint8_t*>(pos));
				if (line_info.has_value())
				{
					location = utils::string::va("%s (line %d col %d)", location, line_info->first, line_info->second);
				}

				console::warn("\tat %s\n", location);
			}
		}

		void vm_error_stub(int mark_pos)
		{
			const bool dev_script = developer_script ? developer_script->current.enabled : false;
			if (!dev_script && !force_error_print)
			{
				utils::hook::invoke<void>(SELECT_VALUE(0x140415C90, 0x1404F5B10), mark_pos);
				return;
			}

			console::warn("*********** script runtime error *************\n");

			const auto opcode_id = *reinterpret_cast<std::uint8_t*>(SELECT_VALUE(0x14C4015E8, 0x14A348FE8));
			const std::string error_str = gsc_error_msg.has_value()
				? utils::string::va(": %s", gsc_error_msg.value().data())
				: "";

			if ((opcode_id >= 0x1A && opcode_id <= 0x20) || (opcode_id >= 0xA9 && opcode_id <= 0xAF))
			{
				builtin_call_error(error_str);
			}
			else
			{
				const auto opcode = get_opcode_name(opcode_id);
				if (opcode.has_value())
				{
					console::warn("while processing instruction %s%s\n", opcode.value().data(), error_str.data());
				}
				else
				{
					console::warn("while processing instruction 0x%X%s\n", opcode_id, error_str.data());
				}
			}

			force_error_print = false;
			gsc_error_msg = {};

			print_callstack();
			console::warn("**********************************************\n");
			utils::hook::invoke<void*>(SELECT_VALUE(0x140415C90, 0x1404F5B10), mark_pos);
		}

		void print(const function_args& args)
		{
			std::string buffer{};

			for (auto i = 0u; i < args.size(); ++i)
			{
				const auto str = args[i].to_string();
				buffer.append(str);
				buffer.append("\t");
			}
			console::info("%s\n", buffer.data());
		}

		scripting::script_value typeof(const function_args& args)
		{
			return args[0].type_name();
		}

		void* store_func_id_stub()
		{
			// CallBuiltin: ecx holds the id read from the bytecode
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.pushad64();
				a.call_aligned(set_function_id);
				a.popad64();

				a.jmp(0x140445BF6);
			});
		}

		void* store_func_id_pointer_stub()
		{
			// CallBuiltinPointer: ecx holds the id read from the stack
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.pushad64();
				a.call_aligned(set_function_id);
				a.popad64();

				a.sub(rsi, 0x10);
				a.mov(dword_ptr(rsp, 0x60), ecx);

				a.jmp(0x140445BEF);
			});
		}

		void* store_method_id_stub()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.pushad64();
				a.mov(ecx, edi);
				a.call_aligned(set_function_id);
				a.popad64();

				// original code
				a.mov(ecx, r12d);
				a.mov(dword_ptr(rbp, 0x7C), eax);
				a.mov(ebx, eax);
				a.call(0x14043DF80); // RemoveRefToObject

				a.jmp(0x140445F18);
			});
		}

		void add_1_15_builtins()
		{
			functions[0x2FB] = [](const function_args& args) -> scripting::script_value // isweaponsilenced (weapon)
			{
				// TODO improve, 1.04 has no isweaponsilenced and doesnt check weapondef like 1.15 does
				const auto weapon = args[0].as<std::string>();
				return weapon.find("silence") != std::string::npos ? 1 : 0;
			};

			for (const auto id : {0x2FC, 0x2FD, 0x2FE, 0x2FF, 0x300, 0x301})
			{
				functions[static_cast<std::uint16_t>(id)] = [](const function_args&)
				{
					// random nullsubs......
					return scripting::script_value{};
				};
			}

			functions[0x302] = [](const function_args&) -> scripting::script_value // getnumberofclients
			{
				return scripting::call_function("getdvarint", {"sv_maxclients"});
			};

			functions[0x303] = [](const function_args& args) // tablelookup
			{
				return scripting::call_function("tablelookup", args.get_raw());
			};

			functions[0x304] = [](const function_args& args) // tablelookuprownum
			{
				auto values = args.get_raw();
				values.resize(std::min(values.size(), std::size_t(3)));
				return scripting::call_function("tablelookuprownum", values);
			};

			functions[0x305] = [](const function_args& args) -> scripting::script_value // currency balance (controller, type)
			{
				static const char* names[] = {nullptr, "launchCredits", "credits", "parts", "codPoints", "bonus"};
				const auto type = args[1].as<int>();
				if (type <= 0 || type >= static_cast<int>(std::size(names)))
				{
					return 0;
				}

				std::string data{};
				if (!utils::io::read_file("players2/user/depot.json", &data))
				{
					return 0;
				}

				const auto json = nlohmann::json::parse(data, nullptr, false);
				if (json.is_discarded() || !json.contains("currencies") || !json["currencies"].contains(names[type]))
				{
					return 0;
				}

				return json["currencies"][names[type]].get<int>();
			};

			functions[0x306] = [](const function_args&) -> scripting::script_value // supply drop count (controller, type)
			{
				return 0;
			};

			functions[0x307] = [](const function_args&) -> scripting::script_value // bundle price info (name)
			{
				scripting::array price{};
				price.set(std::string("amount"), 999999);
				return price;
			};

			functions[0x2FA] = [](const function_args& args) -> scripting::script_value // getcacplayerdataforgroup
			{
				const auto values = args.get_raw();
				if (!values.empty() && values.back().is<std::string>() && values.back().as<std::string>() == "hasEverVisitedDepot")
				{
					// playerdata has no hasEverVisitedDepot, so this covers it
					return utils::io::file_exists("players2/user/depot_visited") ? 1 : 0;
				}

				return scripting::call_function("getcacplayerdataforgroup", values);
			};

			functions[0x308] = [](const function_args& args) // setplayerdataforgroup (controller, group, path?, value)
			{
				const auto values = args.get_raw();
				if (values.size() >= 2 && values[values.size() - 2].is<std::string>() && values[values.size() - 2].as<std::string>() == "hasEverVisitedDepot")
				{
					// ^ same as above
					if (values.back().is<int>() && values.back().as<int>())
					{
						utils::io::write_file("players2/user/depot_visited", "1", false);
					}
					else
					{
						utils::io::remove_file("players2/user/depot_visited");
					}
				}

				return scripting::script_value{};
			};

			functions[0x309] = [](const function_args&) -> scripting::script_value // inpartywithotherplayers
			{
				return 0;
			};

			functions[0x30A] = [](const function_args&) -> scripting::script_value // getglasspieces
			{
				return scripting::array{};
			};

			methods[0x8487] = [](const game::scr_entref_t ent_ref, const function_args& args) // scriptmodelplayanimdeltamotionfrompos
			{
				// TODO: blend trees are not ported, the 5th arg is ignored (1.15 doesn't do this though, so check it out)
				const auto count = game::scr_VmPub->outparamcount;
				game::scr_VmPub->outparamcount = std::min(count, 4u);
				reinterpret_cast<void(*)(game::scr_entref_t)>(0x140380630)(ent_ref);
				game::scr_VmPub->outparamcount = count;
				return scripting::script_value{};
			};

			methods[0x8583] = [](const game::scr_entref_t, const function_args&)
			{
				return scripting::script_value{}; // some hodgepodge only thing
			};

			methods[0x8584] = [](const game::scr_entref_t, const function_args&)
			{
				return scripting::script_value{}; // idk
			};

			methods[0x8585] = [](const game::scr_entref_t, const function_args&) -> scripting::script_value
			{
				return 1; // has stats
			};

			methods[0x8586] = [](const game::scr_entref_t, const function_args&) -> scripting::script_value 
			{
				return 1; // matchmaking ranked check
			};
		}
	}

	void add_devmap_entry(std::uint8_t* codepos, std::size_t size, const std::string& name, xsk::gsc::buffer devmap_buf)
	{
		std::vector<dev_map_instruction> devmap{};
		const auto* devmap_ptr = reinterpret_cast<const dev_map*>(devmap_buf.data);

		devmap.resize(devmap_ptr->num_instructions);
		std::memcpy(devmap.data(), devmap_ptr->instructions, sizeof(dev_map_instruction) * devmap_ptr->num_instructions);

		devmap_entries.emplace_back(codepos, size, name, std::move(devmap));
	}

	void clear_devmap()
	{
		devmap_entries.clear();
	}

	void scr_error(const char* error, const bool force_print)
	{
		force_error_print = force_print;
		gsc_error_msg = error;

		game::Scr_ErrorInternal();
	}

	void add_function(const std::string& name, script_function function)
	{
		const auto& gsc_ctx = script_loading::gsc_ctx;
		if (gsc_ctx->func_exists(name))
		{
			const auto id = gsc_ctx->func_id(name);
			functions[id] = function;
		}
		else
		{
			const auto id = ++function_id_start;
			gsc_ctx->func_add(name, static_cast<std::uint16_t>(id));
			functions[id] = function;
		}
	}

	void add_method(const std::string& name, script_method method)
	{
		const auto& gsc_ctx = script_loading::gsc_ctx;
		if (gsc_ctx->meth_exists(name))
		{
			const auto id = gsc_ctx->meth_id(name);
			methods[id] = method;
		}
		else
		{
			const auto id = ++method_id_start;
			gsc_ctx->meth_add(name, static_cast<std::uint16_t>(id));
			methods[id] = method;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			function_id_start = 0x30A;

			developer_script = dvars::register_bool("developer_script", false, 0, "Enable developer script comments");

			if (game::environment::is_sp())
			{
				utils::hook::set<uint32_t>(0x1403BD86C, 0x1000); // change builtin func count

				utils::hook::set<uint32_t>(0x1403BD872 + 4, RVA(&func_table));
				utils::hook::set<uint32_t>(0x1403CB718 + 4, RVA(&func_table));
				utils::hook::inject(0x1403BDC28 + 3, &func_table);
				utils::hook::set<uint32_t>(0x1403BDC1E, sizeof(func_table));

				utils::hook::set<uint32_t>(0x1403BD882 + 4, RVA(&meth_table));
				utils::hook::set<uint32_t>(0x1403CBA3B + 4, RVA(&meth_table));
				utils::hook::inject(0x1403BDC36 + 3, &meth_table);
				utils::hook::set<uint32_t>(0x1403BDC3F, sizeof(meth_table));

				utils::hook::nop(0x1403CB723, 8);
				utils::hook::call(0x1403CB723, vm_call_builtin_function_stub);

				utils::hook::call(0x1403CBA12, get_entity_id_stub);
				utils::hook::nop(0x1403CBA46, 6);
				utils::hook::nop(0x1403CBA4E, 2);
				utils::hook::call(0x1403CBA46, vm_call_builtin_method_stub);

				utils::hook::call(0x1403CC9F3, vm_error_stub); // LargeLocalResetToMark
			}
			else
			{
				utils::hook::set<uint32_t>(0x140437CEC, 0x1000); // change builtin func count

				utils::hook::set<uint32_t>(0x140437CF2 + 4, RVA(&func_table)); // Scr_RegisterFunction
				utils::hook::inject(0x1404380F8 + 3, &func_table); // Scr_BeginLoadScripts
				utils::hook::set<uint32_t>(0x1404380EC + 2, sizeof(func_table)); // memset size

				utils::hook::set<uint32_t>(0x140437D02 + 4, RVA(&meth_table)); // Scr_RegisterFunction
				utils::hook::inject(0x140438106 + 3, &meth_table); // Scr_BeginLoadScripts
				utils::hook::set<uint32_t>(0x14043810D + 2, sizeof(meth_table)); // memset size

				// the VM call sites index the tables directly, we look the function up ourselves
				utils::hook::nop(0x140445C1D, 2);
				utils::hook::call(0x140445C18, vm_call_builtin_function_stub_mp);

				utils::hook::call(0x140445F06, get_entity_id_stub);
				utils::hook::nop(0x140445F36, 2);
				utils::hook::call(0x140445F31, vm_call_builtin_method_stub_mp);

				// store the function/method id, the pointer call path has no id in the bytecode
				utils::hook::jump(0x140445F0B, store_method_id_stub(), false);
				utils::hook::jump(0x1404456F8, store_func_id_stub(), false);
				utils::hook::jump(0x140445BE7, store_func_id_pointer_stub(), false);
				utils::hook::nop(0x140445BE7 + 5, 3);

				utils::hook::call(0x140446EE3, vm_error_stub); // LargeLocalResetToMark

				add_1_15_builtins();
			}

			if (game::environment::is_dedi())
			{
				add_function("isusingmatchrulesdata", [](const function_args& args)
				{
					// return 0 so the game doesn't override the cfg
					return 0;
				});
			}

			add_function("print", [](const function_args& args)
			{
				print(args);
				return scripting::script_value{};
			});

			add_function("println", [](const function_args& args)
			{
				print(args);
				return scripting::script_value{};
			});

			add_function("assert", [](const function_args& args)
			{
				const auto expr = args[0].as<int>();
				if (!expr)
				{
					throw std::runtime_error("assert fail");
				}

				return scripting::script_value{};
			});

			add_function("assertex", [](const function_args& args)
			{
				const auto expr = args[0].as<int>();
				if (!expr)
				{
					const auto error = args[1].as<std::string>();
					throw std::runtime_error(error);
				}

				return scripting::script_value{};
			});

			add_function("getfunction", [](const function_args& args)
			{
				const auto filename = args[0].as<std::string>();
				const auto function = args[1].as<std::string>();

				if (!scripting::script_function_table[filename].contains(function))
				{
					throw std::runtime_error("function not found");
				}

				return scripting::function{scripting::script_function_table[filename][function]};
			});

			add_function("replacefunc", [](const function_args& args)
			{
				const auto what = args[0].get_raw();
				const auto with = args[1].get_raw();

				if (what.type != game::VAR_FUNCTION || with.type != game::VAR_FUNCTION)
				{
					throw std::runtime_error("replacefunc: parameter 1 must be a function");
				}

				logfile::set_gsc_hook(what.u.codePosValue, with.u.codePosValue);

				return scripting::script_value{};
			});

			add_function("toupper", [](const function_args& args)
			{
				const auto string = args[0].as<std::string>();
				return utils::string::to_upper(string);
			});

			add_function("logprint", [](const function_args& args)
			{
				std::string buffer{};

				for (auto i = 0u; i < args.size(); ++i)
				{
					const auto string = args[i].as<std::string>();
					buffer.append(string);
				}

				game::G_LogPrintf("%s", buffer.data());

				return scripting::script_value{};
			});

			add_function("executecommand", [](const function_args& args)
			{
				command::execute(args[0].as<std::string>(), false);

				return scripting::script_value{};
			});

			add_function("typeof", typeof);
			add_function("type", typeof);

			if (!game::environment::is_sp())
			{
				add_function("say", [](const function_args& args)
				{
					const auto message = args[0].as<std::string>();
					game::SV_GameSendServerCommand(-1, game::SV_CMD_CAN_IGNORE, utils::string::va("%c \"%s\"", 84, message.data()));

					return scripting::script_value{};
				});

				add_method("tell", [](const game::scr_entref_t ent, const function_args& args)
				{
					if (ent.classnum != 0)
					{
						throw std::runtime_error("Invalid entity");
					}

					const auto client = ent.entnum;

					if (game::mp::g_entities[client].client == nullptr)
					{
						throw std::runtime_error("Not a player entity");
					}

					const auto message = args[0].as<std::string>();
					game::SV_GameSendServerCommand(client, game::SV_CMD_CAN_IGNORE, utils::string::va("%c \"%s\"", 84, message.data()));

					return scripting::script_value{};
				});

				add_function("lookupsoundlength", [](const function_args& args) // sp already has this function, implement for mp
				{
					if (args.size() == 1)
					{
						const auto sound_name = args[0].as<std::string>();
						const auto sound_length = game::SND_SV_LookupSoundLength(sound_name.data());
						return scripting::script_value{sound_length};
					}

					return scripting::script_value{0};
				});
			}
		}
	};
}

REGISTER_COMPONENT(script_extension::component)
