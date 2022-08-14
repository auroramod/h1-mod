#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "filesystem.hpp"

#include "game/game.hpp"

#include <xsk/gsc/types.hpp>
#include <xsk/gsc/interfaces/compiler.hpp>
#include <xsk/gsc/interfaces/assembler.hpp>
#include <xsk/resolver.hpp>
#include <interface.hpp>

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/compression.hpp>

namespace gsc
{
	namespace
	{
		auto compiler = ::gsc::compiler();
		auto assembler = ::gsc::assembler();

		std::unordered_map<std::string, unsigned int> scr_main_handles;
		std::unordered_map<std::string, unsigned int> scr_init_handles;

		game::ScriptFile script_file_;

		game::ScriptFile* load_custom_script(const char* file_name, const std::string& real_name)
		{
			memset(&script_file_, 0, sizeof(script_file_)); // we need to memset this so we can reuse i again

			filesystem::file source{real_name + ".gsc"};
			if (!source.exists())
			{
				return nullptr;
			}

			console::debug("found '%s.gsc'\n", real_name.data());

			const auto& source_buffer = source.get_buffer();
			auto data = std::vector<uint8_t>{source_buffer.begin(), source_buffer.end()};

			try
			{
				compiler->compile(real_name, data);
			}
			catch (std::exception& e)
			{
				game::Com_Error(game::ERR_DROP, "Failed to compile '%s'.\n%s", real_name.data(), e.what());
				return nullptr;
			}

			auto assembly = compiler->output();

			assembler->assemble(real_name, assembly);

			auto* script_file_ptr = &script_file_;
			script_file_ptr->name = file_name;

			auto stack = assembler->output_stack(); // this is the uncompressed stack
			script_file_ptr->len = static_cast<int>(stack.size());

			auto script = assembler->output_script(); // this is the assembly bytecode
			script_file_ptr->bytecodeLen = static_cast<int>(script.size());

			// we compress the assembler's stack because that's the only way ProcessScript knows how to read it
			auto compressed = utils::compression::zlib::gsc_compress(stack); // this util comes from gsc-tool, could just change the dependency to add it lmfao

			// the size of the whole buffer for the bytecode assembly + compressed buffer
			size_t buffer_size = script.size() + compressed.size() + 1;

			auto* buffer = game::PMem_AllocFromSource_NoDebug(buffer_size, 4, 0, game::PMEM_SOURCE_SCRIPT);
			memset(buffer, 0, buffer_size);
			memcpy(buffer, script.data(), script.size()); // the first part of the buffer will be the bytecode assembly
			memcpy(&buffer[script.size()], compressed.data(), compressed.size()); // then, at the end of the bytecode assembly, the compressed stack buffer starts

			script_file_ptr->bytecode = &buffer[0];
			script_file_ptr->buffer = &buffer[script.size()];
			script_file_ptr->compressedLen = static_cast<int>(compressed.size());

			return script_file_ptr;
		}

		uint16_t resolve_token(const std::string& token)
		{
			return xsk::gsc::h1::resolver::token_id(token);
		}

		game::ScriptFile* load_script(int, const char* name, int)
		{
			std::string real_name{name};
			auto id = static_cast<std::uint16_t>(std::atoi(name));
			if (id)
			{
				real_name = xsk::gsc::h1::resolver::token_name(id);
			}

			auto* script = load_custom_script(name, real_name);
			if (script)
			{
				console::debug("--------script data--------\n");
				console::debug("name is '%s'\n", script->name);
				console::debug("compressedLen is %d\n", script->compressedLen);
				console::debug("len is %d\n", script->len);
				console::debug("bytecodeLen is %d\n", script->bytecodeLen);
				//console::debug("buffer is %s\n", script->buffer);
				//console::debug("bytecode is %s\n", script->bytecode);
				console::debug("---------------------------\n");

				return script;
			}

			return game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, name, 1).scriptfile;
		}

		void load_gametype_script_stub()
		{
			console::debug(__FUNCTION__ "\n");

			const auto in_vl = game::VirtualLobby_Loaded();
			console::debug("in vl? %d", in_vl);
			if (in_vl)
			{
				utils::hook::invoke<void>(0x18BC00_b);
				return;
			}

			// TODO: clear handles
			scr_main_handles.clear();
			scr_init_handles.clear();

			char path[MAX_PATH]{};
			sprintf_s(path, "%s/%s", "gsc", "ye"); // TODO: change this to not be hardcoded

			auto script_loaded = game::Scr_LoadScript(path);
			if (script_loaded)
			{
				console::debug("load script was successful, path is '%s'\n", path);
				console::debug("load script returned '%x'\n", script_loaded);

				auto token = resolve_token("main");
				auto main_handle = game::Scr_GetFunctionHandle(path, token);
				console::debug("main_handle is %x\n", main_handle);
				if (main_handle)
				{
					console::debug("Found valid function handle '%s::main'.\n", path);
					scr_main_handles.insert_or_assign(path, main_handle);
				}

				token = resolve_token("init");
				auto init_handle = game::Scr_GetFunctionHandle(path, token);
				console::debug("init_handle is %x\n", init_handle);
				if (init_handle)
				{
					console::debug("Found valid function handle '%s::init'.\n", path);
					scr_init_handles.insert_or_assign(path, init_handle);
				}
			}

			utils::hook::invoke<void>(0x18BC00_b);
		}

		void g_load_structs_stub()
		{
			console::debug(__FUNCTION__ "\n");

			const auto in_vl = game::VirtualLobby_Loaded();
			console::debug("in vl? %d", in_vl);
			if (in_vl)
			{
				utils::hook::invoke<void>(0x458520_b);
				return;
			}

			for (auto& function_handle : scr_main_handles)
			{
				const auto* name = function_handle.first.data();

				console::debug("[" __FUNCTION__ "] Loading main handle '%x::main'...\n", name);
				unsigned int thread = game::Scr_ExecThread(function_handle.second, 0);
				console::debug("[" __FUNCTION__ "] Thread ID is %d\n", thread);
				game::RemoveRefToObject(thread);
				console::debug("[" __FUNCTION__ "] Executed handle '%x::main'.\n", name);
			}

			utils::hook::invoke<void>(0x458520_b);
		}

		void idk_lol_stub()
		{
			console::debug(__FUNCTION__ "\n");

			const auto in_vl = game::VirtualLobby_Loaded();
			console::debug("in vl? %d", in_vl);
			if (in_vl)
			{
				utils::hook::invoke<void>(0x41DBC0_b);
				return;
			}

			for (auto& function_handle : scr_init_handles)
			{
				const auto* name = function_handle.first.data();

				console::debug("[" __FUNCTION__ "] Loading main handle '%x::init'...\n", name);
				auto thread = game::Scr_ExecThread(function_handle.second, 0);
				console::debug("[" __FUNCTION__ "] Thread ID is %d\n", thread);
				game::RemoveRefToObject(thread);
				console::debug("[" __FUNCTION__ "] Executed handle '%x::init'.\n", name);
			}

			utils::hook::invoke<void>(0x41DBC0_b);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// hook scriptfile loading
			utils::hook::call(0x50E357_b, load_script); // hooking DB_FindXAssetHeader for ASSET script

			// load custom scripts
			utils::hook::call(0x18C325_b, load_gametype_script_stub);

			// execute handles
			utils::hook::call(0x420EA2_b, g_load_structs_stub); // execute main handles (Scr_LoadGameType is inlined)
			utils::hook::call(0x420F19_b, idk_lol_stub); // execute init handles (Scr_StartupGameType is inlined)

			//utils::hook::jump(0x479270, script_error);
		}
	};
}

REGISTER_COMPONENT(gsc::component)
