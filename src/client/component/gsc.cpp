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
		//auto compiler = ::gsc::compiler();
		//auto assembler = ::gsc::assembler();

		//std::unordered_map<std::string, unsigned int> scr_main_handles;
		//std::unordered_map<std::string, unsigned int> scr_init_handles;

		game::ScriptFile* load_custom_script(const char* name)
		{
			utils::memory::allocator script_allocator;

			std::string script_dir = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty Modern Warfare Remastered\\h1-mod\\gsc";

			//auto id = static_cast<std::uint16_t>(std::atoi(name));
			if (!strcmp("maps/mp/gametypes/war", name)) // has no ID
			{
				console::debug("[OVERRIDE] Loading maps/mp/gametypes/war...");

				const auto data = utils::io::read_file(script_dir + "\\war.gscbin");
				const auto allocated_data = script_allocator.allocate(data.size());
				auto allocated_struct = script_allocator.allocate<game::ScriptFile>();
				std::memcpy(allocated_data, data.data(), data.size());

				const auto* name = reinterpret_cast<const char*>(allocated_data);
				auto len = strlen(name) + 1;

				allocated_struct->name = name;
				allocated_struct->compressedLen = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(allocated_data) + len);
				allocated_struct->len = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(allocated_data) + len + 4);
				allocated_struct->bytecodeLen = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(allocated_data) + len + 8);
				allocated_struct->buffer = reinterpret_cast<const char*>(reinterpret_cast<std::uintptr_t>(allocated_data) + len + 12);
				allocated_struct->bytecode = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(allocated_data) + len + 12 + allocated_struct->compressedLen);

				console::debug("------------------------\n");
				console::debug("name is %s\n", allocated_struct->name);
				console::debug("compressedLen is %d\n", allocated_struct->compressedLen);
				console::debug("len is %d\n", allocated_struct->len);
				console::debug("bytecodeLen is %d\n", allocated_struct->bytecodeLen);
				console::debug("buffer is %s\n", allocated_struct->buffer);
				console::debug("bytecode is %s\n", allocated_struct->bytecode);
				console::debug("------------------------\n");

				return allocated_struct;
			}

			return nullptr;
		}

		game::ScriptFile* load_script(game::XAssetType type, const char* name, int allow_create_default)
		{
			std::string real_name{name};
			auto id = static_cast<std::uint16_t>(std::atoi(name));
			if (id)
			{
				real_name = xsk::gsc::h1::resolver::token_name(id);
			}

			console::debug("Loading script %s (%s)\n", real_name.data(), name);

			auto* script = load_custom_script(name);
			if (script)
			{
				return script;
			}

			return game::DB_FindXAssetHeader(type, name, allow_create_default).scriptfile;
		}

	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// hook scriptfile loading
			utils::hook::call(0x50E357_b, load_script); // hooking DB_FindXAssetHeader for ASSET script
		}
	};
}

REGISTER_COMPONENT(gsc::component)
