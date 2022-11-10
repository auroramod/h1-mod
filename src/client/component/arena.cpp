#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "filesystem.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/memory.hpp>

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

		if (!game::DB_XAssetExists(game::ASSET_TYPE_RAWFILE, path.data()) || 
			game::DB_IsXAssetDefault(game::ASSET_TYPE_RAWFILE, path.data()))
		{
			return false;
		}
		
		const auto rawfile = game::DB_FindXAssetHeader(game::ASSET_TYPE_RAWFILE, path.data(), 0).rawfile;
		const auto len = game::DB_GetRawFileLen(rawfile);

		const auto rawfile_buffer = utils::memory::get_allocator()->allocate_array<char>(len);
		const auto _1 = gsl::finally([&]
		{
			utils::memory::get_allocator()->free(rawfile_buffer);
		});

		game::DB_GetRawBuffer(rawfile, rawfile_buffer, len);
		*game::ui_num_arenas += game::GameInfo_ParseArenas(rawfile_buffer, MAX_ARENAS - *game::ui_num_arenas,
			&game::ui_arena_infos[*game::ui_num_arenas]);
		return true;
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
