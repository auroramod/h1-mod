#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "fastfiles.hpp"
#include "filesystem.hpp"

#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace fastfiles
{
	static utils::concurrency::container<std::string> current_fastfile;
	static utils::concurrency::container<std::optional<std::string>> current_usermap;

	namespace
	{
		utils::hook::detour db_try_load_x_file_internal_hook;
		utils::hook::detour db_find_xasset_header_hook;
		game::dvar_t* g_dump_scripts;

		std::vector<HANDLE> fastfile_handles;

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
			auto result = db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, name, allow_create_default);
			const auto diff = game::Sys_Milliseconds() - start;

			if (type == game::XAssetType::ASSET_TYPE_SCRIPTFILE)
			{
				dump_gsc_script(name, result);
			}

			if (type == game::XAssetType::ASSET_TYPE_RAWFILE)
			{
				if (result.rawfile)
				{
					const std::string override_rawfile_name = "override/"s + name;
					const auto override_rawfile = db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, override_rawfile_name.data(), 0);
					if (override_rawfile.rawfile)
					{
						result.rawfile = override_rawfile.rawfile;
						console::debug("using override asset for rawfile: \"%s\"\n", name);
					}
				}
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
				try_load_zone(language + "_"s + name, false);
				if (game::environment::is_mp())
				{
					try_load_zone(language + "_"s + name + "_mp"s, false);
				}
			}

			if (!fastfiles::exists(name))
			{
				return false;
			}

			game::XZoneInfo info{};
			info.name = name.data();
			info.allocFlags = (game ? game::DB_ZONE_GAME : game::DB_ZONE_COMMON) | game::DB_ZONE_CUSTOM;
			info.freeFlags = 0;
			game::DB_LoadXAssets(&info, 1u, game::DBSyncMode::DB_LOAD_ASYNC);
			return true;
		}

		HANDLE find_fastfile(const std::string& filename, bool check_loc_folder)
		{
			std::string path{};
			std::string loc_folder{};

			if (check_loc_folder && game::DB_IsLocalized(filename.data()))
			{
				const auto handle = find_fastfile(filename, false);
				if (handle != INVALID_HANDLE_VALUE)
				{
					return handle;
				}

				loc_folder = game::SEH_GetCurrentLanguageName() + "/"s;
			}

			if (!filesystem::find_file(loc_folder + filename, &path))
			{
				if (!filesystem::find_file("zone/"s + loc_folder + filename, &path))
				{
					return INVALID_HANDLE_VALUE;
				}
			}

			const auto handle = CreateFileA(path.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, nullptr);
			if (handle != INVALID_HANDLE_VALUE)
			{
				fastfile_handles.push_back(handle);
			}

			return handle;
		}

		HANDLE find_usermap(const std::string& mapname)
		{
			const auto usermap = fastfiles::get_current_usermap();
			if (!usermap.has_value())
			{
				return INVALID_HANDLE_VALUE;
			}

			const auto& usermap_value = usermap.value();
			const std::string usermap_file = utils::string::va("%s.ff", usermap_value.data());
			const std::string usermap_load_file = utils::string::va("%s_load.ff", usermap_value.data());

			if (mapname == usermap_file || mapname == usermap_load_file)
			{
				const auto path = utils::string::va("usermaps\\%s\\%s",
					usermap_value.data(), mapname.data());
				if (utils::io::file_exists(path))
				{
					return CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, nullptr);
				}
			}

			return INVALID_HANDLE_VALUE;
		}

		utils::hook::detour sys_createfile_hook;
		HANDLE sys_create_file(game::Sys_Folder folder, const char* base_filename, bool ignore_usermap)
		{
			const auto* fs_basepath = game::Dvar_FindVar("fs_basepath");
			const auto* fs_game = game::Dvar_FindVar("fs_game");

			const std::string dir = fs_basepath ? fs_basepath->current.string : "";
			const std::string mod_dir = fs_game ? fs_game->current.string : "";
			const std::string name = base_filename;

			if (name == "mod.ff")
			{
				if (!mod_dir.empty())
				{
					const auto path = utils::string::va("%s\\%s\\%s",
						dir.data(), mod_dir.data(), base_filename);

					if (utils::io::file_exists(path))
					{
						return CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
							FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, nullptr);
					}
				}

				return INVALID_HANDLE_VALUE;
			}

			auto handle = sys_createfile_hook.invoke<HANDLE>(folder, base_filename);
			if (handle != INVALID_HANDLE_VALUE)
			{
				return handle;
			}

			if (!ignore_usermap)
			{
				const auto usermap = find_usermap(name);
				if (usermap != INVALID_HANDLE_VALUE)
				{
					return usermap;
				}
			}

			if (name.ends_with(".ff"))
			{
				handle = find_fastfile(name, true);
			}

			return handle;
		}

		HANDLE sys_create_file_stub(game::Sys_Folder folder, const char* base_filename)
		{
			return sys_create_file(folder, base_filename, false);
		}

		utils::hook::detour db_file_exists_hook;
		bool db_file_exists_stub(const char* file, int a2)
		{
			const auto file_exists = db_file_exists_hook.invoke<bool>(file, a2);
			if (file_exists)
			{
				return file_exists;
			}

			return fastfiles::usermap_exists(file);
		}

		template <typename T>
		inline void merge(std::vector<T>* target, T* source, size_t length)
		{
			if (source)
			{
				for (size_t i = 0; i < length; ++i)
				{
					target->push_back(source[i]);
				}
			}
		}

		template <typename T>
		inline void merge(std::vector<T>* target, std::vector<T> source)
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

		void load_lua_file_asset_stub(void* a1)
		{
			const auto fastfile = fastfiles::get_current_fastfile();
			if (fastfile == "mod")
			{
				console::error("Mod tried to load a lua file!\n");
				return;
			}

			const auto usermap = fastfiles::get_current_usermap();
			if (usermap.has_value())
			{
				const auto& usermap_value = usermap.value();
				const auto usermap_load = usermap_value + "_load";

				if (fastfile == usermap_value || fastfile == usermap_load)
				{
					console::error("Usermap tried to load a lua file!\n");
					return;
				}
			}

			utils::hook::invoke<void>(0x39CA90_b, a1);
		}

		void db_level_load_add_zone_stub(void* load, const char* name, const unsigned int alloc_flags,
			const size_t size_est)
		{
			auto is_builtin_map = false;
			for (auto map = &game::maps[0]; map->unk; ++map)
			{
				if (!std::strcmp(map->name, name))
				{
					is_builtin_map = true;
					break;
				}
			}

			if (is_builtin_map)
			{
				game::DB_LevelLoadAddZone(load, name, alloc_flags, size_est);
			}
			else
			{
				game::DB_LevelLoadAddZone(load, name, alloc_flags | game::DB_ZONE_CUSTOM, size_est);
			}
		}

		void db_find_aipaths_stub(game::XAssetType type, const char* name, int allow_create_default)
		{
			if (game::DB_XAssetExists(type, name))
			{
				game::DB_FindXAssetHeader(type, name, allow_create_default);
			}
			else
			{
				console::warn("No aipaths found for this map\n");
			}
		}

		int format_bsp_name(char* filename, int size, const char* mapname)
		{
			std::string name = mapname;
			auto fmt = "maps/%s.d3dbsp";
			if (name.starts_with("mp_"))
			{
				fmt = "maps/mp/%s.d3dbsp";
			}

			return game::Com_sprintf(filename, size, fmt, mapname);
		}

		void get_bsp_filename_stub(char* filename, int size, const char* mapname)
		{
			auto base_mapname = mapname;
			game::Com_IsAddonMap(mapname, &base_mapname);
			format_bsp_name(filename, size, base_mapname);
		}

		utils::hook::detour image_file_decrypt_value_hook;
		bool image_file_decrypt_value_stub(char* value, int size, char* buffer)
		{
			auto is_all_zero = true;
			for (auto i = 0; i < size; i++)
			{
				if (value[i] != 0)
				{
					is_all_zero = false;
				}
			}

			if (is_all_zero)
			{
				return true;
			}

			return image_file_decrypt_value_hook.invoke<bool>(value, size, buffer);
		}

		int com_sprintf_stub(char* dest, int size, const char* /*fmt*/, const char* mapname)
		{
			return format_bsp_name(dest, size, mapname);
		}
	}

	bool exists(const std::string& zone, bool ignore_usermap)
	{
		const auto is_localized = game::DB_IsLocalized(zone.data());
		const auto handle = sys_create_file((is_localized ? game::SF_ZONE_LOC : game::SF_ZONE),
			utils::string::va("%s.ff", zone.data()), ignore_usermap);

		if (handle != INVALID_HANDLE_VALUE)
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

	void close_fastfile_handles()
	{
		for (const auto& handle : fastfile_handles)
		{
			CloseHandle(handle);
		}
	}

	void set_usermap(const std::string& usermap)
	{
		current_usermap.access([&](std::optional<std::string>& current_usermap_)
		{
			current_usermap_ = usermap;
		});
	}

	void clear_usermap()
	{
		current_usermap.access([&](std::optional<std::string>& current_usermap_)
		{
			current_usermap_.reset();
		});
	}

	std::optional<std::string> get_current_usermap()
	{
		return current_usermap.access<std::optional<std::string>>([&](
			std::optional<std::string>& current_usermap_)
		{
			return current_usermap_;
		});
	}

	bool usermap_exists(const std::string& name)
	{
		if (is_stock_map(name))
		{
			return false;
		}

		return utils::io::file_exists(utils::string::va("usermaps\\%s\\%s.ff", name.data(), name.data()));
	}

	bool is_stock_map(const std::string& name)
	{
		return fastfiles::exists(name, true);
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

			// Allow loading of unsigned fastfiles & imagefiles
			if (!game::environment::is_sp())
			{
				utils::hook::nop(0x368153_b, 2); // DB_InflateInit

				image_file_decrypt_value_hook.create(0x367520_b, image_file_decrypt_value_stub);
				utils::hook::set(0x366F00_b, 0xC301B0);
			}

			if (game::environment::is_sp())
			{
				// Allow loading mp maps
				utils::hook::set(0x40AF90_b, 0xC300B0);
				// Don't sys_error if aipaths are missing
				utils::hook::call(0x2F8EE9_b, db_find_aipaths_stub);
			}
			else
			{
				// Allow loading sp maps on mp
				utils::hook::jump(0x15AFC0_b, get_bsp_filename_stub);
				utils::hook::call(0x112ED8_b, com_sprintf_stub);
			}

			// Allow loading of mixed compressor types
			utils::hook::nop(SELECT_VALUE(0x1C4BE7_b, 0x3687A7_b), 2);

			// Fix compressor type on streamed file load
			db_read_stream_file_hook.create(SELECT_VALUE(0x1FB9D0_b, 0x3A1BF0_b), db_read_stream_file_stub);

			// Add custom zone paths
			sys_createfile_hook.create(game::Sys_CreateFile, sys_create_file_stub);
			if (!game::environment::is_sp())
			{
				db_file_exists_hook.create(0x394DC0_b, db_file_exists_stub);
			}

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

				// dont load localized zone for custom maps
				utils::hook::call(0x394A99_b, db_level_load_add_zone_stub);
			}

			// prevent mod.ff from loading lua files
			if (game::environment::is_mp())
			{
				utils::hook::call(0x3757B4_b, load_lua_file_asset_stub);
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
