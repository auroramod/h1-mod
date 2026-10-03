#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "component/console.hpp"
#include "component/fastfiles.hpp"
#include "component/filesystem.hpp"
#include "component/logfile.hpp"
#include "component/scripting.hpp"
#include "component/memory.hpp"

#include "game/dvars.hpp"

#include "game/scripting/array.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/function.hpp"

#include "script_extension.hpp"
#include "script_loading.hpp"

#include <utils/compression.hpp>
#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

std::unique_ptr<xsk::gsc::h1::context> script_loading::gsc_ctx = std::make_unique<xsk::gsc::h1::context>(xsk::gsc::instance::server);

static utils::hook::detour scr_begin_load_scripts_hook;
static utils::hook::detour scr_end_load_scripts_hook;

static std::unordered_map<std::string, std::uint32_t> main_handles;
static std::unordered_map<std::string, std::uint32_t> init_handles;

static utils::memory::allocator scriptfile_allocator;
static std::unordered_map<std::string, game::ScriptFile*> loaded_scripts;

static struct
{
	char* buf = nullptr;
	char* pos = nullptr;
	const unsigned int size = memory::custom_script_mem_size;
} script_memory;

static bool force_load = false;

void script_loading::post_unpack()
{
	// Load our scripts with an uncompressed stack
	utils::hook::call(SELECT_VALUE(0x1403C7280, 0x140441860), db_get_raw_buffer_stub);

	scr_begin_load_scripts_hook.create(SELECT_VALUE(0x1403BDB90, 0x140438060), scr_begin_load_scripts_stub);
	scr_end_load_scripts_hook.create(SELECT_VALUE(0x1403BDCC0, 0x140438190), scr_end_load_scripts_stub);

	// ProcessScript: hook xasset functions to return our own custom scripts
	utils::hook::call(SELECT_VALUE(0x1403C7217, 0x1404417F7), find_script);
	utils::hook::call(SELECT_VALUE(0x1403C7227, 0x140441807), db_is_x_asset_default);

	if (game::environment::is_sp())
	{
		// GScr_LoadScripts: initial loading of scripts
		utils::hook::call(0x1402BA152, load_gametype_script_stub);
	}
	else
	{
		// GScr_LoadScripts: reimplemented to use the 1.15 script paths (+ our custom scripts)
		utils::hook::call(0x140343651, gscr_load_scripts_stub);
	}

	// main is called from scripting.cpp
	// init is called from scripting.cpp

	scripting::on_shutdown([](bool free_scripts, bool post_shutdown)
	{
		if (free_scripts && post_shutdown)
		{
			clear();
		}
	});
}

void script_loading::pre_destroy()
{
	scr_begin_load_scripts_hook.clear();
	scr_end_load_scripts_hook.clear();
}

void script_loading::load_main_handles()
{
	for (auto& function_handle : main_handles)
	{
		console::info("Executing '%s::main'\n", function_handle.first.data());
		game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
	}
}

void script_loading::load_init_handles()
{
	for (auto& function_handle : init_handles)
	{
		console::info("Executing '%s::init'\n", function_handle.first.data());
		game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
	}
}

game::ScriptFile* script_loading::find_script(game::XAssetType type, const char* name, int allow_create_default)
{
	std::string real_name = name;
	const auto id = static_cast<std::uint16_t>(std::atoi(name));
	if (id)
	{
		real_name = gsc_ctx->token_name(id);
	}

	auto* script = load_custom_script(name, real_name);
	if (script)
	{
		return script;
	}

	return game::DB_FindXAssetHeader(type, name, allow_create_default).scriptfile;
}

char* script_loading::allocate_buffer(size_t size)
{
	if (script_memory.buf == nullptr)
	{
		script_memory.buf = game::PMem_AllocFromSource_NoDebug(script_memory.size, 4, 1, game::PMEM_SOURCE_SCRIPT);
		script_memory.pos = script_memory.buf;
	}

	if (script_memory.pos + size > script_memory.buf + script_memory.size)
	{
		game::Com_Error(game::ERR_FATAL, "Out of custom script memory");
	}

	const auto pos = script_memory.pos;
	script_memory.pos += size;
	return pos;
}

void script_loading::free_script_memory()
{
	game::PMem_PopFromSource_NoDebug(script_memory.buf, script_memory.size, 4, 1, game::PMEM_SOURCE_SCRIPT);
	script_memory.buf = nullptr;
	script_memory.pos = nullptr;
}

void script_loading::clear()
{
	main_handles.clear();
	init_handles.clear();
	loaded_scripts.clear();
	scriptfile_allocator.clear();
	script_extension::clear_devmap();
	free_script_memory();
}

bool script_loading::read_raw_script_file(const std::string& name, std::string* data)
{
	if (filesystem::read_file(name, data))
	{
		return true;
	}

	const auto* name_str = name.data();
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

game::ScriptFile* script_loading::load_custom_script(const char* file_name, const std::string& real_name)
{
	if (const auto itr = loaded_scripts.find(file_name); itr != loaded_scripts.end())
	{
		return itr->second;
	}

	if (game::VirtualLobby_Loaded() && !force_load)
	{
		return nullptr;
	}

	std::string source_buffer{};
	if (!read_raw_script_file(real_name + ".gsc", &source_buffer) || source_buffer.empty())
	{
		return nullptr;
	}

	// filter out "GSC rawfiles" that were used for development usage and are not meant for us.
	// each "GSC rawfile" has a ScriptFile counterpart to be used instead
	if (game::DB_XAssetExists(game::ASSET_TYPE_SCRIPTFILE, file_name) &&
		!game::DB_IsXAssetDefault(game::ASSET_TYPE_SCRIPTFILE, file_name))
	{
		if ((real_name.starts_with("maps/createfx") || real_name.starts_with("maps/createart") || real_name.starts_with("maps/mp"))
			&& (real_name.ends_with("_fx") || real_name.ends_with("_fog") || real_name.ends_with("_hdr")))
		{
			console::debug("Refusing to compile rawfile '%s'\n", real_name.data());
			return game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, file_name, false).scriptfile;
		}
	}

	console::info("Loading custom gsc '%s.gsc'", real_name.data());

	try
	{
		auto& compiler = gsc_ctx->compiler();
		auto& assembler = gsc_ctx->assembler();

		std::vector<std::uint8_t> data;
		data.assign(source_buffer.begin(), source_buffer.end());

		const auto assembly_ptr = compiler.compile(real_name, data);
		const auto output_script = assembler.assemble(*assembly_ptr);

		const auto bytecode = std::get<0>(output_script);
		const auto stack = std::get<1>(output_script);

		const auto script_file_ptr = static_cast<game::ScriptFile*>(scriptfile_allocator.allocate(sizeof(game::ScriptFile)));
		script_file_ptr->name = file_name;

		script_file_ptr->len = static_cast<int>(stack.size);
		script_file_ptr->bytecodeLen = static_cast<int>(bytecode.size);

		const auto stack_size = static_cast<std::uint32_t>(stack.size + 1);
		const auto byte_code_size = static_cast<std::uint32_t>(bytecode.size + 1);

		script_file_ptr->buffer = static_cast<char*>(scriptfile_allocator.allocate(stack_size));
		std::memcpy(const_cast<char*>(script_file_ptr->buffer), stack.data, stack.size);

		script_file_ptr->bytecode = allocate_buffer(byte_code_size);
		std::memcpy(script_file_ptr->bytecode, bytecode.data, bytecode.size);

		script_file_ptr->compressedLen = 0;

		loaded_scripts[file_name] = script_file_ptr;

		const auto devmap = std::get<2>(output_script);
		if (devmap.size > 0 && (gsc_ctx->build() & xsk::gsc::build::dev_maps) != xsk::gsc::build::prod)
		{
			script_extension::add_devmap_entry(reinterpret_cast<std::uint8_t*>(script_file_ptr->bytecode), byte_code_size, real_name, devmap);
		}

		console::info("Loaded custom gsc '%s.gsc'", real_name.data());

		return script_file_ptr;
	}
	catch (const std::exception& e)
	{
		console::error("*********** script compile error *************\n");
		console::error("failed to compile '%s':\n%s", real_name.data(), e.what());
		console::error("**********************************************\n");
		return nullptr;
	}
}

std::string script_loading::get_script_file_name(const std::string& name)
{
	const auto id = gsc_ctx->token_id(name);
	if (!id)
	{
		return name;
	}

	return std::to_string(id);
}

std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>> script_loading::read_compiled_script_file(const std::string& name, const std::string& real_name)
{
	const auto* script_file = game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, name.data(), false).scriptfile;
	if (script_file == nullptr)
	{
		throw std::runtime_error(std::format("Could not load scriptfile '{}'", real_name));
	}

	console::debug("Decompiling scriptfile '%s'\n", real_name.data());

	const auto len = script_file->compressedLen;
	const std::string stack{script_file->buffer, static_cast<std::uint32_t>(len)};

	const auto decompressed_stack = utils::compression::zlib::decompress(stack);

	std::vector<std::uint8_t> stack_data;
	stack_data.assign(decompressed_stack.begin(), decompressed_stack.end());

	return {{reinterpret_cast<std::uint8_t*>(script_file->bytecode), static_cast<std::uint32_t>(script_file->bytecodeLen)}, stack_data};
}

void script_loading::load_script(const std::string& name)
{
	if (!game::Scr_LoadScript(name.data()))
	{
		return;
	}

	const auto main_handle = game::Scr_GetFunctionHandle(name.data(), gsc_ctx->token_id("main"));
	const auto init_handle = game::Scr_GetFunctionHandle(name.data(), gsc_ctx->token_id("init"));

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

void script_loading::load_scripts(const std::filesystem::path& root_dir, const std::filesystem::path& subfolder)
{
	std::filesystem::path script_dir = root_dir / subfolder;
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

		load_script(base_name);
	}
}

void script_loading::load_custom_scripts()
{
	for (const auto& path : filesystem::get_search_paths())
	{
		if (game::environment::is_sp())
		{
			load_scripts(path, "scripts/sp/");
			load_scripts(path, "scripts/");
		}
		else
		{
			if (!game::VirtualLobby_Loaded())
			{
				load_scripts(path, "scripts/mp/");
				load_scripts(path, "scripts/");
			}

			force_load = true;
			const auto _0 = gsl::finally([&]
			{
				force_load = false;
			});
			load_scripts(path, "scripts/mp_patches/");
		}
	}
}

int script_loading::db_is_x_asset_default(game::XAssetType type, const char* name)
{
	if (loaded_scripts.contains(name))
	{
		return 0;
	}

	return game::DB_IsXAssetDefault(type, name);
}

void script_loading::load_gametype_script_stub(void* a1, void* a2)
{
	utils::hook::invoke<void>(0x1402B9DA0, a1, a2);
	load_custom_scripts();
}

void script_loading::db_get_raw_buffer_stub(const game::RawFile* rawfile, char* buf, const int size)
{
	if (rawfile->len > 0 && rawfile->compressedLen == 0)
	{
		std::memset(buf, 0, size);
		std::memcpy(buf, rawfile->buffer, std::min(rawfile->len, size));
		return;
	}

	game::DB_GetRawBuffer(rawfile, buf, size);
}

void script_loading::scr_begin_load_scripts_stub()
{
	// s1-mod reimplements this canonically, but for now, let all dev features be used in `developer_script 1`
	const auto* developer_script = script_extension::developer_script;
	const bool dev_script = developer_script ? developer_script->current.enabled : false;
	const auto build = dev_script ?
		xsk::gsc::build::dev :
		xsk::gsc::build::prod;

	gsc_ctx->init(build, []([[maybe_unused]] auto const* ctx, const auto& included_path) -> std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>>
	{
		const auto script_name = std::filesystem::path(included_path).replace_extension().string();

		std::string file_buffer;
		if (!read_raw_script_file(included_path, &file_buffer) || file_buffer.empty())
		{
			const auto name = get_script_file_name(script_name);
			if (game::DB_XAssetExists(game::ASSET_TYPE_SCRIPTFILE, name.data()))
			{
				return read_compiled_script_file(name, script_name);
			}

			throw std::runtime_error(std::format("Could not load gsc file '{}'", script_name));
		}

		std::vector<std::uint8_t> script_data;
		script_data.assign(file_buffer.begin(), file_buffer.end());

		return {{}, script_data};
	});

	scr_begin_load_scripts_hook.invoke<void>();
}

void script_loading::scr_end_load_scripts_stub()
{
	// cleanup the compiler
	gsc_ctx->cleanup();

	scr_end_load_scripts_hook.invoke<void>();
}

unsigned int script_loading::load_and_label_script(const char* filename, const char* label)
{
	if (!game::Scr_LoadScript(filename))
	{
		game::Com_Error(game::ERR_DROP, "Could not find script '%s'", filename);
	}

	const auto func = game::Scr_GetFunctionHandle(filename, gsc_ctx->token_id(label));
	if (!func)
	{
		game::Com_Error(game::ERR_DROP, "Could not label '%s' in script '%s'", label, filename);
	}

	return func;
}

void script_loading::gscr_load_scripts_stub()
{
	scr_begin_load_scripts_stub();

	game::mp::g_scr_data->delete_ = load_and_label_script("scripts/code/delete", "main");
	game::mp::g_scr_data->initstructs = load_and_label_script("scripts/code/struct", "initstructs");
	game::mp::g_scr_data->createstruct = load_and_label_script("scripts/code/struct", "createstruct");

	load_gametype_script();
	load_custom_scripts();
	load_level_script();

	if (game::mp::BG_BotFastFileEnabled())
	{
		load_bot_scripts();
	}

	if (game::mp::BG_AgentSystemEnabled())
	{
		load_agent_scripts();
	}

	game::mp::GScr_PostLoadScripts();
	scr_end_load_scripts_stub();
}

const char* script_loading::get_gametype()
{
	static auto* const g_gametype = game::Dvar_FindVar("g_gametype");

	const char* gametype = (g_gametype ? g_gametype->current.string : "");
	if (game::VirtualLobby_Loaded() || game::Com_InFrontend())
	{
		gametype = "vlobby";
	}

	return gametype;
}

void script_loading::load_gametype_script()
{
	char buffer[64]{};

	const auto gametype = get_gametype();
	sprintf_s(buffer, sizeof(buffer), "scripts/gametypes/%s", gametype);

	auto& data = *game::mp::g_scr_data;
	data.gametype.main = load_and_label_script(buffer, "main");
	data.gametype.startupgametype = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_startgametype");
	data.gametype.playerconnect = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_playerconnect");
	data.gametype.playerdisconnect = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_playerdisconnect");
	data.gametype.playerdamage = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_playerdamage");
	data.gametype.playerkilled = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_playerkilled");
	data.gametype.entityOutOfWorld = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_entityoutofworld");
	data.gametype.playerGrenadeSuicide = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_playergrenadesuicide");
	data.gametype.bulletHitEntity = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_bullethitentity");
	data.gametype.vehicleDamage = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_vehicledamage");
	data.gametype.entityDamage = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_entitydamage");
	data.gametype.codeendgame = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_codeendgame");
	data.gametype.playerlaststand = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_playerlaststand");
	data.gametype.playermigrated = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_playermigrated");
	data.gametype.hostmigration = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_hostmigration");
	data.partymembers = load_and_label_script("scripts/gametypes/_callbacksetup", "codecallback_partymembers");
}

void script_loading::load_level_script()
{
	if (const auto mapname = game::Dvar_FindVar("mapname"))
	{
		char level_script[64]{};

		constexpr int max_buffer_length = sizeof(level_script);
		if (const auto result = sprintf_s(level_script, max_buffer_length, "scripts/maps/%s/%s", mapname->current.string, mapname->current.string);
			result < max_buffer_length)
		{
			game::mp::g_scr_data->levelscript = load_and_label_script(level_script, "main");
		}
	}
}

void script_loading::load_bot_scripts()
{
	char buffer[64]{};

	const auto gametype = get_gametype();
	sprintf_s(buffer, sizeof(buffer), "scripts/mp/bots/_bots_gametype_%s", gametype);

	auto& data = *game::mp::g_scr_data;
	data.botGameTypeMain = load_and_label_script(buffer, "main");
	data.botMain = load_and_label_script("scripts/mp/bots/_bots", "main");
	data.leaderDialog = load_and_label_script("scripts/mp/bots/_bots", "codecallback_leaderdialog");
}

void script_loading::load_agent_scripts()
{
	char buffer[64]{};

	const auto gametype = get_gametype();
	sprintf_s(buffer, sizeof(buffer), "scripts/common/agents/_agents_gametype_%s", gametype);

	auto& data = *game::mp::g_scr_data;
	data.agentGameTypeMain = load_and_label_script(buffer, "main");
	data.agentMain = load_and_label_script("scripts/common/agents/_agents", "main");
	data.agentAdded = load_and_label_script("scripts/common/agents/_agent_common", "codecallback_agentadded");
	data.agentDamaged = load_and_label_script("scripts/common/agents/_agent_common", "codecallback_agentdamaged");
	data.agentKilled = load_and_label_script("scripts/common/agents/_agent_common", "codecallback_agentkilled");
	data.scriptedAgentOnEnterState = load_and_label_script("scripts/common/agents/scripted_agent_utility", "onenterstate");
	data.scriptedAgentOnDeactivate = load_and_label_script("scripts/common/agents/scripted_agent_utility", "ondeactivate");
}

REGISTER_COMPONENT(script_loading)
