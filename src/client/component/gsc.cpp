#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "fastfiles.hpp"
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
#include <interface.hpp>

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

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

			const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_RAWFILE, name.data(), false);
			if (asset.rawfile)
			{
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

			const auto buffer_size = script.size() + 1;
			const auto script_size = script.size();

			const auto buffer = allocate_buffer(buffer_size);
			std::memcpy(buffer, script.data(), script_size);
			std::memcpy(&buffer[script_size], stack.data(), stack.size());

			script_file_ptr->bytecode = &buffer[0];
			script_file_ptr->buffer = &buffer[script.size()];
			script_file_ptr->compressedLen = 0;

			loaded_scripts[real_name] = script_file_ptr;

			return script_file_ptr;
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

				load_script(base_name);
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

			console::info("[SCRIPT] %s\n", buffer.data());
		}

		void load_gametype_script_stub()
		{
			utils::hook::invoke<void>(0x18BC00_b);

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

			clear();

			// TODO: look over this again? quak and I think that the game should handle loading the fully custom scripts that aren't overriding stock files and have the
			// mod creator execute the handles they want themselves. also, does the game load the file/handle right as it's called? -mikey
			
			/*
			fastfiles::enum_assets(game::ASSET_TYPE_RAWFILE, [](game::XAssetHeader header)
			{
				std::string name = header.scriptfile->name;

				if (name.ends_with(".gsc") && name.starts_with("scripts/"))
				{
					const auto base_name = name.substr(0, name.size() - 4);
					load_script(base_name);
				}
			}, true);
			*/

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
				game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
			}

			utils::hook::invoke<void>(0x458520_b);
		}

		void save_registered_weapons_stub()
		{
			for (auto& function_handle : init_handles)
			{
				console::info("Executing '%s::init'\n", function_handle.first.data());
				game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
			}

			utils::hook::invoke<void>(0x41DBC0_b);
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

			utils::hook::invoke<void>(0x396080_b, rawfile, buf, size);
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
			// maybe add the unknown function being referenced?
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
			xsk::gsc::h1::resolver::add_function(name, static_cast<std::uint16_t>(id));
			functions[id] = function;
		}

		void set_gsc_error(const std::string& error)
		{
			force_error_print = true;
			gsc_error = error;
			game::Scr_ErrorInternal();
		}

		void assert_cmd()
		{
			const auto expr = get_argument(0).as<int>();
			if (!expr)
			{
				set_gsc_error("assert fail");
			}
		}

		void assertex_cmd()
		{
			const auto expr = get_argument(0).as<int>();
			if (!expr)
			{
				set_gsc_error(get_argument(1).as<std::string>());
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

			// loads scripts with an uncompressed stack
			utils::hook::call(0x50E3C0_b, db_get_raw_buffer_stub);

			// load handles
			utils::hook::call(0x18C325_b, load_gametype_script_stub);

			// execute handles
			utils::hook::call(0x420EA2_b, g_load_structs_stub);
			utils::hook::call(0x420F19_b, save_registered_weapons_stub);

			// replace builtin print function (temporary?)
			utils::hook::jump(0x437C40_b, gscr_print_stub);

			// restore assert
			utils::hook::jump(0x439350_b, assert_cmd);
			utils::hook::jump(0x439430_b, assertex_cmd);

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
						set_gsc_error("replaceFunc: parameter 1 must be a function");
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
