#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "fastfiles.hpp"
#include "filesystem.hpp"
#include "logfile.hpp"
#include "gsc.hpp"
#include "scripting.hpp"

#include "game/dvars.hpp"

#include <xsk/gsc/types.hpp>
#include <xsk/gsc/interfaces/compiler.hpp>
#include <xsk/gsc/interfaces/decompiler.hpp>
#include <xsk/gsc/interfaces/assembler.hpp>
#include <xsk/gsc/interfaces/disassembler.hpp>
#include <xsk/utils/compression.hpp>
#include <xsk/resolver.hpp>
#include <interface.hpp>

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace gsc
{
	builtin_function func_table[0x1000]{};
	builtin_method meth_table[0x1000]{};

	namespace
	{
		utils::hook::detour scr_emit_function_hook;

		game::dvar_t* developer_script = nullptr;

		auto compiler = ::gsc::compiler();
		auto decompiler = ::gsc::decompiler();
		auto assembler = ::gsc::assembler();
		auto disassembler = ::gsc::disassembler();

		std::unordered_map<std::string, unsigned int> main_handles;
		std::unordered_map<std::string, unsigned int> init_handles;
		std::unordered_map<std::string, game::ScriptFile*> loaded_scripts;

		std::unordered_map<unsigned int, script_function> functions;
		std::unordered_map<unsigned int, script_method> methods;

		std::optional<std::string> gsc_error;

		bool force_error_print = false;

		auto function_id_start = 0x30A;
		auto method_id_start = 0x8586;

		game::scr_entref_t saved_ent_ref;

		std::string unknown_function_error{};
		unsigned int current_filename{};

		char* allocate_buffer(size_t size)
		{
			// PMem_AllocFromSource_NoDebug
			return utils::hook::invoke<char*>(SELECT_VALUE(0x41FB50_b, 0x5A4DC0_b), size, 4, 1, 5);
		}

		bool read_scriptfile(const std::string& name, std::string* data)
		{
			if (filesystem::read_file(name, data))
			{
				return true;
			}

			// TODO: check back on this to see if there is a property we can distinguish compared to our rawfiles, like compressedLen?
			// this will filter out the rawfile "gsc" the game zones actually have, this seems to get all of them
			if (name.starts_with("maps/createfx") || name.starts_with("maps/createart")
				|| (name.starts_with("maps/mp") && name.ends_with("_fx.gsc")))
			{
				return false;
			}

			const auto name_str = name.data();

			if (game::DB_XAssetExists(game::ASSET_TYPE_RAWFILE, name_str) &&
				!game::DB_IsXAssetDefault(game::ASSET_TYPE_RAWFILE, name_str))
			{
				const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_RAWFILE, name_str, false);
				const auto len = game::DB_GetRawFileLen(asset.rawfile);
				data->resize(len);
				game::DB_GetRawBuffer(asset.rawfile, data->data(), len);
				if (len > 0)
				{
					data->pop_back();
				}

				return true;
			}

			return false;
		}

		game::ScriptFile* load_custom_script(const char* file_name, const std::string& real_name)
		{
			if (loaded_scripts.find(real_name) != loaded_scripts.end())
			{
				return loaded_scripts[real_name];
			}

			std::string source_buffer{};
			if (!read_scriptfile(real_name + ".gsc", &source_buffer))
			{
				return nullptr;
			}

			auto data = std::vector<uint8_t>{source_buffer.begin(), source_buffer.end()};

			try
			{
				compiler->compile(real_name, data);
			}
			catch (const std::exception& e)
			{
				console::error("*********** script compile error *************\n");
				console::error("failed to compile '%s':\n%s", real_name.data(), e.what());
				console::error("**********************************************\n");
				return nullptr;
			}

			auto assembly = compiler->output();

			try
			{
				assembler->assemble(real_name, assembly);
			}
			catch (const std::exception& e)
			{
				console::error("*********** script compile error *************\n");
				console::error("failed to assemble '%s':\n%s", real_name.data(), e.what());
				console::error("**********************************************\n");
				return nullptr;
			}

			const auto script_file_ptr = reinterpret_cast<game::ScriptFile*>(allocate_buffer(sizeof(game::ScriptFile)));
			script_file_ptr->name = file_name;

			const auto stack = assembler->output_stack();
			script_file_ptr->len = static_cast<int>(stack.size());

			const auto script = assembler->output_script();
			script_file_ptr->bytecodeLen = static_cast<int>(script.size());

			const auto script_size = script.size();
			const auto buffer_size = script_size + stack.size() + 2;

			const auto buffer = allocate_buffer(buffer_size);
			std::memcpy(buffer, script.data(), script_size);
			std::memcpy(&buffer[script_size], stack.data(), stack.size());

			script_file_ptr->bytecode = &buffer[0];
			script_file_ptr->buffer = &buffer[script.size()];
			script_file_ptr->compressedLen = 0;

			loaded_scripts[real_name] = script_file_ptr;

			return script_file_ptr;
		}

		std::string get_script_file_name(const std::string& name)
		{
			const auto id = xsk::gsc::h1::resolver::token_id(name);
			if (!id)
			{
				return name;
			}

			return std::to_string(id);
		}

		std::vector<std::uint8_t> decompile_script_file(const std::string& name, const std::string& real_name)
		{
			const auto* script_file = game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, name.data(), false).scriptfile;
			if (!script_file)
			{
				throw std::runtime_error(std::format("Could not load scriptfile '{}'", real_name));
			}

			console::info("Decompiling scriptfile '%s'\n", real_name.data());

			std::vector<std::uint8_t> stack{script_file->buffer, script_file->buffer + script_file->len};
			std::vector<std::uint8_t> bytecode{script_file->bytecode, script_file->bytecode + script_file->bytecodeLen};

			auto decompressed_stack = xsk::utils::zlib::decompress(stack, static_cast<std::uint32_t>(stack.size()));

			disassembler->disassemble(name, bytecode, decompressed_stack);
			auto output = disassembler->output();

			decompiler->decompile(name, output);

			return decompiler->output();
		}

		void load_script(const std::string& name)
		{
			if (!game::Scr_LoadScript(name.data()))
			{
				return;
			}

			const auto main_handle = game::Scr_GetFunctionHandle(name.data(),
				xsk::gsc::h1::resolver::token_id("main"));
			const auto init_handle = game::Scr_GetFunctionHandle(name.data(),
				xsk::gsc::h1::resolver::token_id("init"));

			if (main_handle)
			{
				console::info("Loaded '%s::main'\n", name.data());
				main_handles[name] = main_handle;
			}

			if (init_handle)
			{
				console::info("Loaded '%s::init'\n", name.data());
				init_handles[name] = init_handle;
			}
		}

		void load_scripts(const std::filesystem::path& root_dir, const std::filesystem::path& script_dir)
		{
			std::filesystem::path script_dir_path = root_dir / script_dir;
			if (!utils::io::directory_exists(script_dir_path.generic_string()))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir_path.generic_string());
			for (const auto& script : scripts)
			{
				if (!script.ends_with(".gsc"))
				{
					continue;
				}

				std::filesystem::path path(script);
				const auto relative = path.lexically_relative(root_dir).generic_string();
				const auto base_name = relative.substr(0, relative.size() - 4);

				load_script(base_name);
			}
		}

		void clear()
		{
			main_handles.clear();
			init_handles.clear();
			loaded_scripts.clear();
		}

		void load_gametype_script_stub(void* a1, void* a2)
		{
			utils::hook::invoke<void>(SELECT_VALUE(0x2B9DA0_b, 0x18BC00_b), a1, a2);

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

			clear();

			for (const auto& path : filesystem::get_search_paths())
			{
				load_scripts(path, "scripts/");
				if (game::environment::is_sp())
				{
					load_scripts(path, "scripts/sp/");
				}
				else
				{
					load_scripts(path, "scripts/mp/");
				}
			}
		}

		int db_is_xasset_default(game::XAssetType type, const char* name)
		{
			if (loaded_scripts.find(name) != loaded_scripts.end())
			{
				return 0;
			}

			return game::DB_IsXAssetDefault(type, name);
		}

		void db_get_raw_buffer_stub(const game::RawFile* rawfile, char* buf, const int size)
		{
			if (rawfile->len > 0 && rawfile->compressedLen == 0)
			{
				std::memset(buf, 0, size);
				std::memcpy(buf, rawfile->buffer, std::min(rawfile->len, size));
				return;
			}

			utils::hook::invoke<void>(SELECT_VALUE(0x1F1E00_b, 0x396080_b), rawfile, buf, size);
		}

		std::optional<std::pair<std::string, std::string>> find_function(const char* pos)
		{
			for (const auto& file : scripting::script_function_table_sort)
			{
				for (auto i = file.second.begin(); i != file.second.end() && std::next(i) != file.second.end(); ++i)
				{
					const auto next = std::next(i);
					if (pos >= i->second && pos < next->second)
					{
						return {std::make_pair(i->first, file.first)};
					}
				}
			}

			return {};
		}

		void print_callstack()
		{
			for (auto frame = game::scr_VmPub->function_frame; frame != game::scr_VmPub->function_frame_start; --frame)
			{
				const auto pos = frame == game::scr_VmPub->function_frame 
					? game::scr_function_stack->pos 
					: frame->fs.pos;
				const auto function = find_function(pos);

				if (function.has_value())
				{
					console::warn("\tat function \"%s\" in file \"%s.gsc\"", 
						function.value().first.data(), function.value().second.data());
				}
				else
				{
					console::warn("\tat unknown location %p", pos);
				}
			}
		}

		std::optional<std::string> get_opcode_name(const std::uint8_t opcode)
		{
			try
			{
				return {xsk::gsc::h1::resolver::opcode_name(opcode)};
			}
			catch (...)
			{
				return {};
			}
		}

		std::uint16_t get_function_id()
		{
			const auto pos = game::scr_function_stack->pos;
			return *reinterpret_cast<std::uint16_t*>(
				reinterpret_cast<size_t>(pos - 2));
		}

		void builtin_call_error(const std::string& error_str)
		{
			const auto function_id = get_function_id();

			if (function_id > 0x1000)
			{
				console::warn("in call to builtin method \"%s\"%s",
					xsk::gsc::h1::resolver::method_name(function_id).data(), error_str.data());
			}
			else
			{
				console::warn("in call to builtin function \"%s\"%s",
					xsk::gsc::h1::resolver::function_name(function_id).data(), error_str.data());
			}
		}

		void vm_error_stub(void* a1)
		{
			if (!developer_script->current.enabled && !force_error_print)
			{
				utils::hook::invoke<void>(SELECT_VALUE(0x415C90_b, 0x59DDA0_b), a1);
				return;
			}

			console::warn("*********** script runtime error *************\n");

			const auto opcode_id = *reinterpret_cast<std::uint8_t*>(SELECT_VALUE(0xC4015E8_b, 0xB7B8968_b));
			const std::string error_str = gsc_error.has_value()
				? utils::string::va(": %s", gsc_error.value().data())
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
					console::warn("while processing instruction %s%s\n",
						opcode.value().data(), error_str.data());
				}
				else
				{
					console::warn("while processing instruction 0x%X%s\n",
						opcode_id, error_str.data());
				}
			}

			force_error_print = false;
			gsc_error = {};

			print_callstack();
			console::warn("**********************************************\n");
			utils::hook::invoke<void*>(SELECT_VALUE(0x415C90_b, 0x59DDA0_b), a1);
		}

		void get_unknown_function_error(const char* code_pos)
		{
			const auto function = find_function(code_pos);
			if (function.has_value())
			{
				const auto& pos = function.value();
				unknown_function_error = utils::string::va(
					"while processing function '%s' in script '%s':\nunknown script '%s'",
					pos.first.data(), pos.second.data(), scripting::current_file.data()
				);
			}
			else
			{
				unknown_function_error = utils::string::va(
					"unknown script '%s'",
					scripting::current_file.data()
				);
			}
		}

		std::string get_filename_name()
		{
			const auto filename_str = game::SL_ConvertToString(
				static_cast<game::scr_string_t>(current_filename));
			const auto id = std::atoi(filename_str);
			if (id == 0)
			{
				return filename_str;
			}

			return scripting::get_token(id);
		}

		void get_unknown_function_error(unsigned int thread_name)
		{
			const auto filename = get_filename_name();
			const auto name = scripting::get_token(thread_name);

			unknown_function_error = utils::string::va(
				"while processing script '%s':\nunknown function '%s::%s'",
				scripting::current_file.data(), filename.data(), name.data()
			);
		}

		void unknown_function_stub(const char* code_pos)
		{
			get_unknown_function_error(code_pos);
			game::Com_Error(game::ERR_DROP, "script link error\n%s",
				unknown_function_error.data());
		}

		unsigned int find_variable_stub(unsigned int parent_id, unsigned int thread_name)
		{
			const auto res = game::FindVariable(parent_id, thread_name);
			if (!res)
			{
				get_unknown_function_error(thread_name);
				game::Com_Error(game::ERR_DROP, "script link error\n%s",
					unknown_function_error.data());
			}
			return res;
		}

		void scr_emit_function_stub(unsigned int filename, unsigned int thread_name, char* code_pos)
		{
			current_filename = filename;
			scr_emit_function_hook.invoke<void>(filename, thread_name, code_pos);
		}

		function_args get_arguments()
		{
			std::vector<scripting::script_value> args;

			for (auto i = 0; static_cast<unsigned int>(i) < game::scr_VmPub->outparamcount; ++i)
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

		void call_custom_function(const uint16_t id)
		{
			auto error = false;

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
			catch (const std::exception& e)
			{
				error = true;
				force_error_print = true;
				gsc_error = e.what();
			}

			if (error)
			{
				game::Scr_ErrorInternal();
			}
		}

		void call_custom_method(const uint16_t id)
		{
			auto error = false;

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
			catch (const std::exception& e)
			{
				error = true;
				force_error_print = true;
				gsc_error = e.what();
			}

			if (error)
			{
				game::Scr_ErrorInternal();
			}
		}

		void vm_call_builtin_function_stub(builtin_function function)
		{
			const auto function_id = get_function_id();
			const auto is_custom_function = functions.find(function_id) != functions.end();

			if (!is_custom_function)
			{
				function();
			}
			else
			{
				call_custom_function(function_id);
			}
		}

		game::scr_entref_t get_entity_id_stub(unsigned int ent_id)
		{
			const auto ref = game::Scr_GetEntityIdRef(ent_id);
			saved_ent_ref = ref;
			return ref;
		}

		void vm_call_builtin_method_stub(builtin_method method)
		{
			const auto function_id = get_function_id();
			const auto is_custom_function = methods.find(function_id) != methods.end();

			if (!is_custom_function)
			{
				method(saved_ent_ref);
			}
			else
			{
				call_custom_method(function_id);
			}
		}
	}

	game::ScriptFile* find_script(game::XAssetType /*type*/, const char* name, int /*allow_create_default*/)
	{
		if (game::VirtualLobby_Loaded())
		{
			return game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, name, 1).scriptfile;
		}

		std::string real_name = name;
		const auto id = static_cast<std::uint16_t>(std::atoi(name));
		if (id)
		{
			real_name = xsk::gsc::h1::resolver::token_name(id);
		}

		auto* script = load_custom_script(name, real_name);
		if (script)
		{
			return script;
		}

		return game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, name, 1).scriptfile;
	}

	void load_main_handles()
	{
		for (auto& function_handle : main_handles)
		{
			console::info("Executing '%s::main'\n", function_handle.first.data());
			game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
		}
	}

	void load_init_handles()
	{
		for (auto& function_handle : init_handles)
		{
			console::info("Executing '%s::init'\n", function_handle.first.data());
			game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
		}
	}

	namespace function
	{
		void add(const std::string& name, script_function function)
		{
			if (xsk::gsc::h1::resolver::find_function(name))
			{
				const auto id = xsk::gsc::h1::resolver::function_id(name);
				functions[id] = function;
			}
			else
			{
				const auto id = ++function_id_start;
				xsk::gsc::h1::resolver::add_function(name, static_cast<std::uint16_t>(id));
				functions[id] = function;
			}
		}
	}

	namespace method
	{
		void add(const std::string& name, script_method method)
		{
			if (xsk::gsc::h1::resolver::find_method(name))
			{
				const auto id = xsk::gsc::h1::resolver::method_id(name);
				methods[id] = method;
			}
			else
			{
				const auto id = ++method_id_start;
				xsk::gsc::h1::resolver::add_method(name, static_cast<std::uint16_t>(id));
				methods[id] = method;
			}
		}
	}

	function_args::function_args(std::vector<scripting::script_value> values)
		: values_(values)
	{
	}

	unsigned int function_args::size() const
	{
		return static_cast<unsigned int>(this->values_.size());
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

		return { this->values_[index], index };
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			developer_script = dvars::register_bool("developer_script", false, 0, "Print GSC errors");

			// Allow custom scripts to include other custom scripts
			xsk::gsc::h1::resolver::init([](const auto& include_name)
			{
				const auto real_name = include_name + ".gsc";

				std::string file_buffer;
				if (!read_scriptfile(real_name, &file_buffer) || file_buffer.empty())
				{
					const auto name = get_script_file_name(include_name);
					if (game::DB_XAssetExists(game::ASSET_TYPE_SCRIPTFILE, name.data()))
					{
						return decompile_script_file(name, real_name);
					}
					else
					{
						throw std::runtime_error(std::format("Could not load gsc file '{}'", real_name));
					}
				}

				std::vector<std::uint8_t> result;
				result.assign(file_buffer.begin(), file_buffer.end());

				return result;
			});

			// hook xasset functions to return our own custom scripts
			utils::hook::call(SELECT_VALUE(0x3C7217_b, 0x50E357_b), find_script);
			utils::hook::call(SELECT_VALUE(0x3C7227_b, 0x50E367_b), db_is_xasset_default);

			// loads scripts with an uncompressed stack
			utils::hook::call(SELECT_VALUE(0x3C7280_b, 0x50E3C0_b), db_get_raw_buffer_stub);

			// load script handles
			utils::hook::call(SELECT_VALUE(0x2BA152_b, 0x18C325_b), load_gametype_script_stub);

			utils::hook::call(SELECT_VALUE(0x3CC9F3_b, 0x513A53_b), vm_error_stub);

			// patch error to drop + give more information
			utils::hook::call(SELECT_VALUE(0x3BD626_b, 0x504606_b), unknown_function_stub); // CompileError
			utils::hook::call(SELECT_VALUE(0x3BD672_b, 0x504652_b), unknown_function_stub); // ^
			utils::hook::call(SELECT_VALUE(0x3BD75A_b, 0x50473A_b), find_variable_stub);
			scr_emit_function_hook.create(SELECT_VALUE(0x3BD680_b, 0x504660_b), scr_emit_function_stub);

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

			/*
				trying to do a jump hook to push the ent reference (if it exists) and the builtin function/methods works, but
				if longjmp is called because of a runtime error in our custom functions/methods, then the game just kinda dies
				or gets incredibly slow but will eventually load. for functions, the workaround is easy but for methods, we still
				have to remember the entity that called the method so the workaround is just hooking the Scr_GetEntityIdRef call
			*/
			utils::hook::nop(SELECT_VALUE(0x3CB723_b, 0x512783_b), 8);
			utils::hook::call(SELECT_VALUE(0x3CB723_b, 0x512783_b), vm_call_builtin_function_stub);

			utils::hook::call(SELECT_VALUE(0x3CBA12_b, 0x512A72_b), get_entity_id_stub);
			utils::hook::nop(SELECT_VALUE(0x3CBA46_b, 0x512AA6_b), 6);
			utils::hook::nop(SELECT_VALUE(0x3CBA4E_b, 0x512AAE_b), 2);
			utils::hook::call(SELECT_VALUE(0x3CBA46_b, 0x512AA6_b), vm_call_builtin_method_stub);

			function::add("print", [](const function_args& args)
			{
				std::string buffer{};

				for (auto i = 0u; i < args.size(); ++i)
				{
					const auto str = args[i].to_string();
					buffer.append(str);
					buffer.append("\t");
				}
				console::info("%s\n", buffer.data());

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

			function::add("replacefunc", [](const function_args& args)
			{
				const auto what = args[0].get_raw();
				const auto with = args[1].get_raw();

				if (what.type != game::SCRIPT_FUNCTION)
				{
					throw std::runtime_error("replaceFunc: parameter 1 must be a function");
				}

				if (with.type != game::SCRIPT_FUNCTION)
				{
					throw std::runtime_error("replaceFunc: parameter 2 must be a function");
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

			function::add("getfunction", [](const function_args& args) -> scripting::script_value
			{
				const auto filename = args[0].as<std::string>();
				const auto function = args[1].as<std::string>();

				if (scripting::script_function_table[filename].find(function) != scripting::script_function_table[filename].end())
				{
					return scripting::function{scripting::script_function_table[filename][function]};
				}

				return {};
			});

			scripting::on_shutdown([](int free_scripts)
			{
				if (free_scripts)
				{
					xsk::gsc::h1::resolver::cleanup();
					clear();
				}
			});
		}
	};
}

REGISTER_COMPONENT(gsc::component)
