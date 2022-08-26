#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "gsc.hpp"
#include "filesystem.hpp"
#include "logfile.hpp"
#include "scripting.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"
#include "game/scripting/functions.hpp"

#include <xsk/gsc/types.hpp>
#include <xsk/gsc/interfaces/compiler.hpp>
#include <xsk/gsc/interfaces/assembler.hpp>
#include <xsk/resolver.hpp>
#include <xsk/utils/compression.hpp>
#include <interface.hpp>

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/compression.hpp>

namespace gsc
{
	void* func_table[0x1000]{};

	namespace
	{
		game::dvar_t* developer_script = nullptr;

		auto compiler = ::gsc::compiler();
		auto assembler = ::gsc::assembler();

		std::unordered_map<std::string, unsigned int> main_handles;
		std::unordered_map<std::string, unsigned int> init_handles;
		std::unordered_map<std::string, game::ScriptFile*> loaded_scripts;
		std::unordered_map<unsigned int, scripting::script_function> functions;
		std::optional<std::string> gsc_error;

		bool force_error_print = false;

		char* allocate_buffer(size_t size)
		{
			return utils::hook::invoke<char*>(0x5A4DC0_b, size, 4, 1, 5);
		}

		game::ScriptFile* load_custom_script(const char* file_name, const std::string& real_name)
		{
			if (loaded_scripts.find(real_name) != loaded_scripts.end())
			{
				return loaded_scripts[real_name];
			}

			std::string source_buffer{};
			if (!filesystem::read_file(real_name + ".gsc", &source_buffer))
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

			const auto compressed = xsk::utils::zlib::compress(stack);
			const auto buffer_size = script.size() + compressed.size() + 1;
			const auto script_size = script.size();

			const auto buffer = allocate_buffer(buffer_size);
			std::memcpy(buffer, script.data(), script_size);
			std::memcpy(&buffer[script_size], compressed.data(), compressed.size()); 

			script_file_ptr->bytecode = &buffer[0];
			script_file_ptr->buffer = &buffer[script.size()];
			script_file_ptr->compressedLen = static_cast<int>(compressed.size());

			loaded_scripts[real_name] = script_file_ptr;

			return script_file_ptr;
		}

		void load_scripts(const std::filesystem::path& root_dir)
		{
			std::filesystem::path script_dir = root_dir / "scripts";
			if (!utils::io::directory_exists(script_dir.generic_string()))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir.generic_string());
			for (const auto& script : scripts)
			{
				if (!script.ends_with(".gsc"))
				{
					continue;
				}

				std::filesystem::path path(script);
				const auto relative = path.lexically_relative(root_dir).generic_string();
				const auto base_name = relative.substr(0, relative.size() - 4);

				if (!game::Scr_LoadScript(base_name.data()))
				{
					continue;
				}

				const auto main_handle = game::Scr_GetFunctionHandle(base_name.data(), 
					xsk::gsc::h1::resolver::token_id("main"));
				const auto init_handle = game::Scr_GetFunctionHandle(base_name.data(), 
					xsk::gsc::h1::resolver::token_id("init"));

				if (main_handle)
				{
					console::info("Loaded '%s::main'\n", base_name.data());
					main_handles[base_name] = main_handle;
				}
				else if (init_handle)
				{
					console::info("Loaded '%s::init'\n", base_name.data());
					init_handles[base_name] = init_handle;
				}
			}
		}

		void clear()
		{
			main_handles.clear();
			init_handles.clear();
			loaded_scripts.clear();
		}

		void gscr_print_stub()
		{
			const auto num = game::Scr_GetNumParam();
			std::string buffer{};

			for (auto i = 0; i < num; i++)
			{
				const auto str = game::Scr_GetString(i);
				buffer.append(str);
				buffer.append("\t");
			}

			printf("%s\n", buffer.data());
		}

		void load_gametype_script_stub()
		{
			utils::hook::invoke<void>(0x18BC00_b);

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

			clear();

			for (const auto& path : filesystem::get_search_paths())
			{
				load_scripts(path);
			}
		}

		void g_load_structs_stub()
		{
			for (auto& function_handle : main_handles)
			{
				console::info("Executing '%s::main'\n", function_handle.first.data());
				const auto thread = game::Scr_ExecThread(function_handle.second, 0);
				game::RemoveRefToObject(thread);
			}

			utils::hook::invoke<void>(0x458520_b);
		}

		// fed has his as scr_loadlevel, but this shouldn't be a problem
		void save_registered_weapons_stub()
		{
			for (auto& function_handle : init_handles)
			{
				console::info("Executing '%s::init'\n", function_handle.first.data());
				const auto thread = game::Scr_ExecThread(function_handle.second, 0);
				game::RemoveRefToObject(thread);
			}

			utils::hook::invoke<void>(0x41DBC0_b);
		}

		int db_is_xasset_default(int type, const char* name)
		{
			if (loaded_scripts.find(name) != loaded_scripts.end())
			{
				return 0;
			}

			return utils::hook::invoke<int>(0x3968C0_b, type, name);
		}

		std::optional<std::pair<std::string, std::string>> find_function(const char* pos)
		{
			for (const auto& file : scripting::script_function_table)
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
				const auto function = find_function(frame->fs.pos);
				if (function.has_value())
				{
					console::warn("\tat function \"%s\" in file \"%s.gsc\"", 
						function.value().first.data(), function.value().second.data(), frame->fs.pos);
				}
				else
				{
					console::warn("\tat unknown location", frame->fs.pos);
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

		void* vm_error_stub(void* a1)
		{
			if (!developer_script->current.enabled || force_error_print)
			{
				return utils::hook::invoke<void*>(0x59DDA0_b, a1);
			}

			force_error_print = false;

			console::warn("*********** script runtime error *************\n");

			const auto opcode_id = *reinterpret_cast<std::uint8_t*>(0x7B8968_b);
			const auto opcode = get_opcode_name(opcode_id);
			const std::string error_str = gsc_error.has_value()
				? utils::string::va(": %s", gsc_error.value().data())
				: "";
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

			print_callstack();
			console::warn("**********************************************\n");
			return utils::hook::invoke<void*>(0x59DDA0_b, a1);
		}

		void unknown_function_stub()
		{
			// TODO: maybe add the unknown function being referenced?
			game::Com_Error(game::ERR_DROP, "LinkFile: unknown function in script '%s.gsc'", 
				scripting::current_file.data());
		}

		void register_gsc_functions_stub(void* a1, void* a2)
		{
			utils::hook::invoke<void>(0x1CE120_b, a1, a2);
			for (const auto& func : functions)
			{
				game::Scr_RegisterFunction(func.second, 0, func.first);
			}
		}

		scripting::script_value get_argument(int index)
		{
			if (index >= static_cast<int>(game::scr_VmPub->outparamcount))
			{
				return {};
			}

			const auto value = &game::scr_VmPub->top[-index];

			return scripting::script_value(*value);
		}

		auto function_id_start = 0x30A;
		void add_function(const std::string& name, scripting::script_function function)
		{
			const auto id = ++function_id_start;
			scripting::function_map[name] = id; // TODO: change to gsc-tool's function map add function
			functions[id] = function;
		}

		void set_gsc_error(const std::string& error)
		{
			force_error_print = true;
			gsc_error = error;
			game::Scr_ErrorInternal();
		}
	}

	game::ScriptFile* find_script(game::XAssetType /*type*/, const char* name, int /*allow_create_default*/)
	{
		std::string real_name = name;
		const auto id = static_cast<std::uint16_t>(std::atoi(name));
		if (id)
		{
			real_name = xsk::gsc::h1::resolver::token_name(id);
		}

		const auto script = load_custom_script(name, real_name);
		if (script)
		{
			return script;
		}

		return game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, name, 1).scriptfile;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			developer_script = dvars::register_bool("developer_script", false, 0, "Print GSC errors");

			// hook xasset functions to return our own custom scripts
			utils::hook::call(0x50E357_b, find_script);
			utils::hook::call(0x50E367_b, db_is_xasset_default);

			// load handles
			utils::hook::call(0x18C325_b, load_gametype_script_stub);

			// execute handles
			utils::hook::call(0x420EA2_b, g_load_structs_stub);
			utils::hook::call(0x420F19_b, save_registered_weapons_stub);

			// replace builtin print function (temporary?)
			utils::hook::jump(0x437C40_b, gscr_print_stub);

			utils::hook::call(0x513A53_b, vm_error_stub);

			// patch error to drop + give more information
			utils::hook::call(0x504606_b, unknown_function_stub); // CompileError
			utils::hook::call(0x504652_b, unknown_function_stub); // ^

			utils::hook::call(0x504C8B_b, register_gsc_functions_stub);
			utils::hook::set<uint32_t>(0x50484C_b, 0x1000); // change builtin func count

#define RVA(ptr) static_cast<uint32_t>(reinterpret_cast<size_t>(ptr) - 0x140000000)
			std::memcpy(&func_table, reinterpret_cast<void*>(0xAC83820_b), sizeof(reinterpret_cast<void*>(0xAC83820_b)));
			utils::hook::set<uint32_t>(0x504852_b + 4, RVA(&func_table));
			utils::hook::inject(0x504C58_b + 3, &func_table);
			utils::hook::set<uint32_t>(0x512778_b + 4, RVA(&func_table));

			add_function("replacefunc", [](const game::scr_entref_t ref)
			{
				try
				{
					const auto what = get_argument(0).get_raw();
					const auto with = get_argument(1).get_raw();

					if (what.type != game::SCRIPT_FUNCTION)
					{
						set_gsc_error("replaceFunc: parameter 0 must be a function");
						return;
					}

					if (with.type != game::SCRIPT_FUNCTION)
					{
						set_gsc_error("replaceFunc: parameter 0 must be a function");
						return;
					}

					logfile::set_gsc_hook(what.u.codePosValue, with.u.codePosValue);
				}
				catch (const std::exception& e)
				{
					set_gsc_error(utils::string::va("replaceFunc: %s", e.what()));
				}
			});

			scripting::on_shutdown([](int free_scripts)
			{
				if (free_scripts)
				{
					clear();
				}
			});
		}
	};
}

REGISTER_COMPONENT(gsc::component)
