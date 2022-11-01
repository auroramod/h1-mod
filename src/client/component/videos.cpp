#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace videos
{
	namespace
	{
		utils::hook::detour playvid_hook;

		std::unordered_map<std::string, std::string> video_replaces;

		void playvid(const char* name, const int a2, const int a3)
		{
			const auto vid = video_replaces.find(name);
			if (vid != video_replaces.end())
			{
				char path[256];
				game::Sys_BuildAbsPath(path, sizeof(path), game::SF_VIDEO, vid->second.data(), ".bik");

				if (game::Sys_FileExists(path))
				{
					name = vid->second.data();
				}
			}

			return playvid_hook.invoke<void>(name, a2, a3);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			playvid_hook.create(SELECT_VALUE(0x1404A9D00, 0x1405B0AF0), &playvid);

			if (game::environment::is_mp())
			{
				video_replaces["menus_bg_comp2"] = "menus_bg_h1mod";
				video_replaces["mp_menus_bg_options"] = "menus_bg_h1mod_blur";
			}
			else if (game::environment::is_sp())
			{
				video_replaces["sp_menus_bg_main_menu"] = "menus_bg_h1mod_sp";
				video_replaces["sp_menus_bg_campaign"] = "menus_bg_h1mod_sp";
				video_replaces["sp_menus_bg_options"] = "menus_bg_h1mod_sp";
			}
		}
	};
}

//REGISTER_COMPONENT(videos::component)
