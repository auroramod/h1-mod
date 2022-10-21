#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "component/console.hpp"
#include "component/fastfiles.hpp"
#include "component/filesystem.hpp"
#include "component/logfile.hpp"
#include "component/scripting.hpp"

#include "game/dvars.hpp"

#include "game/scripting/array.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/function.hpp"

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
	namespace
	{
		auto compiler = ::gsc::compiler();
		auto decompiler = ::gsc::decompiler();
		auto assembler = ::gsc::assembler();
		auto disassembler = ::gsc::disassembler();

		std::unordered_map<std::string, std::uint32_t> main_handles;
		std::unordered_map<std::string, std::uint32_t> init_handles;

		std::unordered_map<std::string, game::ScriptFile*> loaded_scripts;

		void clear()
		{
			main_handles.clear();
			init_handles.clear();
			loaded_scripts.clear();
		}

		bool read_script_file(const std::string& name, std::string* data)
		{
			if (filesystem::read_file(name, data))
			{
				return true;
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

		char* allocate_buffer(size_t size)
		{
			// PMem_AllocFromSource_NoDebug
			return utils::hook::invoke<char*>(SELECT_VALUE(0x41FB50_b, 0x5A4DC0_b), size, 4, 1, 5);
		}

		game::ScriptFile* load_custom_script(const char* file_name, const std::string& real_name)
		{
			if (const auto itr = loaded_scripts.find(real_name); itr != loaded_scripts.end())
			{
				return itr->second;
			}

			/*
				without this check, gsc rawfiles that a map contains will be compiled. however, these aren't the correct files.
				each rawfile has a scriptfile counterpart in asset pool that is meant to be used instead.
				the gsc rawfiles are just leftover from creating the maps.

				(if you are creating a custom map, you can safely have gsc rawfiles without having scriptfile counterparts)
			*/
			if (real_name.starts_with("maps/createfx") || real_name.starts_with("maps/createart")
				|| (real_name.starts_with("maps/mp") && real_name.ends_with("_fx.gsc")))
			{
				if (game::DB_XAssetExists(game::ASSET_TYPE_SCRIPTFILE, real_name.data()))
				{
					return game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, real_name.data(), false).scriptfile;
				}
			}

			std::string source_buffer{};
			if (!read_script_file(real_name + ".gsc", &source_buffer))
			{
				return nullptr;
			}

			std::vector<std::uint8_t> data;
			data.assign(source_buffer.begin(), source_buffer.end());

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
			script_file_ptr->buffer = reinterpret_cast<char*>(&buffer[script.size()]);
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

			const auto main_handle = game::Scr_GetFunctionHandle(name.data(), xsk::gsc::h1::resolver::token_id("main"));
			const auto init_handle = game::Scr_GetFunctionHandle(name.data(), xsk::gsc::h1::resolver::token_id("init"));

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

		int db_is_x_asset_default(game::XAssetType type, const char* name)
		{
			if (loaded_scripts.contains(name))
			{
				return 0;
			}

			return game::DB_IsXAssetDefault(type, name);
		}

		void gscr_load_gametype_script_stub(void* a1, void* a2)
		{
			utils::hook::invoke<void>(SELECT_VALUE(0x2B9DA0_b, 0x18BC00_b), a1, a2);

			clear();

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

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

	game::ScriptFile* find_script(game::XAssetType type, const char* name, int allow_create_default)
	{
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

		return game::DB_FindXAssetHeader(type, name, allow_create_default).scriptfile;
	}

	class loading final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			// allow custom scripts to include other custom scripts
			xsk::gsc::h1::resolver::init([](const auto& include_name)
			{
				const auto real_name = include_name + ".gsc";

				std::string file_buffer;
				if (!read_script_file(real_name, &file_buffer) || file_buffer.empty())
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
			utils::hook::call(SELECT_VALUE(0x3C7227_b, 0x50E367_b), db_is_x_asset_default);

			// GScr_LoadScripts
			utils::hook::call(SELECT_VALUE(0x2BA152_b, 0x18C325_b), gscr_load_gametype_script_stub);

			// loads scripts with an uncompressed stack
			utils::hook::call(SELECT_VALUE(0x3C7280_b, 0x50E3C0_b), db_get_raw_buffer_stub);

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

REGISTER_COMPONENT(gsc::loading)