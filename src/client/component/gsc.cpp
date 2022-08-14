#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "filesystem.hpp"
#include "scripting.hpp"

#include "game/game.hpp"
#include "game/scripting/execution.hpp"

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
		auto compiler = ::gsc::compiler();
		auto assembler = ::gsc::assembler();

		std::unordered_map<std::string, const char*> main_handles;
		std::unordered_map<std::string, const char*> init_handles;
		utils::memory::allocator script_allocator;

		bool override_is_default_script = false;
		
		char* allocate_buffer(void* data, size_t size)
		{
			const auto buffer = script_allocator.allocate_array<char>(size);
			std::memcpy(buffer, data, size);
			return buffer;
		}

		game::ScriptFile* load_custom_script(const char* file_name, const std::string& real_name)
		{
			std::string source_buffer{};
			if (!utils::io::read_file(real_name + ".gsc", &source_buffer))
			{
				override_is_default_script = false;
				return nullptr;
			}

			auto data = std::vector<uint8_t>{source_buffer.begin(), source_buffer.end()};

			try
			{
				compiler->compile(real_name, data);
			}
			catch (const std::exception& e)
			{
				console::error("Failed to compile '%s':\n%s", real_name.data(), e.what());
				override_is_default_script = false;
				return nullptr;
			}

			auto assembly = compiler->output();
			assembler->assemble(real_name, assembly);

			const auto script_file_ptr = script_allocator.allocate<game::ScriptFile>();
			script_file_ptr->name = file_name;

			const auto stack = assembler->output_stack();
			script_file_ptr->len = static_cast<int>(stack.size());

			const auto script = assembler->output_script();
			script_file_ptr->bytecodeLen = static_cast<int>(script.size());

			const auto compressed = xsk::utils::zlib::compress(stack);
			const auto buffer_size = script.size() + compressed.size() + 1;
			const auto script_size = script.size();

			const auto buffer = utils::memory::get_allocator()->allocate_array<char>(buffer_size);
			std::memcpy(buffer, script.data(), script_size);
			std::memcpy(&buffer[script_size], compressed.data(), compressed.size()); 

			script_file_ptr->bytecode = &buffer[0];
			script_file_ptr->buffer = &buffer[script.size()];
			script_file_ptr->compressedLen = static_cast<int>(compressed.size());

			return script_file_ptr;
		}

		game::ScriptFile* load_script(int, const char* name, int)
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

		void load_scripts(const std::string& script_dir)
		{
			if (!utils::io::directory_exists(script_dir))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir);
			for (const auto& script : scripts)
			{
				if (!script.ends_with(".gsc"))
				{
					continue;
				}

				const auto base_name = script.substr(0, script.size() - 4);

				override_is_default_script = true;
				if (!game::Scr_LoadScript(base_name.data()) ||
					scripting::script_function_table.find(base_name) == scripting::script_function_table.end())
				{
					continue;
				}

				const auto& script_table = scripting::script_function_table[base_name];
				if (script_table.find("init") != script_table.end())
				{
					main_handles[base_name] = script_table.at("init");
				}
				
				if (script_table.find("main") != script_table.end())
				{
					init_handles[base_name] = script_table.at("main");
				}
			}
		}

		void clear()
		{
			script_allocator.clear();
			main_handles.clear();
			init_handles.clear();
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
			const auto _0 = gsl::finally([]()
			{
				utils::hook::invoke<void>(0x18BC00_b);
			});

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

			clear();

			for (const auto& path : filesystem::get_search_paths())
			{
				load_scripts(path + "/scripts/");
				if (game::environment::is_sp())
				{
					load_scripts(path + "/scripts/sp/");
				}
				else
				{
					load_scripts(path + "/scripts/mp/");
				}
			}
		}

		void g_load_structs_stub()
		{
			const auto _0 = gsl::finally([]()
			{
				utils::hook::invoke<void>(0x458520_b);
			});

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

			for (auto& function_handle : main_handles)
			{
				console::info("Executing '%s::main'\n", function_handle.first.data());
				scripting::exec_ent_thread(*game::levelEntityId, function_handle.second, {});
			}

			main_handles.clear();
		}

		void save_registered_weapons_stub()
		{
			const auto _0 = gsl::finally([]()
			{
				utils::hook::invoke<void>(0x41DBC0_b);
			});

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

			for (auto& function_handle : init_handles)
			{
				console::info("Executing '%s::init'\n", function_handle.first.data());
				scripting::exec_ent_thread(*game::levelEntityId, function_handle.second, {});
			}

			init_handles.clear();
		}

		int db_is_xasset_default(int type, const char* name)
		{
			if (override_is_default_script)
			{
				override_is_default_script = false;
				return 0;
			}

			return utils::hook::invoke<int>(0x3968C0_b, type, name);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::call(0x50E357_b, load_script);
			utils::hook::call(0x50E367_b, db_is_xasset_default);

			utils::hook::call(0x18C325_b, load_gametype_script_stub);

			// execute handles
			utils::hook::call(0x420EA2_b, g_load_structs_stub);
			utils::hook::call(0x420F19_b, save_registered_weapons_stub);

			// replace builtin print function
			utils::hook::jump(0x437C40_b, gscr_print_stub);

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
