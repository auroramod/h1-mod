#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include "component/logfile.hpp"
#include "component/command.hpp"
#include "component/console.hpp"
#include "component/scripting.hpp"

#include "script_error.hpp"
#include "script_extension.hpp"
#include "script_loading.hpp"

#include <utils/hook.hpp>

namespace gsc
{
	std::uint16_t function_id_start = 0x30A;
	std::uint16_t method_id_start = 0x8586;

	builtin_function func_table[0x1000];
	builtin_method meth_table[0x1000];

	const game::dvar_t* developer_script = nullptr;

	namespace
	{
		std::unordered_map<std::uint16_t, script_function> functions;
		std::unordered_map<std::uint16_t, script_method> methods;

		bool force_error_print = false;
		std::optional<std::string> gsc_error_msg;
		game::scr_entref_t saved_ent_ref;

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
			const auto pos = game::scr_function_stack->pos;
			return *reinterpret_cast<std::uint16_t*>(
				reinterpret_cast<size_t>(pos - 2));
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
				scr_error(utils::string::va("builtin function \"%s\" doesn't exist", gsc_ctx->func_name(function_id).data()), true);
				return;
			}

			func();
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
				scr_error(utils::string::va("builtin method \"%s\" doesn't exist", gsc_ctx->meth_name(method_id).data()), true);
				return;
			}

			meth(saved_ent_ref);
		}

		void builtin_call_error(const std::string& error)
		{
			const auto function_id = get_function_id();

			if (function_id > 0x1000)
			{
				console::warn("in call to builtin method \"%s\"%s", gsc_ctx->meth_name(function_id).data(), error.data());
			}
			else
			{
				console::warn("in call to builtin function \"%s\"%s", gsc_ctx->func_name(function_id).data(), error.data());
			}
		}

		std::optional<std::string> get_opcode_name(const std::uint8_t opcode)
		{
			try
			{
				const auto index = gsc_ctx->opcode_enum(opcode);
				return { gsc_ctx->opcode_name(index) };
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
				const auto function = find_function(frame->fs.pos);

				if (function.has_value())
				{
					console::warn("\tat function \"%s\" in file \"%s.gsc\"\n", function.value().first.data(), function.value().second.data());
				}
				else
				{
					console::warn("\tat unknown location %p\n", pos);
				}
			}
		}

		void vm_error_stub(int mark_pos)
		{
			const bool dev_script = developer_script ? developer_script->current.enabled : false;
			if (!dev_script && !force_error_print)
			{
				utils::hook::invoke<void>(SELECT_VALUE(0x415C90_b, 0x59DDA0_b), mark_pos);
				return;
			}

			console::warn("*********** script runtime error *************\n");

			const auto opcode_id = *reinterpret_cast<std::uint8_t*>(SELECT_VALUE(0xC4015E8_b, 0xB7B8968_b));
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
			utils::hook::invoke<void*>(SELECT_VALUE(0x415C90_b, 0x59DDA0_b), mark_pos);
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
	}

	void scr_error(const char* error, const bool force_print)
	{
		force_error_print = force_print;
		gsc_error_msg = error;

		game::Scr_ErrorInternal();
	}

	namespace function
	{
		void add(const std::string& name, script_function function)
		{
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
	}

	namespace method
	{
		void add(const std::string& name, script_method method)
		{
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
	}

	function_args::function_args(std::vector<scripting::script_value> values)
		: values_(values)
	{
	}

	std::uint32_t function_args::size() const
	{
		return static_cast<std::uint32_t>(this->values_.size());
	}

	std::vector<scripting::script_value> function_args::get_raw() const
	{
		return this->values_;
	}

	scripting::value_wrap function_args::get(const int index) const
	{
		if (index >= this->values_.size())
		{
			throw std::runtime_error(utils::string::va("parameter %d does not exist", index));
		}

		return {this->values_[index], index};
	}

	class extension final : public component_interface
	{
	public:
		void post_unpack() override
		{
			developer_script = dvars::register_bool("developer_script", false, 0, "Enable developer script comments");

			utils::hook::set<uint32_t>(SELECT_VALUE(0x3BD86C_b, 0x50484C_b), 0x1000); // change builtin func count

			utils::hook::set<uint32_t>(SELECT_VALUE(0x3BD872_b, 0x504852_b) + 4,
				static_cast<uint32_t>(reverse_b((&func_table))));
			utils::hook::set<uint32_t>(SELECT_VALUE(0x3CB718_b, 0x512778_b) + 4,
				static_cast<uint32_t>(reverse_b((&func_table))));
			utils::hook::inject(SELECT_VALUE(0x3BDC28_b, 0x504C58_b) + 3, &func_table);
			utils::hook::set<uint32_t>(SELECT_VALUE(0x3BDC1E_b, 0x504C4E_b), sizeof(func_table));

			utils::hook::set<uint32_t>(SELECT_VALUE(0x3BD882_b, 0x504862_b) + 4,
				static_cast<uint32_t>(reverse_b((&meth_table))));
			utils::hook::set<uint32_t>(SELECT_VALUE(0x3CBA3B_b, 0x512A9B_b) + 4,
				static_cast<uint32_t>(reverse_b(&meth_table)));
			utils::hook::inject(SELECT_VALUE(0x3BDC36_b, 0x504C66_b) + 3, &meth_table);
			utils::hook::set<uint32_t>(SELECT_VALUE(0x3BDC3F_b, 0x504C6F_b), sizeof(meth_table));

			utils::hook::nop(SELECT_VALUE(0x3CB723_b, 0x512783_b), 8);
			utils::hook::call(SELECT_VALUE(0x3CB723_b, 0x512783_b), vm_call_builtin_function_stub);

			utils::hook::call(SELECT_VALUE(0x3CBA12_b, 0x512A72_b), get_entity_id_stub);
			utils::hook::nop(SELECT_VALUE(0x3CBA46_b, 0x512AA6_b), 6);
			utils::hook::nop(SELECT_VALUE(0x3CBA4E_b, 0x512AAE_b), 2);
			utils::hook::call(SELECT_VALUE(0x3CBA46_b, 0x512AA6_b), vm_call_builtin_method_stub);

			utils::hook::call(SELECT_VALUE(0x3CC9F3_b, 0x513A53_b), vm_error_stub); // LargeLocalResetToMark

			if (game::environment::is_dedi())
			{
				function::add("isusingmatchrulesdata", [](const function_args& args)
				{
					// return 0 so the game doesn't override the cfg
					return 0;
				});
			}

			function::add("print", [](const function_args& args)
			{
				print(args);
				return scripting::script_value{};
			});

			function::add("println", [](const function_args& args)
			{
				print(args);
				return scripting::script_value{};
			});

			function::add("assert", [](const function_args& args)
			{
				const auto expr = args[0].as<int>();
				if (!expr)
				{
					throw std::runtime_error("assert fail");
				}

				return scripting::script_value{};
			});

			function::add("assertex", [](const function_args& args)
			{
				const auto expr = args[0].as<int>();
				if (!expr)
				{
					const auto error = args[1].as<std::string>();
					throw std::runtime_error(error);
				}

				return scripting::script_value{};
			});

			function::add("getfunction", [](const function_args& args)
			{
				const auto filename = args[0].as<std::string>();
				const auto function = args[1].as<std::string>();

				if (!scripting::script_function_table[filename].contains(function))
				{
					throw std::runtime_error("function not found");
				}

				return scripting::function{scripting::script_function_table[filename][function]};
			});

			function::add("replacefunc", [](const function_args& args)
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

			function::add("toupper", [](const function_args& args)
			{
				const auto string = args[0].as<std::string>();
				return utils::string::to_upper(string);
			});

			function::add("logprint", [](const function_args& args)
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

			function::add("executecommand", [](const function_args& args)
			{
				command::execute(args[0].as<std::string>(), false);

				return scripting::script_value{};
			});

			function::add("typeof", typeof);
			function::add("type", typeof);

			if (!game::environment::is_sp())
			{
				function::add("say", [](const function_args& args)
				{
					const auto message = args[0].as<std::string>();
					game::SV_GameSendServerCommand(-1, game::SV_CMD_CAN_IGNORE, utils::string::va("%c \"%s\"", 84, message.data()));

					return scripting::script_value{};
				});

				method::add("tell", [](const game::scr_entref_t ent, const function_args& args)
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
			}
		}
	};
}

REGISTER_COMPONENT(gsc::extension)