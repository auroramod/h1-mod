#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "filesystem.hpp"
#include "fonts.hpp"
#include "localized_strings.hpp"
#include "materials.hpp"
#include "mods.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>

namespace mods
{
	std::string mod_path{};

	namespace
	{
		utils::hook::detour db_release_xassets_hook;
		bool release_assets = false;

		void db_release_xassets_stub()
		{
			if (release_assets)
			{
				materials::clear();
				fonts::clear();
			}

			localized_strings::clear();

			db_release_xassets_hook.invoke<void>();
		}

		void restart()
		{
			scheduler::once([]()
			{
				release_assets = true;
				const auto _0 = gsl::finally([]()
				{
					release_assets = false;
				});

				game::Com_Shutdown("");
			}, scheduler::pipeline::main);
		}

		void full_restart(const std::string& arg)
		{
			auto mode = game::environment::is_mp() ? " -multiplayer "s : " -singleplayer "s;

			utils::nt::relaunch_self(mode.append(arg), true);
			utils::nt::terminate();
		}
	}

	bool mod_requires_restart(const std::string& path)
	{
		return utils::io::file_exists(path + "/mod.ff") || utils::io::file_exists(path + "/zone/mod.ff");
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!utils::io::directory_exists("mods"))
			{
				utils::io::create_directory("mods");
			}

			db_release_xassets_hook.create(SELECT_VALUE(0x1F4DB0_b, 0x399740_b), db_release_xassets_stub);

			command::add("loadmod", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: loadmod mods/<modname>");
					return;
				}

				if (!game::Com_InFrontend())
				{
					console::info("Cannot load mod while in-game!\n");
					game::CG_GameMessage(0, "^1Cannot unload mod while in-game!");
					return;
				}

				const auto path = params.get(1);
				if (!utils::io::directory_exists(path))
				{
					console::info("Mod %s not found!\n", path);
					return;
				}

				console::info("Loading mod %s\n", path);

				if (mod_requires_restart(mod_path) || mod_requires_restart(path))
				{
					// vid_restart is still broken :(
					// TODO: above was fed's comment for H2, can we actually use it just fine?
					console::info("Restarting...\n");
					full_restart("+set fs_game \""s + path + "\"");
				}
				else
				{
					filesystem::unregister_path(mod_path);
					filesystem::register_path(path);
					mod_path = path;
					restart();
				}
			});

			command::add("unloadmod", [](const command::params& params)
			{
				if (mod_path.empty())
				{
					console::info("No mod loaded\n");
					return;
				}

				if (!game::Com_InFrontend())
				{
					console::info("Cannot unload mod while in-game!\n");
					game::CG_GameMessage(0, "^1Cannot unload mod while in-game!");
					return;
				}

				console::info("Unloading mod %s\n", mod_path.data());

				if (mod_requires_restart(mod_path))
				{
					console::info("Restarting...\n");
					full_restart("");
				}
				else
				{
					filesystem::unregister_path(mod_path);
					mod_path.clear();
					restart();
				}
			});

			command::add("com_restart", []()
			{
				if (!game::Com_InFrontend())
				{
					return;
				}

				restart();
			});
		}
	};
}

REGISTER_COMPONENT(mods::component)
