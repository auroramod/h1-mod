#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "filesystem.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>

#define MAX_ARENAS 64

namespace
{
	struct UISpawnPos
	{
		float allySpawnPos[2];
		float axisSpawnPos[2];
		float objectives[5][2];
	};

	struct mapInfo
	{
		char mapName[32];
		char mapLoadName[16];
		char mapDescription[32];
		char mapLoadImage[32];
		char mapCustomKey[32][16];
		char mapCustomValue[32][64];
		int mapCustomCount;
		char mapCamoTypes[2][16];
		int isAliensMap;
		int mapPack;
		int unk1;
		int gametype;
		char __pad0[132];
		UISpawnPos mapSpawnPos[32];
	};
	static_assert(sizeof(mapInfo) == 4648);

	int* ui_num_arenas;
	int* ui_arena_buf_pos;

	std::recursive_mutex _mutex;

	bool parse_arena(const std::string& path)
	{
		std::lock_guard<std::recursive_mutex> $(_mutex);

		char rawfile_buffer[0x18000];
		memset(rawfile_buffer, 0, sizeof(rawfile_buffer));

		// read from disk
		auto buffer = filesystem::read_file(path);
		if (buffer.size())
		{
			//console::debug("[DISK]: parsing arena \"%s\"...\n", path.data());

			// GameInfo_ParseArenas
			*ui_num_arenas += utils::hook::invoke<int>(0x4DE0B0_b, buffer.data(), MAX_ARENAS - *ui_num_arenas,
				reinterpret_cast<char*>(std::uintptr_t(0xAA55AE0_b) + (*ui_num_arenas * 8)));

			return true;
		}

		// read from fastfile
		// DB_ReadRawFile
		auto* buf = utils::hook::invoke<char*>(0x3994B0_b, path.data(), rawfile_buffer, sizeof(rawfile_buffer));
		if (buf)
		{
			//console::debug("[FF]: parsing arena \"%s\"...\n", path.data());

			// GameInfo_ParseArenas
			*ui_num_arenas += utils::hook::invoke<int>(0x4DE0B0_b, buf, MAX_ARENAS - *ui_num_arenas,
				reinterpret_cast<char*>(std::uintptr_t(0xAA55AE0_b) + (*ui_num_arenas * 8)));

			return true;
		}

		//console::debug("could not parse arena \"%s\"\n", path.data());
		return false;
	}

	void load_arenas_stub()
	{
		*ui_num_arenas = 0;
		*ui_arena_buf_pos = 0;

		// parse basemaps
		parse_arena("mp/basemaps.arena");

		// read usermap arena from disk
		auto mapname = game::Dvar_FindVar("ui_mapname");
		if (mapname && mapname->current.string)
		{
			auto usermap_path = "usermaps/"s + mapname->current.string;
			auto arena_path = usermap_path + "/" + mapname->current.string + ".arena";

			// try parsing arena
			parse_arena(arena_path);
		}
	}
}

namespace arena
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			// patch arena stuff
			ui_num_arenas = reinterpret_cast<int*>(0xAA55AD8_b);
			ui_arena_buf_pos = reinterpret_cast<int*>(0xAA55ADC_b);

			// load custom arenas
			utils::hook::jump(0x4DE030_b, load_arenas_stub);
		}
	};
}

REGISTER_COMPONENT(arena::component)