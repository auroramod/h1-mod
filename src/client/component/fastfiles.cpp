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

		utils::hook::detour db_read_stream_file_hook;
		void db_read_stream_file_stub(int a1, int a2)
		{
			// always use lz4 compressor type when reading stream files
			*game::g_compressor = 4;
			return db_read_stream_file_hook.invoke<void>(a1, a2);
		}

		namespace mp
		{
			void skip_extra_zones_stub(utils::hook::assembler& a)
			{
				const auto skip = a.newLabel();
				const auto original = a.newLabel();

				a.pushad64();
				a.test(esi, game::DB_ZONE_CUSTOM); // allocFlags
				a.jnz(skip);

				a.bind(original);
				a.popad64();
				a.mov(rdx, 0x8E2F80_b);
				a.mov(rcx, rbp);
				a.call(0x840A20_b);
				a.jmp(0x398070_b);

				a.bind(skip);
				a.popad64();
				a.mov(r14d, game::DB_ZONE_CUSTOM);
				a.not_(r14d);
				a.and_(esi, r14d);
				a.jmp(0x39814F_b);
			}
		}
		namespace sp
		{
			void skip_extra_zones_stub(utils::hook::assembler& a)
			{
				const auto skip = a.newLabel();
				const auto original = a.newLabel();

				a.pushad64();
				a.test(ebp, game::DB_ZONE_CUSTOM); // allocFlags
				a.jnz(skip);

				a.bind(original);
				a.popad64();
				a.mov(r8d, 9);
				a.mov(rdx, 0x782210_b);
				a.jmp(0x1F4006_b);

				a.bind(skip);
				a.popad64();
				a.mov(r15d, game::DB_ZONE_CUSTOM);
				a.not_(r15d);
				a.and_(ebp, r15d);
				a.jmp(0x1F4023_b);
			}
		}

		bool try_load_zone(std::string name, bool localized, bool game = false)
		{
			if (localized)
			{
				const auto language = game::SEH_GetCurrentLanguageCode();
				if (!try_load_zone(language + "_"s + name, false) && language != "eng"s)
				{
					try_load_zone("eng_" + name, false);
				}
			}

			if (!fastfiles::exists(name))
			{
				console::debug("fastfile %s doesn't exist\n", name.data());
				return false;
			}

			game::XZoneInfo info{};
			info.name = name.data();
			info.allocFlags = (game ? game::DB_ZONE_GAME : game::DB_ZONE_COMMON) | game::DB_ZONE_CUSTOM;
			info.freeFlags = 0;
			game::DB_LoadXAssets(&info, 1u, game::DBSyncMode::DB_LOAD_ASYNC);
			return true;
		}

		utils::hook::detour sys_createfile_hook;
		HANDLE sys_create_file_stub(game::Sys_Folder folder, const char* base_filename)
		{
			const auto* fs_basepath = game::Dvar_FindVar("fs_basepath");
			const auto* fs_game = game::Dvar_FindVar("fs_game");

			std::string dir = fs_basepath ? fs_basepath->current.string : "";
			std::string mod_dir = fs_game ? fs_game->current.string : "";

			if (base_filename == "mod.ff"s)
			{
				if (!mod_dir.empty())
				{
					auto path = utils::string::va("%s\\%s\\%s", dir.data(), mod_dir.data(), base_filename);
					if (utils::io::file_exists(path))
					{
						return CreateFileA(path, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
					}
				}
				return (HANDLE)-1;
			}

			return sys_createfile_hook.invoke<HANDLE>(folder, base_filename);
		}

		template <typename T> inline void merge(std::vector<T>* target, T* source, size_t length)
		{
			if (source)
			{
				for (size_t i = 0; i < length; ++i)
				{
					target->push_back(source[i]);
				}
			}
		}

		template <typename T> inline void merge(std::vector<T>* target, std::vector<T> source)
		{
			for (auto& entry : source)
			{
				target->push_back(entry);
			}
		}

		void load_pre_gfx_zones(game::XZoneInfo* zoneInfo, unsigned int zoneCount, game::DBSyncMode syncMode)
		{
			std::vector<game::XZoneInfo> data;
			merge(&data, zoneInfo, zoneCount);

			// code_pre_gfx

			game::DB_LoadXAssets(data.data(), static_cast<std::uint32_t>(data.size()), syncMode);
		}

		void load_post_gfx_and_ui_and_common_zones(game::XZoneInfo* zoneInfo, unsigned int zoneCount, game::DBSyncMode syncMode)
		{
			std::vector<game::XZoneInfo> data;
			merge(&data, zoneInfo, zoneCount);

			// code_post_gfx
			// ui
			// common

			try_load_zone("h1_mod_common", true);

			game::DB_LoadXAssets(data.data(), static_cast<std::uint32_t>(data.size()), syncMode);

			try_load_zone("mod", true);
		}

		void load_ui_zones(game::XZoneInfo* zoneInfo, unsigned int zoneCount, game::DBSyncMode syncMode)
		{
			std::vector<game::XZoneInfo> data;
			merge(&data, zoneInfo, zoneCount);

			// ui

			game::DB_LoadXAssets(data.data(), static_cast<std::uint32_t>(data.size()), syncMode);
		}
	}

	bool exists(const std::string& zone)
	{
		const auto is_localized = game::DB_IsLocalized(zone.data());
		const auto handle = game::Sys_CreateFile((is_localized ? game::SF_ZONE_LOC : game::SF_ZONE), utils::string::va("%s.ff", zone.data()));
		if (handle != (HANDLE)-1)
		{
			CloseHandle(handle);
			return true;
		}

		return false;
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

			// Allow loading of unsigned fastfiles
			if (!game::environment::is_sp())
			{
				utils::hook::nop(0x368153_b, 2); // DB_InflateInit
			}

			// Allow loading of mixed compressor types
			utils::hook::nop(SELECT_VALUE(0x1C4BE7_b, 0x3687A7_b), 2);

			// Fix compressor type on streamed file load
			db_read_stream_file_hook.create(SELECT_VALUE(0x1FB9D0_b, 0x3A1BF0_b), db_read_stream_file_stub);

			// Add custom zone paths
			sys_createfile_hook.create(game::Sys_CreateFile, sys_create_file_stub);

			// load our custom pre_gfx zones
			utils::hook::call(SELECT_VALUE(0x3862ED_b, 0x15C3FD_b), load_pre_gfx_zones);
			utils::hook::call(SELECT_VALUE(0x3865E7_b, 0x15C75D_b), load_pre_gfx_zones);

			// load our custom ui and common zones
			utils::hook::call(SELECT_VALUE(0x5634AA_b, 0x686421_b), load_post_gfx_and_ui_and_common_zones);

			// load our custom ui zones
			utils::hook::call(SELECT_VALUE(0x3A5676_b, 0x17C6D2_b), load_ui_zones);

			// Don't load extra zones with loadzone
			if (game::environment::is_sp())
			{
				utils::hook::nop(0x1F3FF9_b, 13);
				utils::hook::jump(0x1F3FF9_b, utils::hook::assemble(sp::skip_extra_zones_stub), true);
			}
			else
			{
				utils::hook::nop(0x398061_b, 15);
				utils::hook::jump(0x398061_b, utils::hook::assemble(mp::skip_extra_zones_stub), true);
			}

			command::add("loadzone", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("usage: loadzone <zone>\n");
					return;
				}

				const auto name = params.get(1);
				if (!try_load_zone(name, false))
				{
					console::warn("loadzone: zone \"%s\" could not be found!\n", name);
				}
			});
		}
	};
}

REGISTER_COMPONENT(fastfiles::component)
