#include <std_include.hpp>

#include "console.hpp"
#include "filesystem.hpp"

#include "game/game.hpp"

#include "loader/component_loader.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>

namespace transient
{
	namespace
	{
		// hash is built in CL_CanUseTransientAsset (0x7503B)
		std::uint32_t hash_xmodel_name(const std::string& name) 
		{
			std::uint64_t hash = 0x7;
			for (auto character : name) 
			{
				hash = (hash * 0x1000193) ^ static_cast<std::int8_t>(character);
			}
			return static_cast<std::uint32_t>(hash);
		}

		utils::hook::detour process_transient_list_hook;
		void process_transient_list_stub(const char* asset_name, const int is_patch, const int is_dlc) 
		{
			// TODO: only loads h1_mod_post_gfx for now. maybe check if buffer is JSON?
			if (strcmp(asset_name, "transient/h1_mod_post_gfx.asslist"))
			{
				process_transient_list_hook.invoke<void>(asset_name, is_patch, is_dlc);
				return;
			}

			std::string asset_list_buffer;

#ifdef DEBUG
			if (filesystem::read_file(asset_name, &asset_list_buffer))
			{
				console::debug("reading \"%s\" from disk\n", asset_name);
			}
#endif

			// read transient asset json from zone (rawfile,h1_mod_post_gfx.asslist)
			if (asset_list_buffer.empty() && 
				(game::DB_XAssetExists(game::ASSET_TYPE_RAWFILE, asset_name) && !game::DB_IsXAssetDefault(game::ASSET_TYPE_RAWFILE, asset_name)))
			{
				console::debug("reading \"%s\" from fastfile\n", asset_name);
				const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_RAWFILE, asset_name, false);
				const auto len = game::DB_GetRawFileLen(asset.rawfile);
				asset_list_buffer.resize(len);
				game::DB_GetRawBuffer(asset.rawfile, asset_list_buffer.data(), len);
				if (len > 0)
				{
					asset_list_buffer.pop_back();
				}
			}

			rapidjson::Document document;
			document.Parse(asset_list_buffer.c_str());
			if (document.HasParseError()) 
			{
				console::error("Failed to parse JSON content for \"%s\"\n", asset_name);
				return;
			}

			if (!document.HasMember("pools") || !document["pools"].IsArray()) 
			{
				console::error("Failed to find 'pools', or it is not defined as a array in \"%s\"\n", asset_name);
				return;
			}

			const auto& pools = document["pools"];

#ifdef DEBUG
			console::debug("[%s] processing asset list with %d pools\n", asset_name, static_cast<int>(pools.Size()));
#endif

			auto pool_register_error = 0;

			for (rapidjson::SizeType i = 0; i < pools.Size(); ++i) 
			{
				const auto& pool = pools[i];

				if (!pool.HasMember("name") || !pool.HasMember("memoryAllocation") || !pool.HasMember("zones"))
				{
					console::error("bad structure in pool definition at index %d\n", i);
					continue;
				}

				auto pool_name = pool["name"].GetString();

#ifdef DEBUG
				console::debug("[%s] parsing pool '%s'\n", asset_name, pool_name);
#endif

				std::uint64_t memory_allocation = pool["memoryAllocation"].GetUint64();

				char pool_index;
				auto file_index_offset = 0;
				std::uint64_t zone_count = pool["zones"].Size();

				// if pool_register_error has a value, the pool was badly allocated
				pool_register_error |= game::CL_TransientMem_RegisterPool(
					pool_name,
					&zone_count,
					&memory_allocation,
					1u,
					&pool_index,
					&file_index_offset
				);

				const auto& zones = pool["zones"];
				for (rapidjson::SizeType j = 0; j < zones.Size(); ++j) 
				{
					const auto& zone = zones[j];
					if (!zone.HasMember("name") || !zone.HasMember("assets")) 
					{
						console::error("bad structure in zone json at index %d in pool '%s'\n", j, pool_name);
						continue;
					}

					auto zone_name = zone["name"].GetString();

#ifdef DEBUG
					console::debug("[%s] parsing zone '%s' in pool '%s'\n", asset_name, zone_name, pool_name);
#endif

					auto is_dlc_or_patch = (is_dlc || is_patch);

					const auto registered_file_index = (static_cast<int>(j) + file_index_offset);
					// registerfile is __int64, but var is uint16_t.. so maybe it's not __int64?
					std::int16_t zone_index = game::CL_TransientMem_RegisterFile(zone_name, pool_index, registered_file_index, is_dlc_or_patch);

					const auto& assets = zone["assets"];
					for (rapidjson::SizeType k = 0; k < assets.Size(); ++k) 
					{
						if (!assets[k].IsString()) 
						{
							console::error("bad asset definition at index %d in zone '%s'\n", k, zone_name);
							continue;
						}

						auto tr_asset_name = assets[k].GetString();
#ifdef DEBUG
						console::debug("[%s] registering asset '%s' in zone '%s' for pool '%s'\n", asset_name, tr_asset_name, zone_name, pool_name);
#endif
						std::uint32_t tr_asset_hash = hash_xmodel_name(tr_asset_name);
						game::CL_RegisterTransientAsset(tr_asset_hash, static_cast<std::uint32_t>(zone_index), is_dlc_or_patch);
					}
				}
			}

			game::DB_InitTransientModelTextures();
			*game::g_databaseReady = 1; // g_databaseReady
			game::DB_MarkUsedAssetsInternal(); // DB_MarkUsedAssetsInternal

			if (pool_register_error) 
			{
				game::Com_Error(game::errorParm::ERR_DROP, utils::string::va("Memory allocation exceeded while processing custom pools for %s", asset_name));
			}
		}

		void try_alloc_custom_pools_stub(const char* pool_name, const int is_dlc)
		{
			game::DB_TryAllocTransientPoolsForParent(pool_name, is_dlc); // virtualLobby
			game::DB_TryAllocTransientPoolsForParent("h1_mod_post_gfx", 0);
		}

		/*
		void print_pool_sizes(std::uint64_t tr_pool_size, std::uint64_t tr_pool_entry_size, const char* pool_name)
		{
			console::debug("[%s] tr_pool_size: %d, tr_pool_entry_size: %d", pool_name, tr_pool_size, tr_pool_entry_size);
		}

		void debug_budget_variables_stub(utils::hook::assembler& a)
		{
			a.and_(rcx, 0xFFFFFFFFFFFF0000);
			a.mov(r8d, 1);

			a.pushad64();
			a.lea(r8, ptr(rbx, 0)); // pool_name
			a.call_aligned(print_pool_sizes);
			a.popad64();

			a.jmp(0x74AB6_b);
		}
		*/
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			// load our custom transient pool (DB_TryAllocTransientPools, checks for "transient/<zone>.asslist" & "transient/patch_<zone>.asslist")
			// TODO: load pools from a rawfile or something?
			utils::hook::call(0x39A322_b, try_alloc_custom_pools_stub);

			// parse JSON transient lists after custom pool is loaded
			process_transient_list_hook.create(0x399170_b, process_transient_list_stub);

			// debugging memory budgets for TR pools
			//utils::hook::jump(0x74AA9_b, utils::hook::assemble(debug_budget_variables_stub), true);
		}
	};
 }

REGISTER_COMPONENT(transient::component);
