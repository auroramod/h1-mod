#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "gsc.hpp"
#include "filesystem.hpp"
#include "scripting.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

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
	namespace
	{
		game::dvar_t* developer_script = nullptr;

		std::unordered_map<int, std::string> opcodes =
		{
			{0x17, "SET_NEW_LOCAL_VARIABLE_FIELD_CACHED0"},
			{0x18, "EVAL_SELF_FIELD_VARIABLE"},
			{0x19, "RETN"},
			{0x1A, "CALL_BUILTIN_FUNC_0"},
			{0x1B, "CALL_BUILTIN_FUNC_1"},
			{0x1C, "CALL_BUILTIN_FUNC_2"},
			{0x1D, "CALL_BUILTIN_FUNC_3"},
			{0x1E, "CALL_BUILTIN_FUNC_4"},
			{0x1F, "CALL_BUILTIN_FUNC_5"},
			{0x20, "CALL_BUILTIN_FUNC"},
			{0x21, "BOOL_NOT"},
			{0x22, "CALL_FAR_METHOD_THEAD"},
			{0x23, "JMP_EXPR_TRUE"},
			{0x24, "SET_LEVEL_FIELD_VARIABLE_FIELD"},
			{0x25, "CAST_BOOL"},
			{0x26, "EVAL_NEW_LOCAL_ARRAY_REF_CACHED0"},
			{0x27, "CALL_BUILTIN_FUNC_POINTER"},
			{0x28, "INEQUALITY"},
			{0x29, "GET_THISTHREAD"},
			{0x2A, "CLEAR_FIELD_VARIABLE"},
			{0x2B, "GET_FLOAT"},
			{0x2C, "SAFE_CREATE_VARIABLE_FIELD_CACHED"},
			{0x2D, "CALL_FAR_FUNC2"},
			{0x2E, "CALL_FAR_FUNC"},
			{0x2F, "CALL_FAR_FUNC_CHILD_THREAD"},
			{0x30, "CLEAR_LOCAL_VARIABLE_FIELD_CACHED0"},
			{0x31, "CLEAR_LOCAL_VARIABLE_FIELD_CACHED"},
			{0x32, "CHECK_CLEAR_PARAMS"},
			{0x33, "CAST_FIELD_OBJ"},
			{0x34, "END"},
			{0x35, "SIZE"},
			{0x36, "EMPTY_ARRAY"},
			{0x37, "BIT_AND"},
			{0x38, "LESSEQUAL"},
			{0x39, "VOIDCODEPOS"},
			{0x3A, "CALL_METHOD_THREAD_POINTER"},
			{0x3B, "ENDSWITCH"},
			{0x3C, "CLEAR_VARIABLE_FIELD"},
			{0x3D, "DIV"},
			{0x3E, "CALL_FAR_METHOD_CHILD_THEAD"},
			{0x3F, "GET_USHORT"},
			{0x40, "JMP_TRUE"},
			{0x41, "GET_SELF"},
			{0x42, "CALL_FAR_FUNC_THREAD"},
			{0x43, "CALL_LOCAL_FUNC_THREAD"},
			{0x44, "SET_LOCAL_VARIABLE_FIELD_CACHED0"},
			{0x45, "SET_LOCAL_VARIABLE_FIELD_CACHED"},
			{0x46, "PLUS"},
			{0x47, "BOOL_COMPLEMENT"},
			{0x48, "CALL_METHOD_POINTER"},
			{0x49, "INC"},
			{0x4A, "REMOVE_LOCAL_VARIABLES"},
			{0x4B, "JMP_EXPR_FALSE"},
			{0x4C, "SWITCH"},
			{0x4D, "CLEAR_PARAMS"},
			{0x4E, "EVAL_LOCAL_VARIABLE_REF_CACHED0"},
			{0x4F, "EVAL_LOCAL_VARIABLE_REF_CACHED"},
			{0x50, "CALL_LOCAL_METHOD"},
			{0x51, "EVAL_FIELD_VARIABLE"},
			{0x52, "EVAL_FIELD_VARIABLE_REF"},
			{0x53, "GET_STRING"},
			{0x54, "CALL_FUNC_POINTER"},
			{0x55, "EVAL_LEVEL_FIELD_VARIABLE"},
			{0x56, "GET_VECTOR"},
			{0x57, "ENDON"},
			{0x58, "GREATEREQUAL"},
			{0x59, "GET_SELF_OBJ"},
			{0x5A, "SET_ANIM_FIELD_VARIABLE_FIELD"},
			{0x5B, "SET_VARIABLE_FIELD"},
			{0x5C, "CALL_LOCAL_FUNC2"},
			{0x5D, "CALL_LOCAL_FUNC"},
			{0x5E, "EVAL_LOCAL_ARRAY_REF_CACHED0"},
			{0x5F, "EVAL_LOCAL_ARRAY_REF_CACHED"},
			{0x60, "GET_FAR_FUNC"},
			{0x61, "LESS"},
			{0x62, "GET_GAME_REF"},
			{0x63, "WAITFRAME"},
			{0x64, "WAITTILLFRAMEEND"},
			{0x65, "SAFE_SET_VARIABLE_FIELD_CACHED0"},
			{0x66, "SAFE_SET_VARIABLE_FIELD_CACHED"},
			{0x67, "CALL_METHOD_CHILD_THREAD_POINTER"},
			{0x68, "GET_LEVEL"},
			{0x69, "NOTIFY"},
			{0x6A, "DEC_TOP"},
			{0x6B, "SHIFT_LEFT"},
			{0x6C, "CALL_LOCAL_METHOD_THREAD"},
			{0x6D, "CALL_LOCAL_METHOD_CHILD_THREAD"},
			{0x6E, "GREATER"},
			{0x6F, "EVAL_LOCAL_VARIABLE_CACHED0"},
			{0x70, "EVAL_LOCAL_VARIABLE_CACHED1"},
			{0x71, "EVAL_LOCAL_VARIABLE_CACHED2"},
			{0x72, "EVAL_LOCAL_VARIABLE_CACHED3"},
			{0x73, "EVAL_LOCAL_VARIABLE_CACHED4"},
			{0x74, "EVAL_LOCAL_VARIABLE_CACHED5"},
			{0x75, "EVAL_LOCAL_VARIABLE_CACHED"},
			{0x76, "SAFE_SET_WAITTILL_VARIABLE_FIELD_CACHED"},
			{0x77, "JMP"},
			{0x78, "CALL_FUNC_THREAD_POINTER"},
			{0x79, "GET_ZERO"},
			{0x7A, "WAIT"},
			{0x7B, "MINUS"},
			{0x7C, "SET_SELF_FIELD_VARIABLE_FIELD"},
			{0x7D, "EVAL_NEW_LOCAL_VARIABLE_REF_CACHED0"},
			{0x7E, "MULT"},
			{0x7F, "CREATE_LOCAL_VARIABLE"},
			{0x80, "CALL_LOCAL_FUNC_CHILD_THREAD"},
			{0x81, "GET_INT"},
			{0x82, "MOD"},
			{0x83, "EVAL_ANIM_FIELD_VARIABLE_REF"},
			{0x84, "GET_BUILTIN_FUNC"},
			{0x85, "GET_GAME"},
			{0x86, "WAITTILL"},
			{0x87, "DEC"},
			{0x88, "EVAL_LOCAL_VARIABLE_OBJECT_CACHED"},
			{0x89, "PRE_CALL"},
			{0x8A, "GET_ANIM"},
			{0x8B, "GET_UNDEFINED"},
			{0x8C, "EVAL_LEVEL_FIELD_VARIABLE_REF"},
			{0x8D, "GET_ANIM_OBJ"},
			{0x8E, "GET_LEVEL_OBJ"},
			{0x8F, "BIT_EXOR"},
			{0x90, "EQUALITY"},
			{0x91, "CLEAR_ARRAY"},
			{0x92, "JMP_BACK"},
			{0x93, "GET_ANIMATION"},
			{0x94, "EVAL_ANIM_FIELD_VARIABLE"},
			{0x95, "GET_ANIMTREE"},
			{0x96, "GET_ISTRING"},
			{0x97, "EVAL_ARRAY_REF"},
			{0x98, "EVAL_SELF_FIELD_VARIABLE_REF"},
			{0x99, "GET_NBYTE"},
			{0x9A, "GET_BUILTIN_METHOD"},
			{0x9B, "CALL_BUILTIN_METHOD_POINTER"},
			{0x9C, "EVAL_ARRAY"},
			{0x9D, "VECTOR"},
			{0x9E, "CALL_FAR_METHOD"},
			{0x9F, "EVAL_LOCAL_ARRAY_CACHED"},
			{0xA0, "GET_BYTE"},
			{0xA1, "CALL_FUNC_CHILD_THREAD_POINTER"},
			{0xA2, "BIT_OR"},
			{0xA3, "ADD_ARRAY"},
			{0xA4, "WAITTILLMATCH2"},
			{0xA5, "WAITTILLMATCH"},
			{0xA6, "GET_LOCAL_FUNC"},
			{0xA7, "GET_NUSHORT"},
			{0xA8, "SHIFT_RIGHT"},
			{0xA9, "CALL_BUILTIN_METHOD_0"},
			{0xAA, "CALL_BUILTIN_METHOD_1"},
			{0xAB, "CALL_BUILTIN_METHOD_2"},
			{0xAC, "CALL_BUILTIN_METHOD_3"},
			{0xAD, "CALL_BUILTIN_METHOD_4"},
			{0xAE, "CALL_BUILTIN_METHOD_5"},
			{0xAF, "CALL_BUILTIN_METHOD"},
			{0xB0, "JMP_FALSE"},
		};
		
		auto compiler = ::gsc::compiler();
		auto assembler = ::gsc::assembler();

		std::unordered_map<std::string, unsigned int> main_handles;
		std::unordered_map<std::string, unsigned int> init_handles;
		std::unordered_map<std::string, game::ScriptFile*> loaded_scripts;

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
			if (!developer_script->current.enabled)
			{
				return utils::hook::invoke<void*>(0x59DDA0_b, a1);
			}

			console::warn("*********** script runtime error *************\n");

			const auto opcode_id = *reinterpret_cast<std::uint8_t*>(0x7B8968_b);
			const auto opcode = get_opcode_name(opcode_id);
			if (opcode.has_value())
			{
				console::warn("while processing instruction %s\n", opcode.value().data());
			}
			else
			{
				console::warn("while processing instruction 0x%X\n", opcode_id);
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
