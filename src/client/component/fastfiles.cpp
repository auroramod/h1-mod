#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "fastfiles.hpp"

#include "command.hpp"
#include "console.hpp"
#include "filesystem.hpp"
#include "imagefiles.hpp"
#include "weapon.hpp"
#include "optimization.hpp"
#include "fastfiles_material_sites.hpp"
#include "fastfiles_sorted_material_sites.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

static utils::concurrency::container<std::string> current_fastfile;
static utils::concurrency::container<std::optional<std::string>> current_usermap;
static utils::concurrency::container<std::vector<HANDLE>> fastfile_handles;

static std::vector<fastfiles::buffer_info> string_buffers;

static utils::hook::detour db_init_load_x_file_hook;
static utils::hook::detour db_try_load_x_file_internal_hook;
static utils::hook::detour db_find_xasset_header_hook;
static utils::hook::detour db_read_stream_file_hook;
static utils::hook::detour sys_createfile_hook;
static utils::hook::detour db_file_exists_hook;
static utils::hook::detour image_file_decrypt_value_hook;
static utils::hook::detour db_unload_x_zones_hook;
static utils::hook::detour db_link_xasset_entry_hook;
static utils::hook::detour db_process_transient_asset_list_hook;
static utils::hook::detour cl_transient_register_file_hook;

std::uint8_t fastfiles::transient_files[transient_file_count * transient_file_size]{};

static char last_zone_name[256]{};
static char last_asset_name[256]{};
static volatile int last_asset_type = -1;
static volatile int pending_asset_type = -1;

static game::dvar_t* g_dump_scripts = nullptr;
static game::dvar_t* db_print_default_assets = nullptr;

namespace
{
	constexpr std::uint32_t image_pool_size = 24000;

	struct image_stream_entry
	{
		std::uint64_t start;
		std::uint64_t end;
		void* file;
	};

	using stream_compare_t = bool(*)(std::uint32_t a, std::uint32_t b);

	std::uint32_t* stream_loaded_bits = nullptr;
	std::int32_t* stream_priorities = nullptr;
	std::uint32_t* stream_ids = nullptr;
	image_stream_entry* stream_entries = nullptr;

	std::uint32_t stream_read_list[0x200]{}; // word_145331A00
}

struct fastfiles::stream_select_context
{
	char* stream;
	char __pad0[8];
	int urgent;
	char __pad1[4];
	void* current_file;
	std::uint64_t position;
};

void fastfiles::post_unpack()
{
	db_try_load_x_file_internal_hook.create(SELECT_VALUE(0x1401F5700, 0x1402BFFE0), db_try_load_x_file_internal); // DB_TryLoadXFileInternal
	db_init_load_x_file_hook.create(SELECT_VALUE(0x1401C46E0, 0x14028DE30), db_init_load_x_file_stub);
	db_find_xasset_header_hook.create(game::DB_FindXAssetHeader, db_find_xasset_header_stub);
	db_unload_x_zones_hook.create(SELECT_VALUE(0x1401F6040, 0x1402C0BC0), db_unload_x_zones_stub);

	if (!game::environment::is_sp())
	{
		// track the last linked asset for crash dumps
		db_link_xasset_entry_hook.create(0x1402BC920, db_link_xasset_entry_stub);

		db_process_transient_asset_list_hook.create(0x1402BEBE0, db_process_transient_asset_list_stub);
		cl_transient_register_file_hook.create(0x14005D7D0, cl_transient_register_file_stub);
		utils::hook::call(0x1402B819D, cl_transient_temp_alloc_stub); // DB_AllocXZoneMemoryInternal transient XFILE_BLOCK_TEMP
	}

	db_print_default_assets = dvars::register_bool("db_printDefaultAssets",
		false, game::DVAR_ARCHIVE, "Print default asset usage");

	g_dump_scripts = dvars::register_bool("g_dumpScripts", false, game::DVAR_NOFLAG, "Dump GSC scripts");

	reallocate_asset_pools();

	if (game::environment::is_sp())
	{
		// Allow loading mp maps
		utils::hook::set(0x14040AF90, 0xC300B0);

		// Don't sys_error if aipaths are missing
		utils::hook::call(0x1402F8EE9, db_find_aipaths_stub);
	}
	else
	{
		// Allow loading of unsigned fastfiles & imagefiles
		utils::hook::nop(0x14028DDB3, 2); // DB_InflateInit
		image_file_decrypt_value_hook.create(0x14028D1C0, image_file_decrypt_value_stub); // Imagefile_DecryptValue
		utils::hook::set(0x14028CBA0, 0xC301B0);

		// Allow loading sp maps on mp
		utils::hook::jump(0x1404EBAC0, get_bsp_filename_stub); // Com_GetBspFilename
	}

	// Allow loading of mixed compressor types
	utils::hook::nop(SELECT_VALUE(0x1401C4BE7, 0x14028E447), 2);

	// Fix compressor type on streamed file load
	db_read_stream_file_hook.create(SELECT_VALUE(0x1401FB9D0, 0x1402C6840), db_read_stream_file_stub); // DB_ReadStreamFile

	// Add custom zone paths
	sys_createfile_hook.create(game::Sys_CreateFile, sys_create_file_stub);
	if (!game::environment::is_sp())
	{
		db_file_exists_hook.create(0x1402BA970, db_file_exists_stub); // DB_FileExists
	}

	// load our custom pre_gfx zones
	utils::hook::call(SELECT_VALUE(0x1403862ED, 0x1400D9B9D), load_pre_gfx_zones);
	utils::hook::call(SELECT_VALUE(0x1403865E7, 0x1400D9E94), load_pre_gfx_zones);

	// load our custom ui and common zones
	utils::hook::call(SELECT_VALUE(0x1405634AA, 0x1405DF5C1), load_post_gfx_and_ui_and_common_zones);

	// load our custom ui zones
	utils::hook::call(SELECT_VALUE(0x1403A5676, 0x1400DA472), load_ui_zones);

	// Don't load extra zones with loadzone
	if (game::environment::is_sp())
	{
		utils::hook::nop(0x1401F3FF9, 13);
		utils::hook::jump(0x1401F3FF9, utils::hook::assemble(skip_extra_zones_stub_sp), true);
	}
	else
	{
		utils::hook::nop(0x1402BDA91, 15);
		utils::hook::jump(0x1402BDA91, utils::hook::assemble(skip_extra_zones_stub_mp), true);

		// dont load localized zone for custom maps
		utils::hook::call(0x1402BA667, db_level_load_add_zone_stub);

		// handle custom vlobby maps
		utils::hook::call(0x1400DBFA7, db_load_xassets_vlobby_stub);
		utils::hook::call(0x1400DBFD4, wait_for_vlobby_stub); // dont wait for _path ff if it doesnt exist

		// Show missing fastfiles
		utils::hook::call(0x1402C0177, missing_content_error_stub);

		// 1.15 zones stores aliased pointers in proto arrays
		utils::hook::nop(0x14029F235, 6);
		utils::hook::jump(0x14029F235, create_alias_ptr_stub(0x14029F23B, 0x14029F286));
		utils::hook::nop(0x14029EF8D, 6);
		utils::hook::jump(0x14029EF8D, create_alias_ptr_stub(0x14029EF93, 0x14029EFE8));
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
			enum_assets(static_cast<game::XAssetType>(i), [&](game::XAssetHeader /*header*/)
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
		enum_assets(type, [&](game::XAssetHeader /*header*/)
		{
			count++;
		}, true);

		console::info("%i %s: %i / %i\n", type, game::g_assetNames[type], count, game::g_poolSize[type]);
	});

	command::add("assetCount", [](const command::params& /*params*/)
	{
		auto count = 0;
		for (auto i = 0; i < game::ASSET_TYPE_COUNT; i++)
		{
			enum_assets(static_cast<game::XAssetType>(i), [&](game::XAssetHeader /*header*/)
			{
				count++;
			}, true);
		}

		console::info("assets: %i / %i\n", count, 130000);
	});
}

void fastfiles::db_init_load_x_file_stub(game::DBFile* file, std::uint64_t offset)
{
	console::info("Loading fastfile %s\n", file->name);
	db_init_load_x_file_hook.invoke<void>(file, offset);
}

void fastfiles::db_try_load_x_file_internal(const char* zone_name, const int flags)
{
	current_fastfile.access([&](std::string& fastfile)
	{
		fastfile = zone_name;
	});

	strncpy_s(last_zone_name, zone_name, _TRUNCATE);

	optimization::begin_zone_load(zone_name);
	db_try_load_x_file_internal_hook.invoke<void>(zone_name, flags);
	optimization::end_zone_load();
}

game::XAssetEntry* fastfiles::db_link_xasset_entry_stub(const game::XAssetType type, game::XAssetHeader* header)
{
	pending_asset_type = type;
	const auto result = db_link_xasset_entry_hook.invoke<game::XAssetEntry*>(type, header);
	pending_asset_type = -1;

	const auto* name = result && result->asset.header.data ? game::DB_GetXAssetName(&result->asset) : nullptr;
	strncpy_s(last_asset_name, name ? name : "", _TRUNCATE);
	last_asset_type = type;

	return result;
}

std::string fastfiles::get_load_state()
{
	const auto type_name = [](const int type)
	{
		return type >= 0 && type < game::ASSET_TYPE_COUNT ? game::g_assetNames[type] : "none";
	};

	return utils::string::va("zone: %s, last asset: %s (%s), linking: %s",
		last_zone_name, last_asset_name, type_name(last_asset_type), type_name(pending_asset_type));
}

void fastfiles::dump_gsc_script(const std::string& name, game::XAssetHeader header)
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

game::XAssetHeader fastfiles::db_find_xasset_header_stub(game::XAssetType type, const char* name, int allow_create_default)
{
	const auto start = game::Sys_Milliseconds();
	auto result = db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, name, allow_create_default);
	const auto diff = game::Sys_Milliseconds() - start;

	if (type == game::XAssetType::ASSET_TYPE_SCRIPTFILE)
	{
		dump_gsc_script(name, result);
	}

	if (type == game::XAssetType::ASSET_TYPE_RAWFILE ||
		type == game::XAssetType::ASSET_TYPE_STRINGTABLE ||
		type == game::XAssetType::ASSET_TYPE_DDL ||
		type == game::XAssetType::ASSET_TYPE_MENU)
	{
		const std::string override_asset_name = "override/"s + name;
		if (result.rawfile)
		{
			const auto override_rawfile = db_find_xasset_header_hook.invoke<game::XAssetHeader>(type, override_asset_name.data(), 0);
			if (override_rawfile.rawfile)
			{
				result.rawfile = override_rawfile.rawfile;
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

void fastfiles::db_read_stream_file_stub(int a1, int a2)
{
	// always use lz4 compressor type when reading stream files
	*game::g_compressor = 4;
	db_read_stream_file_hook.invoke<void>(a1, a2);
}

void fastfiles::missing_content_error_stub()
{
	game::Com_Error(game::ERR_DROP, utils::string::va("Missing fastfile %s.ff",
		get_current_fastfile().data()));
}

void* fastfiles::create_alias_ptr_stub(const size_t load_inline, const size_t skip)
{
	return utils::hook::assemble([=](utils::hook::assembler& a)
	{
		const auto null_ptr = a.newLabel();
		const auto alias_ptr = a.newLabel();

		a.mov(rax, qword_ptr(rcx));
		a.test(rax, rax);
		a.jz(null_ptr);

		a.mov(rdx, 0xFDFDFDFFFFFFFFFF);
		a.cmp(rax, rdx);
		a.jnz(alias_ptr);
		a.jmp(load_inline);

		a.bind(alias_ptr);
		a.call(0x1402C4AE0); // DB_ConvertOffsetToPointer

		a.bind(null_ptr);
		a.jmp(skip);
	});
}

void fastfiles::skip_extra_zones_stub_mp(utils::hook::assembler& a)
{
	const auto skip = a.newLabel();
	const auto original = a.newLabel();

	a.pushad64();
	a.test(esi, game::DB_ZONE_CUSTOM); // allocFlags
	a.jnz(skip);

	a.bind(original);
	a.popad64();
	a.mov(rdx, 0x140823080);
	a.mov(rcx, rbp);
	a.call(0x140793730);
	a.jmp(0x1402BDAA0);

	a.bind(skip);
	a.popad64();
	a.mov(r14d, game::DB_ZONE_CUSTOM);
	a.not_(r14d);
	a.and_(esi, r14d);
	a.jmp(0x1402BDB7F);
}

void fastfiles::skip_extra_zones_stub_sp(utils::hook::assembler& a)
{
	const auto skip = a.newLabel();
	const auto original = a.newLabel();

	a.pushad64();
	a.test(ebp, game::DB_ZONE_CUSTOM); // allocFlags
	a.jnz(skip);

	a.bind(original);
	a.popad64();
	a.mov(r8d, 9);
	a.mov(rdx, 0x140782210);
	a.jmp(0x1401F4006);

	a.bind(skip);
	a.popad64();
	a.mov(r15d, game::DB_ZONE_CUSTOM);
	a.not_(r15d);
	a.and_(ebp, r15d);
	a.jmp(0x1401F4023);
}

bool fastfiles::try_load_zone(std::string name, const bool localized, const bool game)
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

	if (!exists(name))
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

HANDLE fastfiles::find_fastfile(const std::string& filename, const bool check_loc_folder)
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

HANDLE fastfiles::find_usermap(const std::string& mapname)
{
	const auto usermap = get_current_usermap();
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

HANDLE fastfiles::sys_create_file(game::Sys_Folder folder, const char* base_filename, const bool ignore_usermap)
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

HANDLE fastfiles::sys_create_file_stub(game::Sys_Folder folder, const char* base_filename)
{
	return sys_create_file(folder, base_filename, false);
}

void* fastfiles::cl_transient_temp_alloc_stub(const std::uint64_t size, [[maybe_unused]] const std::uint32_t alignment)
{
	// transient zones put XFILE_BLOCK_TEMP in the 0x1000 byte mp_transient_temp buffer without a size check,
	// some 1.15 zones (mp_vm_rpg_base_tr, mp_vm_febsnp_base_tr) need more than its 64KB commit
	constexpr std::uint64_t game_buffer_size = 0x1000;
	static void* buffer = nullptr;
	static std::uint64_t buffer_size = 0;

	if (size <= game_buffer_size)
	{
		return *game::mp::s_transientTempBuffer;
	}

	if (size > buffer_size)
	{
		if (buffer)
		{
			VirtualFree(buffer, 0, MEM_RELEASE);
		}

		buffer_size = (size + 0xFFFF) & ~0xFFFFull;
		buffer = VirtualAlloc(nullptr, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!buffer)
		{
			buffer_size = 0;
			game::Com_Error(game::ERR_FATAL, "Failed to allocate 0x%llX bytes of transient temp memory", size);
		}
	}

	return buffer;
}

void fastfiles::db_process_transient_asset_list_stub(const char* name, int is_patch, int is_dlc)
{
	// 1.04 only dedupes dlc lists, 1.15 also dedupes the _patch lists that re-register base files and assets
	db_process_transient_asset_list_hook.invoke<void>(name, is_patch, is_patch || is_dlc);
}

int fastfiles::cl_transient_register_file_stub(const char* name, const std::uint8_t pool, const std::uint32_t unused, const int dedupe)
{
	if (dedupe)
	{
		if (const auto index = merge_transient_patch_file(name); index >= 0)
		{
			return index;
		}
	}

	return cl_transient_register_file_hook.invoke<int>(name, pool, unused, dedupe);
}

int fastfiles::merge_transient_patch_file(const char* name)
{
	constexpr std::string_view patch_suffix = "_p_tr";
	const std::string_view patch_name = name;
	if (patch_name.size() <= patch_suffix.size() + 1 || !patch_name.ends_with(patch_suffix))
	{
		return -1;
	}

	const auto base_name = std::string(patch_name.substr(0, patch_name.size() - patch_suffix.size())) + "_tr";
	const auto file = game::mp::CL_TransientMem_FindFileByHash(get_transient_file_hash(base_name.data()));
	if (!file)
	{
		return -1;
	}

	const auto index = static_cast<std::uint16_t>((file - transient_files) / transient_file_size);
	const auto hash = get_transient_file_hash(name);
	if (game::mp::CL_TransientMem_FindFileByHash(hash))
	{
		return index;
	}

	strncpy_s(reinterpret_cast<char*>(file + 9), 0x23, name, _TRUNCATE);
	*reinterpret_cast<std::uint32_t*>(file) = hash;

	constexpr std::uint16_t invalid_index = 0xFFFF;
	auto* link = game::mp::s_transientFileHashTable.get() + hash % 0x805;
	while (*link != invalid_index)
	{
		if (*link == index)
		{
			return index;
		}

		link = reinterpret_cast<std::uint16_t*>(transient_files + *link * transient_file_size + 4);
	}

	*link = index;
	return index;
}

std::uint32_t fastfiles::get_transient_file_hash(const char* name)
{
	std::uint32_t hash = 0;
	for (auto c = name; *c; c++)
	{
		hash = (hash * 0x1000193) ^ static_cast<std::uint8_t>(*c);
	}

	return hash;
}

bool fastfiles::db_file_exists_stub(const char* file, int a2)
{
	if (const auto file_exists = db_file_exists_hook.invoke<bool>(file, a2))
	{
		return file_exists;
	}

	return usermap_exists(file);
}

template <typename T>
void fastfiles::merge(std::vector<T>* target, T* source, size_t length)
{
	if (source)
	{
		for (size_t i = 0; i < length; ++i)
		{
			target->push_back(source[i]);
		}
	}
}

void fastfiles::load_pre_gfx_zones(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode)
{
	imagefiles::close_custom_handles();

	std::vector<game::XZoneInfo> data;
	merge(&data, zone_info, zone_count);

	// code_pre_gfx

	weapon::clear_modifed_enums();
	try_load_zone("mod_pre_gfx", true);
	try_load_zone("h1_mod_pre_gfx", true);

	game::DB_LoadXAssets(data.data(), static_cast<std::uint32_t>(data.size()), sync_mode);
}

void fastfiles::load_post_gfx_and_ui_and_common_zones(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode)
{
	std::vector<game::XZoneInfo> data;
	merge(&data, zone_info, zone_count);

	// code_post_gfx
	// ui
	// common

	try_load_zone("h1_mod_common", true);

	game::DB_LoadXAssets(data.data(), static_cast<std::uint32_t>(data.size()), sync_mode);

	try_load_zone("mod", true);
}

void fastfiles::load_ui_zones(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode)
{
	std::vector<game::XZoneInfo> data;
	merge(&data, zone_info, zone_count);

	// ui

	game::DB_LoadXAssets(data.data(), static_cast<std::uint32_t>(data.size()), sync_mode);
}

bool fastfiles::is_builtin_map(const char* name)
{
	for (auto map = &game::maps[0]; map->unk; ++map)
	{
		if (!std::strcmp(map->name, name))
		{
			return true;
		}
	}

	return false;
}

void fastfiles::db_level_load_add_zone_stub(void* load, const char* name, const unsigned int alloc_flags,
	const size_t size_est)
{
	if (is_builtin_map(name))
	{
		game::DB_LevelLoadAddZone(load, name, alloc_flags, size_est);
	}
	else
	{
		game::DB_LevelLoadAddZone(load, name, alloc_flags | game::DB_ZONE_CUSTOM, size_est);
	}
}

void fastfiles::db_find_aipaths_stub(game::XAssetType type, const char* name, int allow_create_default)
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

int fastfiles::format_bsp_name(char* filename, int size, const char* mapname)
{
	std::string name = mapname;
	auto fmt = "maps/%s.d3dbsp";
	if (name.starts_with("mp_"))
	{
		fmt = "maps/mp/%s.d3dbsp";
	}

	return game::Com_sprintf(filename, size, fmt, mapname);
}

void fastfiles::get_bsp_filename_stub(char* filename, int size, const char* mapname)
{
	auto base_mapname = mapname;
	game::Com_IsAddonMap(mapname, &base_mapname);
	format_bsp_name(filename, size, base_mapname);
}

bool fastfiles::image_file_decrypt_value_stub(char* value, int size, char* buffer)
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

const char* fastfiles::get_zone_name_internal(const unsigned int index)
{
	if (game::environment::is_sp())
	{
		return game::sp::g_zones[index].name;
	}

	return game::mp::g_zones[index].name;
}

void fastfiles::db_unload_x_zones_stub(const unsigned short* unload_zones,
	const unsigned int unload_count, const bool create_default)
{
	for (auto i = 0u; i < unload_count; i++)
	{
		const auto zone_name = get_zone_name_internal(unload_zones[i]);
		if (zone_name[0] != '\0')
		{
			imagefiles::close_handle(zone_name);
		}
	}

	db_unload_x_zones_hook.invoke<void>(unload_zones, unload_count, create_default);
}

void fastfiles::db_load_xassets_vlobby_stub(game::XZoneInfo* zone_info, unsigned int zone_count, game::DBSyncMode sync_mode)
{
	if (!is_builtin_map(zone_info->name))
	{
		set_usermap(zone_info->name);
		zone_info->allocFlags |= game::DB_ZONE_CUSTOM;
	}

	game::DB_LoadXAssets(zone_info, zone_count, sync_mode);
}

char fastfiles::wait_for_vlobby_stub(const char* zone, int a2)
{
	static const auto virtual_lobby_map = *reinterpret_cast<game::dvar_t**>(0x1425F6DA0); // virtualLobbyMap
	if (*zone == 0 || exists(zone))
	{
		return game::mp::DB_IsFileLoaded(zone, a2);
	}

	return game::mp::DB_IsFileLoaded(virtual_lobby_map->current.string, a2);
}

constexpr unsigned int fastfiles::get_asset_type_size(const game::XAssetType type)
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

constexpr unsigned int fastfiles::get_pool_type_size(const game::XAssetType type)
{
	constexpr int asset_pool_sizes[] =
	{
		128, 256, 16, 1, 128, 5000,
		5248, 4352, 10624, 256, 49152,
		12288, 12288, 72864, 512,
		2750, 23264, 12000, 256, 64,
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

constexpr unsigned int fastfiles::get_pool_type_size_sp(const game::XAssetType type)
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
char* fastfiles::reallocate_asset_pool()
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
char* fastfiles::reallocate_asset_pool_multiplier()
{
	constexpr auto pool_size = get_pool_type_size(Type);
	return reallocate_asset_pool<Type, pool_size * Multiplier>();
}

void fastfiles::memset_stub(void* place, int value, size_t size)
{
	for (const auto& buffer : string_buffers)
	{
		std::memset(buffer.ptr, 0, buffer.size);
	}

	std::memset(place, value, size);
}

void fastfiles::reallocate_weapon_pool()
{
	constexpr auto multiplier = 2;
	constexpr auto pool_size = get_pool_type_size(game::ASSET_TYPE_WEAPON) * multiplier;
	static void* weapon_complete_defs[pool_size]{};
	static void* weapon_strings[pool_size]{};

	string_buffers.emplace_back(weapon_strings, pool_size * sizeof(void*));

	utils::hook::set<uint32_t>(0x1400C47C4 + 4, RVA(weapon_strings));
	utils::hook::set<uint32_t>(0x1400C47D5 + 4, RVA(weapon_strings));
	utils::hook::set<uint32_t>(0x1400B58A2 + 4, RVA(weapon_strings) - 0x2917A30);

	reallocate_asset_pool<game::ASSET_TYPE_WEAPON, pool_size>();

	utils::hook::inject(0x1401FA255 + 3, weapon_complete_defs + 8);

	utils::hook::inject(0x14005158D + 3, weapon_complete_defs);
	utils::hook::inject(0x14009FDC0 + 3, weapon_complete_defs);
	utils::hook::inject(0x14009FED0 + 3, weapon_complete_defs);
	utils::hook::inject(0x14009FF24 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400A1A30 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400A3855 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400A5C81 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400A81BF + 3, weapon_complete_defs);
	utils::hook::inject(0x1400A8450 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400A8A59 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400AB11B + 3, weapon_complete_defs);
	utils::hook::inject(0x1400B1153 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400B44AB + 3, weapon_complete_defs);
	utils::hook::inject(0x1400B512B + 3, weapon_complete_defs);
	utils::hook::inject(0x1400B7CB5 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400B7EC2 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400B7F76 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400BA2E3 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400C2C18 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400C57B6 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400C772E + 3, weapon_complete_defs);
	utils::hook::inject(0x1400C78C1 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400C8E18 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400C9CAE + 3, weapon_complete_defs);
	utils::hook::inject(0x1400CD451 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400D1CD7 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400D2165 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400D3630 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400D384E + 3, weapon_complete_defs);
	utils::hook::inject(0x1400D3DEE + 3, weapon_complete_defs);
	utils::hook::inject(0x1400DCABD + 3, weapon_complete_defs);
	utils::hook::inject(0x1400E1EE0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400E69AC + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F30F4 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F3F15 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F66E2 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F6BA4 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F70AF + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F8DA0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F950C + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F95F2 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F9712 + 3, weapon_complete_defs);
	utils::hook::inject(0x1400F9A13 + 3, weapon_complete_defs);
	utils::hook::inject(0x14017CBFA + 3, weapon_complete_defs);
	utils::hook::inject(0x14017E55E + 3, weapon_complete_defs);
	utils::hook::inject(0x14018110A + 3, weapon_complete_defs);
	utils::hook::inject(0x1401815E4 + 3, weapon_complete_defs);
	utils::hook::inject(0x140181CF0 + 3, weapon_complete_defs);
	utils::hook::inject(0x140181ECF + 3, weapon_complete_defs);
	utils::hook::inject(0x140194B6B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401D45F5 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401DB214 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401DBB40 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401DC1A0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401DC260 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401DE00A + 3, weapon_complete_defs);
	utils::hook::inject(0x1401E4E45 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401E4E84 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401EBB50 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401EC573 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401EC65B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401EC724 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401ED60B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401ED93B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401EE526 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401EE60B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F00A0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F0222 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F076C + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F1CE8 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F1D86 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F24D8 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F25E8 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F2C0B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F34D3 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F3819 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F3A5C + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F3CC4 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F4281 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F437B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F46D3 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F4810 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F4F57 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F576E + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F58E9 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F5A24 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F5B9F + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F5C15 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F5DDC + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F6024 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F61C8 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F63B0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F65F0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F699C + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F6B3F + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F775A + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F7EF9 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F85D3 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F865F + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F875B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F885B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F896B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F8A5B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F8B5B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F8C3B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F8D51 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F8E6B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F8F6D + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F904B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F9185 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F921B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F931B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F96B5 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F9C1F + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F9D0D + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F9DAC + 3, weapon_complete_defs);
	utils::hook::inject(0x1401F9FCC + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FA211 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FA371 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FA427 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FA5AE + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FA7EA + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FAE60 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FB048 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FB0B4 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FB8CC + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FBD90 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FBEAE + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FBFF3 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FC848 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FC90E + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FC9C0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FCA3B + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FCACD + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FD062 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FD3B7 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FD706 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FD840 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FD8A0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FD910 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FDA60 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FDAD1 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FDC20 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FDD4D + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE490 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE5A0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE600 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE660 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE6C0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE720 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE8C3 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FE9E0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FEA72 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FEB15 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FEC95 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FEED9 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FF493 + 3, weapon_complete_defs);
	utils::hook::inject(0x1401FF690 + 3, weapon_complete_defs);
	utils::hook::inject(0x1402000D0 + 3, weapon_complete_defs);
	utils::hook::inject(0x140200150 + 3, weapon_complete_defs);
	utils::hook::inject(0x1402004D9 + 3, weapon_complete_defs);
	utils::hook::inject(0x140200A70 + 3, weapon_complete_defs);
	utils::hook::inject(0x140200BD0 + 3, weapon_complete_defs);
	utils::hook::inject(0x140201055 + 3, weapon_complete_defs);
	utils::hook::inject(0x140201170 + 3, weapon_complete_defs);
	utils::hook::inject(0x1402012E6 + 3, weapon_complete_defs);
	utils::hook::inject(0x1402013E2 + 3, weapon_complete_defs);
	utils::hook::inject(0x140201657 + 3, weapon_complete_defs);
	utils::hook::inject(0x140201717 + 3, weapon_complete_defs);
	utils::hook::inject(0x140201A75 + 3, weapon_complete_defs);
	utils::hook::inject(0x140201C77 + 3, weapon_complete_defs);
	utils::hook::inject(0x140202620 + 3, weapon_complete_defs);
	utils::hook::inject(0x140202689 + 3, weapon_complete_defs);
	utils::hook::inject(0x14020280C + 3, weapon_complete_defs);
	utils::hook::inject(0x14020292D + 3, weapon_complete_defs);
	utils::hook::inject(0x140202D00 + 3, weapon_complete_defs);
	utils::hook::inject(0x140202DAD + 3, weapon_complete_defs);
	utils::hook::inject(0x1402031A8 + 3, weapon_complete_defs);
	utils::hook::inject(0x14020396D + 3, weapon_complete_defs);
	utils::hook::inject(0x1402039F0 + 3, weapon_complete_defs);
	utils::hook::inject(0x140203CD5 + 3, weapon_complete_defs);
	utils::hook::inject(0x140205627 + 3, weapon_complete_defs);
	utils::hook::inject(0x140206969 + 3, weapon_complete_defs);
	utils::hook::inject(0x140206994 + 3, weapon_complete_defs);
	utils::hook::inject(0x14020703B + 3, weapon_complete_defs);
	utils::hook::inject(0x14020775D + 3, weapon_complete_defs);
	utils::hook::inject(0x140213706 + 3, weapon_complete_defs);
	utils::hook::inject(0x140213725 + 3, weapon_complete_defs);
	utils::hook::inject(0x14021373C + 3, weapon_complete_defs);
	utils::hook::inject(0x140219764 + 3, weapon_complete_defs);
	utils::hook::inject(0x14021977B + 3, weapon_complete_defs);
	utils::hook::inject(0x140219A08 + 3, weapon_complete_defs);
	utils::hook::inject(0x14021ABA3 + 3, weapon_complete_defs);
	utils::hook::inject(0x14021B7B8 + 3, weapon_complete_defs);
	utils::hook::inject(0x14021BBBD + 3, weapon_complete_defs);
	utils::hook::inject(0x14021E45E + 3, weapon_complete_defs);
	utils::hook::inject(0x14022762D + 3, weapon_complete_defs);
	utils::hook::inject(0x1402282D0 + 3, weapon_complete_defs);
	utils::hook::inject(0x1403165EA + 3, weapon_complete_defs);
	utils::hook::inject(0x14032A7E5 + 3, weapon_complete_defs);
	utils::hook::inject(0x14032EB24 + 3, weapon_complete_defs);
	utils::hook::inject(0x1403382F4 + 3, weapon_complete_defs);
	utils::hook::inject(0x1403396BB + 3, weapon_complete_defs);
	utils::hook::inject(0x14033DAA7 + 3, weapon_complete_defs);
	utils::hook::inject(0x14033E005 + 3, weapon_complete_defs);
	utils::hook::inject(0x14033E7FD + 3, weapon_complete_defs);
	utils::hook::inject(0x14033F0E1 + 3, weapon_complete_defs);
	utils::hook::inject(0x14033F27D + 3, weapon_complete_defs);
	utils::hook::inject(0x1403405F1 + 3, weapon_complete_defs);
	utils::hook::inject(0x140340BDA + 3, weapon_complete_defs);
	utils::hook::inject(0x140340D53 + 3, weapon_complete_defs);
	utils::hook::inject(0x140340E3C + 3, weapon_complete_defs);
	utils::hook::inject(0x1403411CE + 3, weapon_complete_defs);
	utils::hook::inject(0x140341292 + 3, weapon_complete_defs);
	utils::hook::inject(0x1403413AE + 3, weapon_complete_defs);
	utils::hook::inject(0x14034170B + 3, weapon_complete_defs);
	utils::hook::inject(0x14034181C + 3, weapon_complete_defs);
	utils::hook::inject(0x1403419A3 + 3, weapon_complete_defs);
	utils::hook::inject(0x140341AE3 + 3, weapon_complete_defs);
	utils::hook::inject(0x140341C04 + 3, weapon_complete_defs);
	utils::hook::inject(0x140347843 + 3, weapon_complete_defs);
	utils::hook::inject(0x140347F5A + 3, weapon_complete_defs);
	utils::hook::inject(0x14034899B + 3, weapon_complete_defs);
	utils::hook::inject(0x140348C72 + 3, weapon_complete_defs);
	utils::hook::inject(0x140348DA0 + 3, weapon_complete_defs);
	utils::hook::inject(0x140349471 + 3, weapon_complete_defs);
	utils::hook::inject(0x140349F77 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034A84C + 3, weapon_complete_defs);
	utils::hook::inject(0x14034B3E8 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034B779 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034B9BE + 3, weapon_complete_defs);
	utils::hook::inject(0x14034BC39 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034C4E4 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034C674 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034CF32 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034D285 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034D797 + 3, weapon_complete_defs);
	utils::hook::inject(0x14034F322 + 3, weapon_complete_defs);
	utils::hook::inject(0x140351FD9 + 3, weapon_complete_defs);
	utils::hook::inject(0x1403582F9 + 3, weapon_complete_defs);
	utils::hook::inject(0x1403645DE + 3, weapon_complete_defs);
	utils::hook::inject(0x1403648BE + 3, weapon_complete_defs);
	utils::hook::inject(0x140364AFE + 3, weapon_complete_defs);
	utils::hook::inject(0x140367C84 + 3, weapon_complete_defs);
	utils::hook::inject(0x14036CAE7 + 3, weapon_complete_defs);
	utils::hook::inject(0x14037A0B0 + 3, weapon_complete_defs);
	utils::hook::inject(0x140389BE8 + 3, weapon_complete_defs);
	utils::hook::inject(0x14038AFDD + 3, weapon_complete_defs);
	utils::hook::inject(0x14038B475 + 3, weapon_complete_defs);
	utils::hook::inject(0x14038C384 + 3, weapon_complete_defs);
	utils::hook::inject(0x14038C7A7 + 3, weapon_complete_defs);
	utils::hook::inject(0x14038C96E + 3, weapon_complete_defs);
	utils::hook::inject(0x14038D44E + 3, weapon_complete_defs);
	utils::hook::inject(0x140397DC8 + 3, weapon_complete_defs);
	utils::hook::inject(0x140398703 + 3, weapon_complete_defs);
	utils::hook::inject(0x140399354 + 3, weapon_complete_defs);
	utils::hook::inject(0x140399548 + 3, weapon_complete_defs);
	utils::hook::inject(0x1403995E0 + 3, weapon_complete_defs);
	utils::hook::inject(0x140399C51 + 3, weapon_complete_defs);
	utils::hook::inject(0x14039A4B8 + 3, weapon_complete_defs);
	utils::hook::inject(0x14039B7F3 + 3, weapon_complete_defs);
	utils::hook::inject(0x14039C938 + 3, weapon_complete_defs);
	utils::hook::inject(0x14039CCD0 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047AFDB + 3, weapon_complete_defs);
	utils::hook::inject(0x14047BEED + 3, weapon_complete_defs);
	utils::hook::inject(0x14047BFF4 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047C1DB + 3, weapon_complete_defs);
	utils::hook::inject(0x14047CC90 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047CEBF + 3, weapon_complete_defs);
	utils::hook::inject(0x14047CFA1 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047D150 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047D200 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047D347 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047D3C7 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047D439 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047D57D + 3, weapon_complete_defs);
	utils::hook::inject(0x14047D900 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047DB05 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047E620 + 3, weapon_complete_defs);
	utils::hook::inject(0x14047E686 + 3, weapon_complete_defs);
	utils::hook::inject(0x140499190 + 3, weapon_complete_defs);
	utils::hook::inject(0x140499274 + 3, weapon_complete_defs);
	utils::hook::inject(0x14049B0AB + 3, weapon_complete_defs);
	utils::hook::inject(0x14049B166 + 3, weapon_complete_defs);
	utils::hook::inject(0x14049B1E4 + 3, weapon_complete_defs);
	utils::hook::inject(0x14049CE90 + 3, weapon_complete_defs);
	utils::hook::inject(0x140547C59 + 3, weapon_complete_defs);
	utils::hook::inject(0x14056D5D7 + 3, weapon_complete_defs);
	utils::hook::inject(0x14056D9C1 + 3, weapon_complete_defs);
	utils::hook::inject(0x14056DA73 + 3, weapon_complete_defs);

	utils::hook::inject(0x1401F7780 + 3, weapon_complete_defs);

	utils::hook::set<uint32_t>(0x1400A3C31 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400A3C4E + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400A3C65 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400A4CAA + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400A4CC7 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400A4CDE + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400C3403 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400C3ECC + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400CF412 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400D5146 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400D5182 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400D5AF8 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400D5B80 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400E6C8F + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400E6CAC + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400E6CE4 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400F725E + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1400F8BC9 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140181B2C + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140181B55 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1401E9DC8 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1401F7A84 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140200E57 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140200E72 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140200E89 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1402082A3 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1402201F2 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1402202CE + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140227B44 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14022C18A + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14022C1B3 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1402327EB + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14024CB45 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14024CB62 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14024CB81 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x1403259CF + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140325A17 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14033F8C1 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14033FB45 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034046B + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140346B7A + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140346B97 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140346BB8 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034A962 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034A97F + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034A9A1 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034E015 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034E032 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034E052 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034F59E + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034F5BB + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14034F5DB + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140391ADF + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140391B0F + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140391E0E + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140391E2C + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14047E3DB + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14047E3F7 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14047E411 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x140499D2F + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14049B39B + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14049B618 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14049B6CB + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14049B76E + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14049B7D3 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14049B837 + 4, RVA(&weapon_complete_defs));
	utils::hook::set<uint32_t>(0x14049B900 + 4, RVA(&weapon_complete_defs));

	utils::hook::set<uint32_t>(0x1400C477F + 4, RVA(&weapon_complete_defs));
}

void fastfiles::reallocate_attachment_pool()
{
	constexpr auto multiplier = 2;
	constexpr auto pool_size = get_pool_type_size(game::ASSET_TYPE_ATTACHMENT) * multiplier;
	reallocate_asset_pool<game::ASSET_TYPE_ATTACHMENT, pool_size>();

	static void* attachment_array[pool_size]{};
	static void* attachment_strings[pool_size]{};

	string_buffers.emplace_back(attachment_strings, pool_size * sizeof(void*));

	utils::hook::inject(0x1400C46B9 + 3, attachment_strings);
	utils::hook::inject(0x140357D37 + 3, attachment_strings);
	utils::hook::set<uint32_t>(0x1400B595A + 4, RVA(attachment_strings) - 0x2917A30);

	const auto cg_get_attachment_name_stub = [](utils::hook::assembler& a)
	{
		a.mov(rax, &attachment_array[0]);
		a.mov(r8d, pool_size);
		a.mov(rdx, rax);
		a.mov(ecx, game::ASSET_TYPE_ATTACHMENT);
		a.call(0x1404F51D0);
		a.jmp(0x1400C4693);
	};

	const auto loc_1185A0_stub = [](utils::hook::assembler& a)
	{
		a.mov(rax, &attachment_array[0]);
		a.push(rbx);
		a.imul(rbx, 8);
		a.mov(rcx, qword_ptr(rax, rbx));
		a.pop(rbx);
		a.cmp(qword_ptr(rcx, 8), 0);
		a.lea(rsi, qword_ptr(rcx, 8));
		a.jmp(0x1400C46CE);
	};

	utils::hook::jump(0x1400C467F, utils::hook::assemble(cg_get_attachment_name_stub), true);
	utils::hook::jump(0x1400C46C0, utils::hook::assemble(loc_1185A0_stub), true);
}

void fastfiles::reallocate_attachment_and_weapon()
{
	// weapon & attachment strings are reset here (we need to also reset the reallocated ones)
	utils::hook::call(0x14025618A, memset_stub);
	utils::hook::call(0x14025622C, memset_stub);
	utils::hook::call(0x1402562AC, memset_stub);

	reallocate_weapon_pool(); // TODO
	reallocate_attachment_pool();
}

void fastfiles::reallocate_sound_pool()
{
	constexpr auto original_pool_size = get_pool_type_size(game::ASSET_TYPE_SOUND);
	constexpr auto multiplier = 2;
	constexpr auto pool_size = original_pool_size * multiplier;

	const auto pool = reallocate_asset_pool<game::ASSET_TYPE_SOUND, pool_size>();
	utils::hook::inject(0x1402BBD3D + 3, reinterpret_cast<void*>(reinterpret_cast<size_t>(pool) + 8));

	static unsigned short net_const_string_sound_map[pool_size]{};
	utils::hook::inject(0x1401C954A + 3, net_const_string_sound_map);
	utils::hook::inject(0x1401C97B2 + 3, net_const_string_sound_map);
	utils::hook::inject(0x1401CA096 + 3, net_const_string_sound_map);
	utils::hook::inject(0x1401CA4F7 + 3, net_const_string_sound_map);
}

void fastfiles::reallocate_material_pool()
{
	constexpr auto pool_size = 16000;

	const auto pool = reallocate_asset_pool<game::ASSET_TYPE_MATERIAL, pool_size>();
	utils::hook::inject(0x1402BBB20 + 3, pool + 8);
	utils::hook::inject(0x1402BF42A + 3, pool + 8);
	utils::hook::inject(0x1402BBB6F + 3, pool + 8);
	utils::hook::inject(0x1402BBB02 + 3, pool + 8);

	utils::hook::set(0x1402899CA + 3, pool_size);

	utils::hook::set(0x14028B049 + 2, pool_size);
	utils::hook::set(0x14063EE7F + 2, pool_size);
	utils::hook::set(0x14063DAE0 + 2, pool_size);
	utils::hook::set(0x1405EAAA7 + 2, pool_size);
	utils::hook::set(0x1405EA7F2 + 2, pool_size);

	utils::hook::set(0x1405D97F5 + 1, pool_size);

	//utils::hook::set(0x1402C859E + 3, pool_size);

	utils::hook::set(0x1402C5EFA + 3, pool_size);

	utils::hook::set(0x1405D97F5 + 1, pool_size);
	utils::hook::set(0x1405D97F5 + 1, pool_size);

	static char g_stream_material[0x28][pool_size]{};

	utils::hook::set<uint32_t>(0x140289E21 + 4, RVA(g_stream_material));
	utils::hook::set<uint32_t>(0x14028A8FE + 4, RVA(g_stream_material));
	utils::hook::set<uint32_t>(0x14028B0D5 + 4, RVA(g_stream_material));
	utils::hook::set<uint32_t>(0x1402C5E2B + 4, RVA(g_stream_material));

	utils::hook::inject(0x1402C9371 + 3, g_stream_material);
	utils::hook::inject(0x140289814 + 3, g_stream_material);

	utils::hook::set<uint32_t>(0x1402C852A + 5, RVA(g_stream_material) + 0x08);
	utils::hook::set<uint32_t>(0x1402C8533 + 4, RVA(g_stream_material) + 0x18);

	utils::hook::set<uint32_t>(0x1402C905F + 4, RVA(g_stream_material) + 0x18);
	utils::hook::set<uint32_t>(0x1402C9067 + 5, RVA(g_stream_material) + 0x08);

	constexpr auto g_stream_ptr = 0x145124100;
	const auto replace_g_stream_offset = [](const size_t ptr, const void* arr, const int64_t off = 0)
	{
		const auto offset = reinterpret_cast<int64_t>(arr) - g_stream_ptr + off;
		utils::hook::set<int32_t>(ptr, static_cast<int32_t>(offset));
	};

	replace_g_stream_offset(0x1402CA39D + 4, g_stream_material, 0x18);
	replace_g_stream_offset(0x1402CA3E5 + 5, g_stream_material);
	replace_g_stream_offset(0x1402CA40C + 5, g_stream_material, 0x0A);
	replace_g_stream_offset(0x1402CA5F6 + 5, g_stream_material, 0x02);
	replace_g_stream_offset(0x1402CA648 + 5, g_stream_material, 0x0C);
	replace_g_stream_offset(0x1402CA700 + 6, g_stream_material, 0x20);
	replace_g_stream_offset(0x1402CA961 + 5, g_stream_material, 0x04);
	replace_g_stream_offset(0x1402CA9A1 + 5, g_stream_material, 0x0E);
	replace_g_stream_offset(0x1402CAA58 + 6, g_stream_material, 0x20);
	replace_g_stream_offset(0x1402CAC9E + 5, g_stream_material, 0x10);
	replace_g_stream_offset(0x1402CACB3 + 5, g_stream_material, 0x06);
	replace_g_stream_offset(0x1402CACC5 + 6, g_stream_material, 0x20);

	replace_g_stream_offset(0x140289F47 + 3, g_stream_material);
	replace_g_stream_offset(0x14028B50C + 3, g_stream_material);
	replace_g_stream_offset(0x14028B7CA + 3, g_stream_material);

	replace_g_stream_offset(0x1402C8D4E + 5, g_stream_material);
	replace_g_stream_offset(0x1402C8D5B + 5, g_stream_material);
	replace_g_stream_offset(0x1402C8D64 + 5, g_stream_material, 0x10);
	replace_g_stream_offset(0x1402C8D6D + 6, g_stream_material, 0x20);
	replace_g_stream_offset(0x1402C8D77 + 5, g_stream_material);
	replace_g_stream_offset(0x1402C8D80 + 5, g_stream_material, 0x10);
	replace_g_stream_offset(0x1402C8D89 + 5, g_stream_material, 0x10);
	replace_g_stream_offset(0x1402C8D92 + 6, g_stream_material, 0x20);
	replace_g_stream_offset(0x1402C8D9C + 6, g_stream_material, 0x20);
	replace_g_stream_offset(0x1402C8DA6 + 5, g_stream_material);
	replace_g_stream_offset(0x1402C8DAF + 5, g_stream_material, 0x10);
	replace_g_stream_offset(0x1402C8DB8 + 6, g_stream_material, 0x20);
	replace_g_stream_offset(0x1402C8DD6 + 5, g_stream_material);
	replace_g_stream_offset(0x1402C8DE3 + 5, g_stream_material);
	replace_g_stream_offset(0x1402C8DEC + 5, g_stream_material, 0x10);
	replace_g_stream_offset(0x1402C8DF5 + 5, g_stream_material, 0x10);
	replace_g_stream_offset(0x1402C8DFE + 6, g_stream_material, 0x20);
	replace_g_stream_offset(0x1402C8E08 + 6, g_stream_material, 0x20);

	replace_g_stream_offset(0x14028AFEE + 5, g_stream_material, 0x08);
	replace_g_stream_offset(0x14028AFFB + 4, g_stream_material, 0x18);

	// material stream
	{
		/*
		static uint32_t material_always_loaded[4][352 * 2]{};

		utils::hook::inject(0x14028AD2A + 3, material_always_loaded[0]);
		utils::hook::inject(0x14028AD3B + 3, material_always_loaded[1]);
		utils::hook::inject(0x14028AD4C + 3, material_always_loaded[2]);
		utils::hook::inject(0x14028AD5D + 3, material_always_loaded[3]);

		utils::hook::inject(0x1402C5DBA + 3, material_always_loaded[0]);
		utils::hook::inject(0x14028A2D7 + 3, material_always_loaded[0]);
		utils::hook::inject(0x140289BCD + 3, material_always_loaded[0]);

		utils::hook::inject(0x1402898A0 + 3, material_always_loaded[0]);
		utils::hook::nop(0x1402898A7, 6);

		utils::hook::set<uint32_t>(0x140289E65 + 4, RVA(material_always_loaded[0]));
		utils::hook::set<uint32_t>(0x1402C84C0 + 4, RVA(material_always_loaded[0]));

		replace_g_stream_offset(0x14028AA37 + 4, material_always_loaded[0]);
		replace_g_stream_offset(0x14028AC1C + 4, material_always_loaded[0]);
		replace_g_stream_offset(0x14028AC47 + 4, material_always_loaded[0]);
		replace_g_stream_offset(0x14028AD07 + 4, material_always_loaded[0]);
		replace_g_stream_offset(0x14028ADFE + 3, material_always_loaded[0]);
		replace_g_stream_offset(0x14028B31A + 3, material_always_loaded[0]);
		replace_g_stream_offset(0x14028B76C + 4, material_always_loaded[0]);
		replace_g_stream_offset(0x1402CA3D1 + 4, material_always_loaded[0]);

		replace_g_stream_offset(0x14028AE24 + 3, material_always_loaded[1]);
		replace_g_stream_offset(0x14028B33D + 3, material_always_loaded[1]);
		replace_g_stream_offset(0x1402CA5E8 + 4, material_always_loaded[1]);

		replace_g_stream_offset(0x14028AE4A + 3, material_always_loaded[2]);
		replace_g_stream_offset(0x14028B360 + 3, material_always_loaded[2]);
		replace_g_stream_offset(0x1402CA940 + 4, material_always_loaded[2]);

		replace_g_stream_offset(0x14028AE6D + 3, material_always_loaded[3]);
		replace_g_stream_offset(0x14028B388 + 2, material_always_loaded[3]);
		replace_g_stream_offset(0x1402CAC90 + 4, material_always_loaded[3]);
		*/

		static uint32_t stream_failed[352 * 2]{};

		utils::hook::inject(0x1402C627B + 3, stream_failed);

		utils::hook::set<uint32_t>(0x1402C7D1B + 4, RVA(stream_failed));
		utils::hook::set<uint32_t>(0x1402C858E + 4, RVA(stream_failed));

		static uint32_t material_touch[352 * 2]{};

		utils::hook::inject(0x1402CB532 + 3, material_touch);

		utils::hook::set<uint32_t>(0x1402C7CE9 + 4, RVA(material_touch));
		utils::hook::set<uint32_t>(0x1402C8FCC + 4, RVA(material_touch));
		utils::hook::set<uint32_t>(0x1402C94A9 + 4, RVA(material_touch));
		utils::hook::set<uint32_t>(0x1402CB607 + 4, RVA(material_touch));

		static float base_material_priority[pool_size]{};

		utils::hook::inject(0x1402CB52B + 3, base_material_priority);

		utils::hook::set<uint32_t>(0x1402C7CF1 + 4, RVA(base_material_priority));
		utils::hook::set<uint32_t>(0x1402C8FEC + 4, RVA(base_material_priority));
		utils::hook::set<uint32_t>(0x1402CB611 + 4, RVA(base_material_priority));
		utils::hook::set<uint32_t>(0x1402C94B1 + 6, RVA(base_material_priority));
		utils::hook::set<uint32_t>(0x1402C94BB + 6, RVA(base_material_priority));
		utils::hook::set<uint32_t>(0x1402C94C5 + 6, RVA(base_material_priority));

		static struct
		{
			float base;
			float middle;
		} default_material_priority[pool_size]{};

		utils::hook::set<uint32_t>(0x1402C8FD4 + 4, RVA(default_material_priority));
		utils::hook::set<uint32_t>(0x1402C8FE0 + 4, RVA(default_material_priority) + 0x4);

		replace_g_stream_offset(0x1402CA390 + 6, default_material_priority);
		replace_g_stream_offset(0x1402CA402 + 6, default_material_priority);
		replace_g_stream_offset(0x1402CA639 + 6, default_material_priority);

		replace_g_stream_offset(0x1402CA953 + 6, default_material_priority, 0x4);
		replace_g_stream_offset(0x1402CA990 + 6, default_material_priority, 0x4);
	}
}

void fastfiles::reallocate_material_bitsets()
{
	constexpr std::size_t old_set_size = 0x580;
	constexpr std::size_t new_set_size = 0x800;

	static std::uint32_t material_global[5][new_set_size / 4]{};
	static std::uint32_t material_snapshot[5][new_set_size / 4]{};
	static std::uint32_t material_flags[new_set_size / 4]{};

	struct region
	{
		std::uintptr_t old_start;
		std::size_t sets;
		std::uintptr_t new_start;
	};

	const region regions[] =
	{
		{0x1451E9D80, 5, reinterpret_cast<std::uintptr_t>(material_global)},
		{0x14320C850, 5, reinterpret_cast<std::uintptr_t>(material_snapshot)},
		{0x14320FB40, 1, reinterpret_cast<std::uintptr_t>(material_flags)},
	};

	const auto map = [&](const std::uintptr_t address)
	{
		for (const auto& r : regions)
		{
			if (address >= r.old_start && address < r.old_start + r.sets * old_set_size)
			{
				const auto offset = address - r.old_start;
				return r.new_start + (offset / old_set_size) * new_set_size + offset % old_set_size;
			}
		}

		return address;
	};

	const auto set_field = [](const std::uintptr_t address, const std::int64_t value)
	{
		assert(value == static_cast<std::int32_t>(value));
		utils::hook::set<std::int32_t>(address, static_cast<std::int32_t>(value));
	};

	for (const auto& site : material_bitset_sites::relocs)
	{
		const auto field = site.address + site.offset;
		const auto current = *reinterpret_cast<std::int32_t*>(field);

		auto origin = site.base;
		if (site.kind == image_pool_sites::base)
		{
			origin = map(site.base);
		}

		const auto target = map(site.base + current);
		set_field(field, static_cast<std::int64_t>(target) - static_cast<std::int64_t>(origin));
	}

	set_field(0x14028A5C6 + 6, static_cast<std::int64_t>(map(0x1451EB380)) - 0x10 - 0x145124100);

	for (const auto& site : material_bitset_sites::constants)
	{
		const auto field = site.address + site.offset;
		if (site.size == 1)
		{
			assert(*reinterpret_cast<std::uint8_t*>(field) == static_cast<std::uint8_t>(site.old_value));
			utils::hook::set<std::uint8_t>(field, static_cast<std::uint8_t>(site.new_value));
		}
		else
		{
			assert(*reinterpret_cast<std::int32_t*>(field) == site.old_value);
			utils::hook::set<std::int32_t>(field, site.new_value);
		}
	}

	static void* asset_list[0x8000]{};
	utils::hook::inject(0x14028A31C + 3, asset_list);
	utils::hook::inject(0x14028A65F + 3, asset_list);

	utils::hook::jump(0x1402899F0, append_xmodel_materials);
}

void fastfiles::widen_sorted_materials()
{
	constexpr std::size_t count = 0x4480; // 1.04 0x2980

	struct material_sort_info
	{
		float distance[count];
		char entries[count][20];
		char __pad0[0x80];
	};

	alignas(16) static std::uint16_t sorted_materials[count]{};
	alignas(16) static material_sort_info sort_info{};
	alignas(16) static std::uint32_t used_bits[0x200]{};
	alignas(16) static std::uint8_t sorted_flags[count]{};

	struct region
	{
		std::uintptr_t old_start;
		std::size_t old_size;
		std::uintptr_t new_start;
	};

	const region regions[] =
	{
		{0x14FD6AC00, 0x5300, reinterpret_cast<std::uintptr_t>(sorted_materials)},
		{0x14E06D700, 0xA600, reinterpret_cast<std::uintptr_t>(sort_info.distance)},
		{0x14E077D00, 0x33E00, reinterpret_cast<std::uintptr_t>(sort_info.entries)},
		{0x14FE70FF0, 0x400, reinterpret_cast<std::uintptr_t>(used_bits)},
		{0x1524CC380, 0x2980, reinterpret_cast<std::uintptr_t>(sorted_flags)},
	};

	const auto map = [&](const std::uintptr_t address)
	{
		for (const auto& r : regions)
		{
			if (address >= r.old_start && address < r.old_start + r.old_size)
			{
				return r.new_start + (address - r.old_start);
			}
		}

		return address;
	};

	for (const auto& site : sorted_material_sites::relocs)
	{
		const auto field = site.address + site.offset;
		const auto current = *reinterpret_cast<std::int32_t*>(field);
		const auto origin = site.kind == image_pool_sites::base ? map(site.base) : site.base;
		const auto value = static_cast<std::int64_t>(map(site.base + current)) - static_cast<std::int64_t>(origin);

		assert(value == static_cast<std::int32_t>(value));
		utils::hook::set<std::int32_t>(field, static_cast<std::int32_t>(value));
	}

	for (const auto& site : sorted_material_sites::constants)
	{
		const auto field = site.address + site.offset;
		switch (site.size)
		{
		case 1:
			assert(*reinterpret_cast<std::uint8_t*>(field) == static_cast<std::uint8_t>(site.old_value));
			utils::hook::set<std::uint8_t>(field, static_cast<std::uint8_t>(site.new_value));
			break;
		case 4:
			assert(*reinterpret_cast<std::uint32_t*>(field) == static_cast<std::uint32_t>(site.old_value));
			utils::hook::set<std::uint32_t>(field, static_cast<std::uint32_t>(site.new_value));
			break;
		default:
			assert(*reinterpret_cast<std::uint64_t*>(field) == site.old_value);
			utils::hook::set<std::uint64_t>(field, site.new_value);
			break;
		}
	}

	utils::hook::jump(0x140606E20, effect_surf_sort_key);

	// draw surf masks are built before we run
	utils::hook::set<std::uint64_t>(0x1420DBD30, *reinterpret_cast<std::uint64_t*>(0x1420DBD30) & ~(1ull << 63));
	for (const auto address : {0x1420DBD40ull, 0x1420DBD50ull, 0x1420DBD60ull})
	{
		utils::hook::set<std::uint64_t>(address, *reinterpret_cast<std::uint64_t*>(address) | 1ull << 63);
	}
}

std::uint64_t fastfiles::effect_surf_sort_key(void* surf, const std::uint32_t index)
{
	const auto key = **reinterpret_cast<std::uint64_t**>(reinterpret_cast<std::uintptr_t>(surf) + 8);

	auto high = (key & 0xFFFC000000) << 8;
	if ((key & 0x3FFFF00) == 0xFF00 && static_cast<std::uint8_t>(key) == 0x7F)
	{
		high |= 0x100000000;
	}

	const auto low = (high & 0x100000000) ? 0 : (key >> 8) & 0x3FFFF;
	const auto packed = (((index & 0x3F) | (low << 6)) << 2) | (key & 0xFFFFF00000000000);
	return (packed << 4) | high;
}

std::uint32_t fastfiles::append_xmodel_materials(void** list, const std::uint32_t count)
{
	std::uint32_t seen[0x800 / 4]{};
	std::uint32_t added = 0;

	for (auto i = 0u; i < count; ++i)
	{
		const auto model_index = game::mp::DB_GetXModelIndex(list[i]);
		const auto& entry = game::mp::db_xmodelMaterialLists[model_index];

		for (auto j = 0u; j < entry.count; ++j)
		{
			const auto material_index = static_cast<std::uint16_t>(entry.indices[j]);
			const auto material = game::mp::DB_GetMaterialAtIndex(material_index);
			const auto bit = 0x80000000u >> (material_index & 0x1F);
			auto& word = seen[material_index >> 5];

			if (!(word & bit))
			{
				word |= bit;
				list[count + added++] = material;
			}
		}
	}

	return added;
}

void fastfiles::reallocate_image_pool()
{
	constexpr auto pool_size = image_pool_size;

	const auto pool = reallocate_asset_pool<game::ASSET_TYPE_IMAGE, pool_size>();
	utils::hook::inject(0x1402BBAA2 + 3, pool + 8);
	utils::hook::inject(0x1402BBAC0 + 3, pool + 8);

	struct region
	{
		std::uintptr_t old_start;
		std::size_t old_size;
		std::size_t pre;
		std::uintptr_t new_start;
	};

	static char a123[(0x1780 + 0x2EE00) * 2 + 0x17700 * 4 + 0x40]{};
	static char a4[0x3A9800 * 2 + 0x40]{};
	static char a5[0x5DC00 * 2 + 0x40]{};
	static char b2[0xBB80 * 2 + 0x40]{};
	static char b3[0x20 + 0x1780 * 2 + 0x140]{};
	static char b4[0x119400 * 2 + 0x40]{};
	static char b5[0x600 * 2 + 0x40]{};
	static char b6[0x1770 * 2 + 0x140]{};
	static char b7[0x5E0 * 2 + 0x40]{};
	static char c1[0x1770 * 2 + 0x140]{};

	const auto place = [](char* buffer, const std::uintptr_t old_start, const std::size_t pre = 0)
	{
		const auto start = reinterpret_cast<std::uintptr_t>(buffer) + pre;
		return ((start + 0x3F) & ~std::uintptr_t(0x3F)) + (old_start & 0x3F);
	};

	const auto a1 = place(a123, 0x141C8B900);

	region regions[] =
	{
		{0x141C8B900, 0x1780, 0, a1},
		{0x141C8D080, 0x2EE00, 0, a1 + 0x1780 * 2},
		{0x141CBBE80, 0x17700, 0, a1 + (0x1780 + 0x2EE00) * 2},
		{0x141CD3688, 0x3A9800, 0, place(a4, 0x141CD3688)},
		{0x14207CEA0, 0x5DC00, 0, place(a5, 0x14207CEA0)},
		{0x1451EB900, 0xBB80, 0, place(b2, 0x1451EB900)},
		{0x145216E80, 0x1780, 0x20, place(b3, 0x145216E80, 0x20)},
		{0x145218600, 0x119400, 0, place(b4, 0x145218600)},
		{0x145331E00, 0x600, 0, place(b5, 0x145331E00)},
		{0x145339A80, 0x1770, 0, place(b6, 0x145339A80)},
		{0x14534C580, 0x5E0, 0, place(b7, 0x14534C580)},
		{0x14320E3D0, 0x1770, 0, place(c1, 0x14320E3D0)},
	};

	const auto map = [&](const std::uintptr_t address)
	{
		for (const auto& r : regions)
		{
			if (address >= r.old_start - r.pre && address < r.old_start + r.old_size)
			{
				return r.new_start + (address - r.old_start);
			}
		}

		return address;
	};

	const auto set_field = [](const std::uintptr_t address, const std::int64_t value)
	{
		assert(value == static_cast<std::int32_t>(value));
		utils::hook::set<std::int32_t>(address, static_cast<std::int32_t>(value));
	};

	for (const auto& site : image_pool_sites::relocs)
	{
		const auto field = site.address + site.offset;
		const auto current = *reinterpret_cast<std::int32_t*>(field);

		auto origin = site.base;
		if (site.kind == image_pool_sites::base)
		{
			origin = map(site.base);
		}

		const auto target = map(site.base + current);
		set_field(field, static_cast<std::int64_t>(target) - static_cast<std::int64_t>(origin));
	}

	set_field(0x14008B735 + 2, static_cast<std::int64_t>(map(0x14207CEA0)) - 0x40 - 0x141CD3680);
	set_field(0x14008C6B7 + 3, static_cast<std::int64_t>(map(0x141CD3688)) - 8 - (0x14008C6B7 + 7));
	set_field(0x1402C6406 + 4, static_cast<std::int64_t>(map(0x141CBBE80)) - static_cast<std::int64_t>(map(0x141C8B900)));

	static char stream_files[0x18 * pool_size * 4]{};
	utils::hook::inject(0x14028D726 + 3, stream_files); // sub_14028D720 (idk)
	utils::hook::inject(0x14028D8A8 + 3, stream_files); // DB_LoadXFile
	assert(*reinterpret_cast<std::int32_t*>(0x14028D890 + 1) == 0xBB80);
	utils::hook::set<std::int32_t>(0x14028D890 + 1, pool_size * 4);

	for (const auto& site : image_pool_sites::constants)
	{
		const auto field = site.address + site.offset;
		if (site.size == 1)
		{
			assert(*reinterpret_cast<std::uint8_t*>(field) == static_cast<std::uint8_t>(site.old_value));
			utils::hook::set<std::uint8_t>(field, static_cast<std::uint8_t>(site.new_value));
		}
		else
		{
			assert(*reinterpret_cast<std::int32_t*>(field) == site.old_value);
			utils::hook::set<std::int32_t>(field, site.new_value);
		}
	}

	stream_loaded_bits = reinterpret_cast<std::uint32_t*>(map(0x141C8B900));
	stream_priorities = reinterpret_cast<std::int32_t*>(map(0x141C8D080));
	stream_ids = reinterpret_cast<std::uint32_t*>(map(0x141CBBE80));
	stream_entries = reinterpret_cast<image_stream_entry*>(map(0x145218600));

	widen_stream_id_access(0x14008AD40, 8);
	widen_stream_id_access(0x14008BE80, 9);
	widen_stream_id_access(0x1402C5B47, 9);
	widen_stream_id_access(0x1402C5B77, 9);
	widen_stream_id_access(0x1402C5E8E, 9);
	widen_stream_id_access(0x1402C63C4, 8);
	widen_stream_id_access(0x1402C648E, 5);

	utils::hook::set<std::uint8_t>(0x1402C63F9 + 3, 0x82);
	utils::hook::set<std::uint8_t>(0x1402C6406 + 3, 0x85);
	utils::hook::call(0x1402C6417, sort_stream_ids);
	utils::hook::set<std::uint8_t>(0x1402C6474 + 3, 0xBC);
	utils::hook::set<std::uint8_t>(0x1402C6522 + 3, 4);

	utils::hook::jump(0x1402C8640, select_stream_reads);

	utils::hook::inject(0x1402C6E52 + 3, stream_read_list);
	utils::hook::set(0x1402C6E60, std::array<std::uint8_t, 3>{0x8B, 0x3E, 0x90});
	utils::hook::set<std::uint8_t>(0x1402C6E8B + 3, 4);
}

void fastfiles::widen_stream_id_access(const std::uintptr_t address, const std::size_t length)
{
	const auto code = reinterpret_cast<std::uint8_t*>(address);
	std::vector<std::uint8_t> bytes{code, code + length};

	auto i = 0u;
	if (bytes[i] == 0x66)
	{
		bytes.erase(bytes.begin());
	}

	if ((bytes[i] & 0xF0) == 0x40)
	{
		++i;
	}

	if (bytes[i] == 0x0F && bytes[i + 1] == 0xB7)
	{
		bytes.erase(bytes.begin() + i);
		bytes[i] = 0x8B;
	}

	assert(bytes[i] == 0x8B || bytes[i] == 0x89);

	const auto modrm = bytes[i + 1];
	if ((modrm & 7) == 4 && (modrm >> 6) != 3)
	{
		auto& sib = bytes[i + 2];
		if ((sib >> 6) == 1 && ((sib >> 3) & 7) != 4)
		{
			sib = static_cast<std::uint8_t>((sib & 0x3F) | 0x80);
		}
	}

	bytes.push_back(0x90);
	assert(bytes.size() == length);
	utils::hook::copy(address, bytes.data(), bytes.size());
}

void fastfiles::sort_stream_ids(std::uint32_t* begin, std::uint32_t* end, std::int64_t /*count*/, void* compare)
{
	const auto compare_fn = *static_cast<stream_compare_t*>(compare);
	std::stable_sort(begin, end, [&](const std::uint32_t a, const std::uint32_t b)
	{
		return compare_fn(a, b);
	});
}

void fastfiles::select_stream_reads(stream_select_context* context)
{
	const auto is_loaded = [](const std::uint32_t id)
	{
		return (stream_loaded_bits[id >> 5] & (0x80000000u >> (id & 0x1F))) != 0;
	};

	const auto get_entry = [](const std::uint32_t id) -> image_stream_entry&
	{
		return stream_entries[(id % image_pool_size) * 4 + id / image_pool_size];
	};

	const auto sort_ids = [](std::uint32_t* begin, std::uint32_t* end, const std::uintptr_t compare)
	{
		const auto compare_fn = reinterpret_cast<stream_compare_t>(compare);
		std::stable_sort(begin, end, [&](const std::uint32_t a, const std::uint32_t b)
		{
			return compare_fn(a, b);
		});
	};

	const auto stream = context->stream;
	const auto current_file = context->current_file;
	const auto position = context->position;
	auto urgent = context->urgent != 0;

	if (!urgent && optimization::throttle_streaming())
	{
		*reinterpret_cast<void**>(stream + 0x20EB08) = nullptr;
		return;
	}

	const auto count = *game::mp::streamImageCount;
	sort_ids(stream_ids, stream_ids + count, 0x1402C8C40);

	if (urgent)
	{
		auto split = 0u;
		while (split < count && stream_priorities[stream_ids[split]] < 0x2000)
		{
			++split;
		}

		*game::mp::streamSortFile = current_file;
		sort_ids(stream_ids, stream_ids + split, 0x1402C8B70);

		auto end = split;
		while (end < count && stream_priorities[stream_ids[end]] < 0x4000)
		{
			++end;
		}

		sort_ids(stream_ids + split, stream_ids + end, 0x1402C8B70);
	}

	void* selected_file = nullptr;
	std::uint64_t best_start = 0xFFFFFFFF;
	auto first_priority = 0x7F000;
	auto priority_limit = 0;

	for (auto i = 0u; i < count; ++i)
	{
		const auto id = stream_ids[i];
		if (is_loaded(id))
		{
			continue;
		}

		const auto& entry = get_entry(id);
		if (!entry.file)
		{
			continue;
		}

		const auto priority = stream_priorities[id];
		if (!selected_file)
		{
			first_priority = priority;
			priority_limit = priority > 0 ? priority + 0x1000 : priority;

			if (urgent)
			{
				if (priority > 0x4000)
				{
					urgent = false;
				}
				else
				{
					priority_limit = std::min(priority_limit, 0x4000);
				}
			}
		}

		if (priority > priority_limit)
		{
			break;
		}

		const auto start = entry.start;
		auto take = false;
		if (entry.file == current_file)
		{
			const auto distance = std::abs(static_cast<std::int64_t>(start - position));
			const auto best_distance = std::abs(static_cast<std::int64_t>(best_start - position));

			take = selected_file != current_file || distance < best_distance ||
				(start < position && best_start < position && best_start - start < 0x200000);
		}
		else
		{
			take = !selected_file || (selected_file == entry.file && best_start > start);
		}

		if (!take || (id >= image_pool_size && !is_loaded(id - image_pool_size)))
		{
			continue;
		}

		best_start = start;
		selected_file = entry.file;

		if (urgent)
		{
			break;
		}
	}

	*reinterpret_cast<void**>(stream + 0x20EB08) = selected_file;
	if (!selected_file)
	{
		return;
	}

	*reinterpret_cast<int*>(stream + 0x20EB14) = 0;

	auto limit = 0x4000;
	if (!urgent)
	{
		if (first_priority < 0)
		{
			limit = -1;
		}
		else if (first_priority == 0)
		{
			*reinterpret_cast<int*>(stream + 0x20EB18) = 0;
			limit = 0;
		}
		else
		{
			*reinterpret_cast<int*>(stream + 0x20EB18) = std::max(1, first_priority - 0x1000);
			limit = first_priority > 0x4000 ? first_priority + first_priority / 2 : std::max(priority_limit, 0x4000);
		}
	}

	static std::uint64_t starts[std::size(stream_read_list)]{};
	static std::uint32_t queued[std::size(stream_read_list)]{};
	static std::uint32_t order[std::size(stream_read_list)]{};

	auto added = 0u;
	for (auto i = 0u; i < count; ++i)
	{
		const auto id = stream_ids[i];
		if (is_loaded(id))
		{
			continue;
		}

		if (stream_priorities[id] > limit)
		{
			break;
		}

		const auto& entry = get_entry(id);
		if (!entry.file || entry.file != selected_file || entry.start - best_start >= 0x280000)
		{
			continue;
		}

		if (id >= image_pool_size && !is_loaded(id - image_pool_size) &&
			std::find(queued, queued + added, id - image_pool_size) == queued + added)
		{
			continue;
		}

		starts[added] = entry.start;
		queued[added] = id;
		order[added] = added;

		if (++added >= std::size(stream_read_list))
		{
			break;
		}
	}

	std::stable_sort(order, order + added, [](const std::uint32_t a, const std::uint32_t b)
	{
		return starts[a] < starts[b];
	});

	for (auto i = 0u; i < added; ++i)
	{
		stream_read_list[i] = queued[order[i]];
	}

	*reinterpret_cast<std::uint32_t*>(stream + 0x20EB04) = added;
}

void fastfiles::reallocate_customization()
{
	// packages (unk_140810CE8), 12 byte entries at 0x14135C1E0
	constexpr std::uint32_t limits[4] = {2, 128, 128, 128};
	constexpr auto entry_count = limits[0] + limits[1] + limits[2] + limits[3];
	static char entries[entry_count * 12]{};

	for (auto i = 0; i < 4; i++)
	{
		utils::hook::set<std::uint32_t>(0x140810CE8 + i * 4, limits[i]);
	}

	const auto base = reinterpret_cast<std::uintptr_t>(entries);
	const auto rva = [&](const std::uintptr_t offset)
	{
		return static_cast<std::int32_t>(base + offset - 0x140000000);
	};

	for (const auto address : {0x14004E34C, 0x14004E44C, 0x14004F18C, 0x14004F22A, 0x14004F92C})
	{
		utils::hook::set<std::int32_t>(address, rva(0));
	}

	for (const auto address : {0x14004E39E, 0x14004E48D, 0x14004F237, 0x14004F3BF})
	{
		utils::hook::set<std::int32_t>(address, rva(4));
	}

	utils::hook::set<std::int32_t>(0x14004D3DA, static_cast<std::int32_t>(base - 0x14004D3DE));

	// model name length checks, names are stored as pointers
	utils::hook::set<std::uint8_t>(0x14004EA7D, 0xEB); // sub_14004E9C0
	utils::hook::set<std::uint8_t>(0x14004F9C5, 0xEB); // sub_14004F990

	// models (dword_140810CF8), network index bits (unk_140810CD8), 0x70 byte entries at 0x14135C690
	constexpr std::uint32_t model_limits[4] = {1, 512, 4096, 512};
	constexpr std::uint32_t model_bits[4] = {1, 9, 12, 9};
	constexpr auto model_count = model_limits[0] + model_limits[1] + model_limits[2] + model_limits[3];
	constexpr std::uint32_t model_count_bits = 13;
	static_assert(model_count <= (1u << model_count_bits));
	static char models[model_count * 0x70]{};

	for (auto i = 0; i < 4; i++)
	{
		utils::hook::set<std::uint32_t>(0x140810CF8 + i * 4, model_limits[i]);
		utils::hook::set<std::uint32_t>(0x140810CD8 + i * 4, model_bits[i]);
	}

	utils::hook::set<std::uint32_t>(0x14004DCA1, model_count);
	utils::hook::set<std::uint32_t>(0x14041B8C4, model_count_bits);

	const auto models_base = reinterpret_cast<std::uintptr_t>(models);
	const auto lea_model = [&](const std::uintptr_t address, const std::uintptr_t offset)
	{
		utils::hook::set<std::int32_t>(address + 3, static_cast<std::int32_t>(models_base + offset - (address + 7)));
	};

	for (const auto address : {0x14004D066, 0x14004D11B, 0x14004DCF1, 0x14004DDAE, 0x14004DFA8, 0x14004E008,
		0x14004E09E, 0x14004E56B, 0x14004E5AA, 0x14004EA43, 0x14004EDC9, 0x14004F560})
	{
		lea_model(address, 0);
	}

	lea_model(0x14004DEBA, 0x50);
	lea_model(0x14004F62F, 0x32);

	const std::pair<std::uintptr_t, std::uintptr_t> model_rva_sites[] =
	{
		{0x14004DE2A, 0x0}, {0x14004E42C, 0x30}, {0x14004E546, 0x33}, {0x14004DEA2, 0x48},
		{0x14004DD7E, 0x58}, {0x14004DD54, 0x68}, {0x14004DE77, 0x68}, {0x14004DF74, 0x68},
	};

	for (const auto& [address, offset] : model_rva_sites)
	{
		utils::hook::set<std::int32_t>(address, static_cast<std::int32_t>(models_base + offset - 0x140000000));
	}
}

void fastfiles::reallocate_transient_files()
{
	// s_transientFiles (1.04 2048, 1.15 7144)
	const auto files_base = reinterpret_cast<std::uintptr_t>(transient_files);
	for (const auto address : {0x14005D6C3, 0x14005D730, 0x14005D81C, 0x14005D859, 0x14005DB00})
	{
		utils::hook::set<std::int32_t>(address + 3, static_cast<std::int32_t>(files_base - (address + 7)));
	}

	utils::hook::set<std::int32_t>(0x14005DD08 + 3, static_cast<std::int32_t>(files_base - 0x140000000));
	utils::hook::set<std::int32_t>(0x14005DD17 + 4, static_cast<std::int32_t>(files_base + 4 - 0x140000000));

	utils::hook::set<std::uint32_t>(0x14005D737 + 2, transient_file_count * transient_file_size);
	utils::hook::set<std::uint32_t>(0x14005DB07 + 2, transient_file_count * transient_file_size);
	utils::hook::set<std::uint32_t>(0x14005D83B + 2, transient_file_count);
}

void fastfiles::reallocate_transient_assets()
{
	// s_transientAssetEntries (1.04 4096, 1.15 9216)
	constexpr std::uint32_t asset_count = 0x4000;
	constexpr std::uint32_t asset_size = 0xC;
	static char assets[asset_count * asset_size]{};

	const auto assets_base = reinterpret_cast<std::uintptr_t>(assets);

	const std::pair<std::uintptr_t, std::uintptr_t> rip_sites[] =
	{
		{0x14005DF7D, 0}, {0x14005E39A, 0}, {0x14005E676, 9}, {0x14005EB36, 9}, {0x14005EB71, 0},
		{0x14005EC51, 9}, {0x14005EE51, 9}, {0x14005F236, 9}, {0x14005F473, 9}, {0x14005F59B, 0},
	};

	for (const auto& [address, offset] : rip_sites)
	{
		utils::hook::set<std::int32_t>(address + 3, static_cast<std::int32_t>(assets_base + offset - (address + 7)));
	}

	const std::pair<std::uintptr_t, std::uintptr_t> rva_sites[] =
	{
		{0x14005E0A7 + 4, 0}, {0x14005E0B1 + 5, 4}, {0x14005E147 + 4, 0}, {0x14005E151 + 5, 4},
		{0x14005E4A5 + 3, 0}, {0x14005E4BB + 5, 6}, {0x14005E737 + 4, 0}, {0x14005E748 + 5, 6},
		{0x14005E751 + 5, 4}, {0x14005E75A + 5, 8}, {0x14005E796 + 5, 4}, {0x14005E7B1 + 5, 4},
		{0x14005EA58 + 4, 0}, {0x14005EA62 + 5, 4},
	};

	for (const auto& [address, offset] : rva_sites)
	{
		utils::hook::set<std::int32_t>(address, static_cast<std::int32_t>(assets_base + offset - 0x140000000));
	}

	utils::hook::set<std::uint32_t>(0x14005E702 + 3, asset_count);

	// DB_ProcessTransientAssetList reads asset list max 0x30000 bytes (1.15 0x44C00)
	constexpr std::uint32_t asslist_size = 0x80000;
	utils::hook::set<std::uint32_t>(0x1402BEC16 + 1, asslist_size);
	utils::hook::set<std::uint32_t>(0x1402BEC32 + 2, asslist_size);

	// CL_TransientMem_RegisterPool (1.04 1mb 1.15 3mb)
	utils::hook::set<std::uint32_t>(0x14005DA76 + 2, 0x300000);
}

void fastfiles::reallocate_asset_pools()
{
	if (game::environment::is_sp())
	{
		reallocate_asset_pool<game::ASSET_TYPE_LOCALIZE_ENTRY, get_pool_type_size_sp(game::ASSET_TYPE_LOCALIZE_ENTRY) * 2>();
		return;
	}

	reallocate_attachment_and_weapon();
	reallocate_sound_pool();
	reallocate_material_pool();
	reallocate_material_bitsets();
	widen_sorted_materials();
	reallocate_image_pool();
	reallocate_customization();
	reallocate_transient_files();
	reallocate_transient_assets();
	reallocate_asset_pool_multiplier<game::ASSET_TYPE_XANIMPARTS, 2>();
	reallocate_asset_pool_multiplier<game::ASSET_TYPE_TTF, 2>();
	reallocate_asset_pool_multiplier<game::ASSET_TYPE_LOADED_SOUND, 2>();
	reallocate_asset_pool_multiplier<game::ASSET_TYPE_LOCALIZE_ENTRY, 2>();
}

bool fastfiles::exists(const std::string& zone, const bool ignore_usermap)
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

std::string fastfiles::get_current_fastfile()
{
	return current_fastfile.access<std::string>([&](std::string& fastfile)
	{
		return fastfile;
	});
}

void fastfiles::enum_assets(const game::XAssetType type,
	const std::function<void(game::XAssetHeader)>& callback, const bool include_override)
{
	game::DB_EnumXAssets_Internal(type, static_cast<void(*)(game::XAssetHeader, void*)>([](game::XAssetHeader header, void* data)
	{
		const auto& cb = *static_cast<const std::function<void(game::XAssetHeader)>*>(data);
		cb(header);
	}), &callback, include_override);
}

void fastfiles::enum_asset_entries(const game::XAssetType type, const std::function<void(game::XAssetEntry*)>& callback, bool include_override)
{
	constexpr auto max_asset_count = 130000;
	auto hash = &game::mp::db_hashTable[0];
	for (auto c = 0; c < max_asset_count; c++)
	{
		for (auto i = *hash; i; )
		{
			const auto entry = &game::mp::g_assetEntryPool[i];

			if (entry->asset.type == type)
			{
				callback(entry);

				if (include_override && entry->nextOverride)
				{
					auto next_ovveride = entry->nextOverride;
					while (next_ovveride)
					{
						const auto override = &game::mp::g_assetEntryPool[next_ovveride];
						callback(override);
						next_ovveride = override->nextOverride;
					}
				}
			}

			i = entry->nextHash;
		}

		++hash;
	}
}

void fastfiles::close_fastfile_handles()
{
	fastfile_handles.access([&](std::vector<HANDLE>& handles)
	{
		for (const auto& handle : handles)
		{
			CloseHandle(handle);
		}
	});
}

std::string fastfiles::get_zone_name(const unsigned int index)
{
	return get_zone_name_internal(index);
}

void fastfiles::set_usermap(const std::string& usermap)
{
	current_usermap.access([&](std::optional<std::string>& current_usermap_)
	{
		current_usermap_ = usermap;
	});
}

void fastfiles::clear_usermap()
{
	current_usermap.access([&](std::optional<std::string>& current_usermap_)
	{
		current_usermap_.reset();
	});
}

std::optional<std::string> fastfiles::get_current_usermap()
{
	return current_usermap.access<std::optional<std::string>>([&](
		std::optional<std::string>& current_usermap_)
	{
		return current_usermap_;
	});
}

bool fastfiles::usermap_exists(const std::string& name)
{
	if (is_stock_map(name))
	{
		return false;
	}

	return utils::io::file_exists(utils::string::va("usermaps\\%s\\%s.ff", name.data(), name.data()));
}

bool fastfiles::is_stock_map(const std::string& name)
{
	return exists(name, true);
}

REGISTER_COMPONENT(fastfiles)
