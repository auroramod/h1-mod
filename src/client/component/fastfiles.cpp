#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "fastfiles.hpp"
#include "filesystem.hpp"
#include "imagefiles.hpp"
#include "weapon.hpp"

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
		utils::hook::detour db_init_load_x_file_hook;
		utils::hook::detour db_try_load_x_file_internal_hook;
		utils::hook::detour db_find_xasset_header_hook;

		game::dvar_t* g_dump_scripts;
		game::dvar_t* db_print_default_assets;

		utils::concurrency::container<std::vector<HANDLE>> fastfile_handles;
		bool is_mod_pre_gfx = false;

		void db_init_load_x_file_stub(game::DBFile* file, std::uint64_t offset)
		{
			console::info("Loading fastfile %s\n", file->name);
			return db_init_load_x_file_hook.invoke<void>(file, offset);
		}

		void db_try_load_x_file_internal(const char* zone_name, const int flags)
		{
			is_mod_pre_gfx = zone_name == "mod_pre_gfx"s;
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

			{
				const std::string override_asset_name = "override/"s + name;

				if (type == game::XAssetType::ASSET_TYPE_RAWFILE)
				{
					if (result.rawfile)
					{
						const auto override_rawfile = db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, override_asset_name.data(), 0);
						if (override_rawfile.rawfile)
						{
							result.rawfile = override_rawfile.rawfile;
							console::debug("using override asset for rawfile: \"%s\"\n", name);
						}
					}
				}

				if (type == game::XAssetType::ASSET_TYPE_STRINGTABLE)
				{
					if (result.stringTable)
					{
						const auto override_stringtable = db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, override_asset_name.data(), 0);
						if (override_stringtable.stringTable)
						{
							result.stringTable = override_stringtable.stringTable;
							console::debug("using override asset for stringtable: \"%s\"\n", name);
						}
					}
				}
			}

			if (db_print_default_assets->current.enabled && game::DB_IsXAssetDefault(type, name))
			{
				console::warn("Waited %i msec for default asset \"%s\" of type \"%s\"\n",
					diff, name, game::g_assetNames[type]);
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
				fastfile_handles.access([&](std::vector<HANDLE>& handles)
				{
					handles.push_back(handle);
				});
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
			const std::string usermap_pak_file = utils::string::va("%s.pak", usermap_value.data());

			if (mapname == usermap_file || mapname == usermap_load_file || mapname == usermap_pak_file)
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

			if (name.ends_with(".ff") || name.ends_with(".pak"))
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
			imagefiles::close_custom_handles();

			std::vector<game::XZoneInfo> data;
			merge(&data, zoneInfo, zoneCount);

			// code_pre_gfx

			weapon::clear_modifed_enums();
			try_load_zone("mod_pre_gfx", true);
			try_load_zone("h1_mod_pre_gfx", true);

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

		const char* get_zone_name(const unsigned int index)
		{
			if (game::environment::is_sp())
			{
				return game::sp::g_zones[index].name;
			}
			else
			{
				return game::mp::g_zones[index].name;
			}
		}

		utils::hook::detour db_unload_x_zones_hook;
		void db_unload_x_zones_stub(const unsigned short* unload_zones, 
			const unsigned int unload_count, const bool create_default)
		{
			for (auto i = 0u; i < unload_count; i++)
			{
				const auto zone_name = get_zone_name(unload_zones[i]);
				if (zone_name[0] != '\0')
				{
					imagefiles::close_handle(zone_name);
				}
			}

			db_unload_x_zones_hook.invoke<void>(unload_zones, unload_count, create_default);
		}

		namespace mp
		{
			constexpr unsigned int get_asset_type_size(const game::XAssetType type)
			{
				constexpr int asset_type_sizes[] =
				{
					96, 88, 128, 56, 40, 216,
					56, 680, 592, 32, 32, 32,
					32, 32, 2112, 1936, 104,
					32, 24, 152, 152, 152, 16,
					64, 640, 40, 16, 136, 24,
					296, 176, 2864, 48, 0, 24,
					200, 88, 16, 144, 3616, 56,
					64, 16, 16, 0, 0, 0, 0, 24,
					40, 24, 48, 40, 24, 16, 80,
					128, 2256, 136, 32, 72,
					24, 64, 88, 48, 32, 96, 152,
					64, 32, 32,
				};

				return asset_type_sizes[type];
			}

			constexpr unsigned int get_pool_type_size(const game::XAssetType type)
			{
				constexpr int asset_pool_sizes[] =
				{
					128, 256, 16, 1, 128, 5000,
					5248, 4352, 17536, 256, 49152,
					12288, 12288, 72864, 512,
					2750, 23264, 16000, 256, 64,
					64, 64, 64, 8000, 1, 1, 1, 1,
					1, 2, 1, 1, 32, 0, 128, 910,
					16, 14100, 128, 200, 1, 2048,
					4, 6, 0, 0, 0, 0, 1024, 768,
					400, 128, 128, 24, 24, 24,
					32, 32, 2, 128, 64, 384, 128,
					1, 128, 64, 32, 32, 16, 32, 16
				};

				return asset_pool_sizes[type];
			}

			template <game::XAssetType Type, size_t Size>
			char* reallocate_asset_pool()
			{
				constexpr auto element_size = get_asset_type_size(Type);
				static char new_pool[element_size * Size] = {0};
				static_assert(element_size != 0);
				assert(element_size == game::DB_GetXAssetTypeSize(Type));

				std::memmove(new_pool, game::g_assetPool[Type], game::g_poolSize[Type] * element_size);

				game::g_assetPool[Type] = new_pool;
				game::g_poolSize[Type] = Size;

				return new_pool;
			}

			template <game::XAssetType Type, size_t Multiplier>
			char* reallocate_asset_pool_multiplier()
			{
				constexpr auto pool_size = get_pool_type_size(Type);
				return reallocate_asset_pool<Type, pool_size * Multiplier>();
			}

#define RVA(ptr) static_cast<uint32_t>(reinterpret_cast<size_t>(ptr) - 0_b)

			struct buffer_info
			{
				void* ptr;
				size_t size;
			};

			std::vector<buffer_info> string_buffers;
			void memset_stub(void* place, int value, size_t size)
			{
				for (const auto& buffer : string_buffers)
				{
					std::memset(buffer.ptr, 0, buffer.size);
				}

				std::memset(place, value, size);
			}

			void reallocate_weapon_pool()
			{
				constexpr auto multiplier = 2;
				constexpr auto pool_size = get_pool_type_size(game::ASSET_TYPE_WEAPON) * multiplier;
				static void* weapon_complete_defs[pool_size]{};
				static void* weapon_strings[pool_size]{};

				string_buffers.emplace_back(weapon_strings, pool_size * sizeof(void*));

				utils::hook::set<uint32_t>(0x1186A4_b + 4, RVA(weapon_strings));
				utils::hook::set<uint32_t>(0x1186B5_b + 4, RVA(weapon_strings));
				utils::hook::set<uint32_t>(0x104BD2_b + 4, RVA(weapon_strings) - 0x38F1750);

				reallocate_asset_pool<game::ASSET_TYPE_WEAPON, pool_size>();
				
				utils::hook::inject(0x2E3005_b + 3, 
					reinterpret_cast<void*>(reinterpret_cast<size_t>(weapon_complete_defs) + 8));

				utils::hook::inject(0xED734_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1D59F4_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DCEDB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7BB5_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7D35_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2ECCD0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x429B84_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E1DFD_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E21AB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1E8BC9_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DDBA6_b + 3, weapon_complete_defs);
				utils::hook::inject(0x549FF0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x563D20_b + 3, weapon_complete_defs);
				utils::hook::inject(0x563E04_b + 3, weapon_complete_defs);
				utils::hook::inject(0x618464_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DB218_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41ECDC_b + 3, weapon_complete_defs);
				utils::hook::inject(0x42C882_b + 3, weapon_complete_defs);
				utils::hook::inject(0xEFE22_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1199DD_b + 3, weapon_complete_defs);
				utils::hook::inject(0x11D857_b + 3, weapon_complete_defs);
				utils::hook::inject(0x128E28_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DB83B_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DC5BC_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E2549_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E29DF_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6337_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7963_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2F0BA3_b + 3, weapon_complete_defs);
				utils::hook::inject(0x3044C3_b + 3, weapon_complete_defs);
				utils::hook::inject(0x305118_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41B385_b + 3, weapon_complete_defs);
				utils::hook::inject(0x42544A_b + 3, weapon_complete_defs);
				utils::hook::inject(0x425EAB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x426971_b + 3, weapon_complete_defs);
				utils::hook::inject(0x42ACA7_b + 3, weapon_complete_defs);
				utils::hook::inject(0x10A173_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D922C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DD2D0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DE6C4_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2F041F_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41E2BC_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41F054_b + 3, weapon_complete_defs);
				utils::hook::inject(0x427487_b + 3, weapon_complete_defs);
				utils::hook::inject(0x461657_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A351_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A524_b + 3, weapon_complete_defs);
				utils::hook::inject(0x567328_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DE83F_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DF050_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EBFE0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DF290_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A6A0_b + 3, weapon_complete_defs);
				utils::hook::inject(0xE97A0_b + 3, weapon_complete_defs);
				utils::hook::inject(0xE97F0_b + 3, weapon_complete_defs);
				utils::hook::inject(0xE9900_b + 3, weapon_complete_defs);
				utils::hook::inject(0xE9954_b + 3, weapon_complete_defs);
				utils::hook::inject(0xEDAEC_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1BA6FC_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1E99B2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1E9AD2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2818CA_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2845FD_b + 3, weapon_complete_defs);
				utils::hook::inject(0x284C2D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x285694_b + 3, weapon_complete_defs);
				utils::hook::inject(0x285C2C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2C606A_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2CD275_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2CD2B4_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D50A4_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D8B20_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DC824_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DCDE1_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DEA7C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E1463_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E14EF_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E15EB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E17FB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E18EB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E19EB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E339E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E360A_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E3CE0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E56C8_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E5840_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E58BB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E5FE2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6890_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E68F0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6960_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6AB0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6CA0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7640_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E76A0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7700_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7760_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E77C0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7A80_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E8753_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EA650_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EB870_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EC488_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EFD84_b + 3, weapon_complete_defs);
				utils::hook::inject(0x427D5C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x4288F8_b + 3, weapon_complete_defs);
				utils::hook::inject(0x428C89_b + 3, weapon_complete_defs);
				utils::hook::inject(0x43748E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x4376AE_b + 3, weapon_complete_defs);
				utils::hook::inject(0x43796E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54953B_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A21F_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A5F0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A7E7_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A8D9_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54ADA0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54BAC0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x472198_b + 3, weapon_complete_defs);
				utils::hook::inject(0x285FF2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2C3154_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2C3AC0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DD193_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DECC4_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DEE68_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E16EB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E1ACB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E1CFB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E1EDB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E2015_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E20AB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7530_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E8950_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EBA5C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x30307B_b + 3, weapon_complete_defs);
				utils::hook::inject(0x30308E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x30917E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41AE27_b + 3, weapon_complete_defs);
				utils::hook::inject(0x549354_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54A867_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D4FDB_b + 3, weapon_complete_defs);
				utils::hook::inject(0xEB3BA_b + 3, weapon_complete_defs);
				utils::hook::inject(0xFDC77_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1072EB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x11C14E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1270D5_b + 3, weapon_complete_defs);
				utils::hook::inject(0x12868F_b + 3, weapon_complete_defs);
				utils::hook::inject(0x128848_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2C4160_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7B12_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EA7C6_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EAE75_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EB077_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41BB9D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41E64B_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41E868_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41EBCB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x426172_b + 3, weapon_complete_defs);
				utils::hook::inject(0x4262A0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x439669_b + 3, weapon_complete_defs);
				utils::hook::inject(0x45E912_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46284E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46D658_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46DF93_b + 3, weapon_complete_defs);
				utils::hook::inject(0xD597B_b + 3, weapon_complete_defs);
				utils::hook::inject(0xF3375_b + 3, weapon_complete_defs);
				utils::hook::inject(0x121F3A_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1BA9C8_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1D3F28_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2C4220_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D70DB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DB108_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E2FC1_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EB8D9_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EBB85_b + 3, weapon_complete_defs);
				utils::hook::inject(0x40C304_b + 3, weapon_complete_defs);
				utils::hook::inject(0x42A795_b + 3, weapon_complete_defs);
				utils::hook::inject(0x472530_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E9939_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E7F79_b + 3, weapon_complete_defs);
				utils::hook::inject(0x117129_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DE589_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E4D2E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E4E73_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E578E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6686_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6DCD_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41E05A_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41E1D3_b + 3, weapon_complete_defs);
				utils::hook::inject(0x428ECE_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54BB26_b + 3, weapon_complete_defs);
				utils::hook::inject(0x6183A6_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D6FF6_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EBCED_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E041A_b + 3, weapon_complete_defs);
				utils::hook::inject(0xF38FE_b + 3, weapon_complete_defs);
				utils::hook::inject(0xF7880_b + 3, weapon_complete_defs);
				utils::hook::inject(0x102153_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1021FB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x10415B_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1168F5_b + 3, weapon_complete_defs);
				utils::hook::inject(0x126C09_b + 3, weapon_complete_defs);
				utils::hook::inject(0x180552_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1CCFD0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1D929F_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1D9575_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1E8E0E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1E98CC_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D4EF3_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D638B_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DC023_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E31D7_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E3EC8_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E9364_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2FCEDF_b + 3, weapon_complete_defs);
				utils::hook::inject(0x324376_b + 3, weapon_complete_defs);
				utils::hook::inject(0x440497_b + 3, weapon_complete_defs);
				utils::hook::inject(0x460237_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46025A_b + 3, weapon_complete_defs);
				utils::hook::inject(0x461200_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46EBE4_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46EE70_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46F4E1_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46FD48_b + 3, weapon_complete_defs);
				utils::hook::inject(0x5F2479_b + 3, weapon_complete_defs);
				utils::hook::inject(0x6641D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1074F2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1C7B8D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DE40E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DE8B5_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DF63C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DF7DF_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E0CD9_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E2ADD_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E2B7C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E2DB1_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E4C10_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E9F50_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EA0B0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EA535_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2ED0A5_b + 3, weapon_complete_defs);
				utils::hook::inject(0x305581_b + 3, weapon_complete_defs);
				utils::hook::inject(0x3236BD_b + 3, weapon_complete_defs);
				utils::hook::inject(0x3F2CDA_b + 3, weapon_complete_defs);
				utils::hook::inject(0x407BE5_b + 3, weapon_complete_defs);
				utils::hook::inject(0x4155E0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41C61D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41D96E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41DA71_b + 3, weapon_complete_defs);
				utils::hook::inject(0x42A442_b + 3, weapon_complete_defs);
				utils::hook::inject(0x42F56F_b + 3, weapon_complete_defs);
				utils::hook::inject(0x5482BB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54AA1D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1075A9_b + 3, weapon_complete_defs);
				utils::hook::inject(0xC5394_b + 3, weapon_complete_defs);
				utils::hook::inject(0xEEC4D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x11C2E2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x11E6F2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1994C0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x1E9DE0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x285E44_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E1BE1_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E3F34_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E474C_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E6B2E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E9530_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E95B0_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EC08D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2ECC4D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EFD59_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2FCEFE_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2FCF15_b + 3, weapon_complete_defs);
				utils::hook::inject(0x328444_b + 3, weapon_complete_defs);
				utils::hook::inject(0x40F47E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x4169AB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41C481_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41E742_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41EE62_b + 3, weapon_complete_defs);
				utils::hook::inject(0x41EF3E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x45FD83_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46181E_b + 3, weapon_complete_defs);
				utils::hook::inject(0x46EDD8_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54924D_b + 3, weapon_complete_defs);
				utils::hook::inject(0x54AFA5_b + 3, weapon_complete_defs);
				utils::hook::inject(0x617F77_b + 3, weapon_complete_defs);
				utils::hook::inject(0xF30DF_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2BC095_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D605B_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2D8CD1_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DA7DB_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2DC379_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E3121_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2E5953_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EA8C2_b + 3, weapon_complete_defs);
				utils::hook::inject(0x2EE9F7_b + 3, weapon_complete_defs);
				utils::hook::inject(0x303318_b + 3, weapon_complete_defs);
				utils::hook::inject(0x424D33_b + 3, weapon_complete_defs);
				utils::hook::inject(0x429149_b + 3, weapon_complete_defs);
				utils::hook::inject(0x4299F4_b + 3, weapon_complete_defs);
				utils::hook::inject(0x4417C9_b + 3, weapon_complete_defs);
				utils::hook::inject(0x471083_b + 3, weapon_complete_defs);

				utils::hook::inject(0x2E0440_b + 3, weapon_complete_defs);

				utils::hook::set<uint32_t>(0x427EB1_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x4240A8_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x54B8B1_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x2D274B_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x311DA8_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x311EB8_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x323BD1_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x2E0864_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x2F170C_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x42CB3B_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x136327_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x4671FF_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x46722F_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x46754C_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x565FA2_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x56600E_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x2EA352_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x2EA369_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x56483F_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x2EA337_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x402261_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x4022A9_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x41CED5_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x42B540_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x42B560_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x5660CB_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x42B523_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x117C82_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x411438_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x12AB34_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x129F9B_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x12AC16_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x12AC9D_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x424087_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x54B897_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x129F5C_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x42406A_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x54B87B_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x565D1B_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x123FF5_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x42CB1B_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x42CAFE_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x136310_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x46752E_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x1362F3_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0xF454D_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x41CC61_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x41D7FB_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x427E8F_b + 4, RVA(&weapon_complete_defs));
				utils::hook::set<uint32_t>(0x427E72_b + 4, RVA(&weapon_complete_defs));

				utils::hook::set<uint32_t>(0x11865F_b + 4, RVA(&weapon_complete_defs));
			}

			void reallocate_attachment_pool()
			{
				constexpr auto multiplier = 2;
				constexpr auto pool_size = get_pool_type_size(game::ASSET_TYPE_ATTACHMENT) * multiplier;
				reallocate_asset_pool<game::ASSET_TYPE_ATTACHMENT, pool_size>();

				static void* attachment_array[pool_size]{};
				static void* attachment_strings[pool_size]{};

				string_buffers.emplace_back(attachment_strings, pool_size * sizeof(void*));

				utils::hook::inject(0x118599_b + 3, attachment_strings);
				utils::hook::inject(0x441187_b + 3, attachment_strings);
				utils::hook::set<uint32_t>(0x104C8A_b + 4, RVA(attachment_strings) - 0x38F1750);

				const auto sub_118540_stub = [](utils::hook::assembler& a)
				{
					a.mov(rax, reinterpret_cast<size_t>(&attachment_array[0]));
					a.mov(r8d, pool_size);
					a.mov(rdx, rax);
					a.mov(ecx, game::ASSET_TYPE_ATTACHMENT);
					a.call(0x59D460_b);
					a.jmp(0x118573_b);
				};

				const auto loc_1185A0_stub = [](utils::hook::assembler& a)
				{
					a.mov(rax, reinterpret_cast<size_t>(&attachment_array[0]));
					a.push(rbx);
					a.imul(rbx, 8);
					a.mov(rcx, qword_ptr(rax, rbx));
					a.pop(rbx);
					a.cmp(qword_ptr(rcx, 8), 0);
					a.lea(rsi, qword_ptr(rcx, 8));
					a.jmp(0x1185AE_b);
				};

				utils::hook::jump(0x11855F_b, utils::hook::assemble(sub_118540_stub), true);
				utils::hook::jump(0x1185A0_b, utils::hook::assemble(loc_1185A0_stub), true);
			}

			void reallocate_attachment_and_weapon()
			{
				// weapon & attachment strings are reset here (we need to also reset the reallocated ones)
				utils::hook::call(0x13ABBA_b, memset_stub);
				utils::hook::call(0x13AC5C_b, memset_stub);
				utils::hook::call(0x13ACF0_b, memset_stub);
				utils::hook::call(0x17D1C5_b, memset_stub);

				reallocate_weapon_pool();
				reallocate_attachment_pool();
			}

			void reallocate_sound_pool()
			{
				constexpr auto original_pool_size = get_pool_type_size(game::ASSET_TYPE_SOUND);
				constexpr auto multiplier = 2;
				constexpr auto pool_size = original_pool_size * multiplier;

				const auto pool = reallocate_asset_pool<game::ASSET_TYPE_SOUND, pool_size>();
				utils::hook::inject(0x39621D_b + 3, reinterpret_cast<void*>(reinterpret_cast<size_t>(pool) + 8));

				static unsigned short net_const_string_sound_map[pool_size]{};
				utils::hook::inject(0x2B0CEA_b + 3, net_const_string_sound_map);
				utils::hook::inject(0x2B0F52_b + 3, net_const_string_sound_map);
				utils::hook::inject(0x2B1866_b + 3, net_const_string_sound_map);
				utils::hook::inject(0x2B1CC7_b + 3, net_const_string_sound_map);
			}

			void reallocate_asset_pools()
			{
				reallocate_attachment_and_weapon();
				reallocate_sound_pool();
				reallocate_asset_pool_multiplier<game::ASSET_TYPE_XANIM, 2>();
				reallocate_asset_pool_multiplier<game::ASSET_TYPE_LOADED_SOUND, 2>();
				reallocate_asset_pool_multiplier<game::ASSET_TYPE_LOCALIZE_ENTRY, 2>();
			}
		}

		namespace sp
		{
			constexpr unsigned int get_asset_type_size(const game::XAssetType type)
			{
				constexpr int asset_type_sizes[] =
				{
					96, 88, 128, 56, 40, 216,
					56, 680, 592, 32, 32, 32,
					32, 32, 2112, 1936, 104,
					32, 24, 152, 152, 152, 16,
					64, 640, 40, 16, 136, 24,
					296, 176, 2864, 48, 0, 24,
					200, 88, 16, 144, 3616, 56,
					64, 16, 16, 0, 0, 0, 0, 24,
					40, 24, 48, 40, 24, 16, 80,
					128, 2256, 136, 32, 72,
					24, 64, 88, 48, 32, 96, 152,
					64, 32, 32,
				};

				return asset_type_sizes[type];
			}

			constexpr unsigned int get_pool_type_size(const game::XAssetType type)
			{
				constexpr int asset_pool_sizes[] =
				{
					128, 1024, 16, 1, 128, 5000, 5248,
					2560, 10624, 256, 49152, 12288, 12288,
					72864, 512, 2750, 12000, 16000, 256, 
					64, 64, 64, 64, 8000, 1, 1, 1, 1,
					1, 2, 1, 1, 32, 0, 128,
					400, 0, 11500, 128, 360, 1, 2048,
					4, 6, 0, 0, 0, 0, 1024,
					768, 400, 128, 128, 24, 24, 24,
					32, 128, 2, 0, 64, 384, 128,
					1, 128, 64, 32, 32, 16, 32, 16,
				};

				return asset_pool_sizes[type];
			}

			template <game::XAssetType Type, size_t Size>
			char* reallocate_asset_pool()
			{
				constexpr auto element_size = get_asset_type_size(Type);
				static char new_pool[element_size * Size] = {0};
				static_assert(element_size != 0);
				assert(element_size == game::DB_GetXAssetTypeSize(Type));

				std::memmove(new_pool, game::g_assetPool[Type], game::g_poolSize[Type] * element_size);

				game::g_assetPool[Type] = new_pool;
				game::g_poolSize[Type] = Size;

				return new_pool;
			}

			template <game::XAssetType Type, size_t Multiplier>
			char* reallocate_asset_pool_multiplier()
			{
				constexpr auto pool_size = get_pool_type_size(Type);
				return reallocate_asset_pool<Type, pool_size* Multiplier>();
			}

			void reallocate_asset_pools()
			{
				reallocate_asset_pool_multiplier<game::ASSET_TYPE_LOCALIZE_ENTRY, 2>();
			}
		}

		void reallocate_asset_pools()
		{
			if (!game::environment::is_sp())
			{
				mp::reallocate_asset_pools();
			}
			else
			{
				sp::reallocate_asset_pools();
			}
		}

		utils::hook::detour db_link_x_asset_entry_hook;
		game::XAssetEntry* db_link_x_asset_entry_stub(game::XAssetType type, game::XAssetHeader* header)
		{
			if (!is_mod_pre_gfx)
			{
				return db_link_x_asset_entry_hook.invoke<game::XAssetEntry*>(type, header);
			}

			static game::XAssetEntry entry{};

			if (type != game::ASSET_TYPE_STRINGTABLE)
			{
				return &entry;
			}

			return db_link_x_asset_entry_hook.invoke<game::XAssetEntry*>(type, header);
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
		fastfile_handles.access([&](std::vector<HANDLE>& handles)
		{
			for (const auto& handle : handles)
			{
				CloseHandle(handle);
			}
		});
		
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
			db_try_load_x_file_internal_hook.create(SELECT_VALUE(0x1F5700_b, 0x39A620_b), db_try_load_x_file_internal);
			db_init_load_x_file_hook.create(SELECT_VALUE(0x1C46E0_b, 0x3681E0_b), db_init_load_x_file_stub);
			db_find_xasset_header_hook.create(game::DB_FindXAssetHeader, db_find_xasset_header_stub);

			db_unload_x_zones_hook.create(SELECT_VALUE(0x1F6040_b, 
				0x39B3C0_b), db_unload_x_zones_stub);

			db_print_default_assets = dvars::register_bool("db_printDefaultAssets",
				false, game::DVAR_FLAG_SAVED, "Print default asset usage");

			if (!game::environment::is_sp())
			{
				db_link_x_asset_entry_hook.create(0x396E80_b, db_link_x_asset_entry_stub);
			}

			g_dump_scripts = dvars::register_bool("g_dumpScripts", false, game::DVAR_FLAG_NONE, "Dump GSC scripts");

			reallocate_asset_pools();

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

			command::add("poolUsages", []()
			{
				for (auto i = 0; i < game::ASSET_TYPE_COUNT; i++)
				{
					auto count = 0;
					enum_assets(static_cast<game::XAssetType>(i), [&](game::XAssetHeader header)
					{
						count++;
					}, true);

					console::info("%i %s: %i / %i\n", i, game::g_assetNames[i], count, game::g_poolSize[i]);
				}
			});

			command::add("poolUsage", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: poolUsage <type>\n");
					return;
				}

				const auto type = static_cast<game::XAssetType>(std::atoi(params.get(1)));

				auto count = 0;
				enum_assets(type, [&](game::XAssetHeader header)
				{
					count++;
				}, true);

				console::info("%i %s: %i / %i\n", type, game::g_assetNames[type], count, game::g_poolSize[type]);
			});

			command::add("assetCount", [](const command::params& params)
			{
				auto count = 0;
				for (auto i = 0; i < game::ASSET_TYPE_COUNT; i++)
				{
					enum_assets(static_cast<game::XAssetType>(i), [&](game::XAssetHeader header)
					{
						count++;
					}, true);
				}

				console::info("assets: %i / %i\n", count, 155000);
			});
		}
	};
}

REGISTER_COMPONENT(fastfiles::component)
