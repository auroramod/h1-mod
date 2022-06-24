#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/dvars.hpp"

#include "fastfiles.hpp"
#include "command.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace fastfiles
{
	static utils::concurrency::container<std::string> current_fastfile;

	namespace
	{
		utils::hook::detour db_try_load_x_file_internal_hook;
		utils::hook::detour db_find_xasset_header_hook;
		game::dvar_t* g_dump_scripts;

		void db_try_load_x_file_internal(const char* zone_name, const int flags)
		{
			console::info("Loading fastfile %s\n", zone_name);
			current_fastfile.access([&](std::string& fastfile)
			{
				fastfile = zone_name;
			});
			db_try_load_x_file_internal_hook.invoke<void>(zone_name, flags);
		}

		void dump_gsc_script(const std::string& name, game::XAssetHeader header)
		{
			if (!g_dump_scripts->current.enabled)
			{
				return;
			}

			std::string buffer;
			buffer.append(header.scriptfile->name, strlen(header.scriptfile->name) + 1);
			buffer.append(reinterpret_cast<char*>(&header.scriptfile->compressedLen), 4);
			buffer.append(reinterpret_cast<char*>(&header.scriptfile->len), 4);
			buffer.append(reinterpret_cast<char*>(&header.scriptfile->bytecodeLen), 4);
			buffer.append(header.scriptfile->buffer, header.scriptfile->compressedLen);
			buffer.append(header.scriptfile->bytecode, header.scriptfile->bytecodeLen);

			const auto out_name = utils::string::va("gsc_dump/%s.gscbin", name.data());
			utils::io::write_file(out_name, buffer);

			console::info("Dumped %s\n", out_name);
		}

		game::XAssetHeader db_find_xasset_header_stub(game::XAssetType type, const char* name, int allow_create_default)
		{
			const auto start = game::Sys_Milliseconds();
			const auto result = db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, name, allow_create_default);
			const auto diff = game::Sys_Milliseconds() - start;

			if (type == game::XAssetType::ASSET_TYPE_SCRIPTFILE)
			{
				dump_gsc_script(name, result);
			}

			if (diff > 100)
			{
				console::print(
					result.data == nullptr
						? console::con_type_error
						: console::con_type_warning,
					"Waited %i msec for %sasset \"%s\", of type \"%s\"\n",
					diff,
					result.data == nullptr
						? "missing "
						: "",
					name,
					game::g_assetNames[type]
				);
			}

			return result;
		}
	}

	std::string get_current_fastfile()
	{
		return current_fastfile.access<std::string>([&](std::string& fastfile)
		{
			return fastfile;
		});
	}

	void enum_assets(const game::XAssetType type, 
		const std::function<void(game::XAssetHeader)>& callback, const bool includeOverride)
	{
		game::DB_EnumXAssets_Internal(type, static_cast<void(*)(game::XAssetHeader, void*)>([](game::XAssetHeader header, void* data)
		{
			const auto& cb = *static_cast<const std::function<void(game::XAssetHeader)>*>(data);
			cb(header);
		}), &callback, includeOverride);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			db_try_load_x_file_internal_hook.create(
				SELECT_VALUE(0x1F5700_b, 0x39A620_b), &db_try_load_x_file_internal);

			db_find_xasset_header_hook.create(game::DB_FindXAssetHeader, db_find_xasset_header_stub);

			g_dump_scripts = dvars::register_bool("g_dumpScripts", false, game::DVAR_FLAG_NONE, "Dump GSC scripts");
		}
	};
}

REGISTER_COMPONENT(fastfiles::component)
