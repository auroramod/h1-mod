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
	std::recursive_mutex arena_mutex;

	bool parse_arena(const std::string& path)
	{
		std::lock_guard<std::recursive_mutex> _0(arena_mutex);

		std::string buffer{};
		if (filesystem::read_file(path, &buffer) && !buffer.empty())
		{
			*game::ui_num_arenas += game::GameInfo_ParseArenas(buffer.data(), MAX_ARENAS - *game::ui_num_arenas,
				&game::ui_arena_infos[*game::ui_num_arenas]);
			return true;
		}

		char rawfile_buffer[0x18000] = {0};
		const auto buf = game::DB_ReadRawFile(path.data(), rawfile_buffer, sizeof(rawfile_buffer));
		if (buf)
		{
			*game::ui_num_arenas += game::GameInfo_ParseArenas(buf, MAX_ARENAS - *game::ui_num_arenas, 
				&game::ui_arena_infos[*game::ui_num_arenas]);
			return true;
		}

		return false;
	}

	void load_arenas_stub()
	{
		*game::ui_num_arenas = 0;
		*game::ui_arena_buf_pos = 0;

		parse_arena("mp/basemaps.arena");

		// read usermap arena from disk
		const auto mapname = game::Dvar_FindVar("ui_mapname");
		if (mapname && mapname->current.string)
		{
			const auto usermap_path = "usermaps/"s + mapname->current.string;
			const auto arena_path = usermap_path + "/" + mapname->current.string + ".arena";
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

			// load custom arenas
			utils::hook::jump(0x4DE030_b, load_arenas_stub);
		}
	};
}

REGISTER_COMPONENT(arena::component)
