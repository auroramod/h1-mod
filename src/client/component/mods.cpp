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
	std::optional<std::string> mod_path;

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
			if (game::environment::is_mp())
			{
				// vid_restart works on multiplayer, but not on singleplayer
				command::execute("vid_restart");
				return;
			}

			auto mode = game::environment::is_mp() ? " -multiplayer "s : " -singleplayer "s;

			utils::nt::relaunch_self(mode.append(arg), true);
			utils::nt::terminate();
		}

		bool mod_requires_restart(const std::string& path)
		{
			return utils::io::file_exists(path + "/mod.ff") || utils::io::file_exists(path + "/zone/mod.ff");
		}

		void set_filesystem_data(const std::string& path)
		{
			if (mod_path.has_value())
			{
				filesystem::unregister_path(mod_path.value());
			}

			if (!game::environment::is_sp())
			{
				// modify fs_game on mp/dedi because its not set when we obviously vid_restart (sp does a full relaunch with command line arguments)
				game::Dvar_SetFromStringByNameFromSource("fs_game", path.data(),
					game::DVAR_SOURCE_INTERNAL);
			}
		}
	}

	void set_mod(const std::string& path)
	{
		set_filesystem_data(path);
		mod_path = path;
	}

	void clear_mod()
	{
		set_filesystem_data("");
		mod_path.reset();
	}

	std::optional<std::string> get_mod()
	{
		return mod_path;
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

				if (!game::Com_InFrontend() && (game::environment::is_mp() && !game::VirtualLobby_Loaded()))
				{
					console::info("Cannot load mod while in-game!\n");
					game::CG_GameMessage(0, "^1Cannot load mod while in-game!");
					return;
				}

				const auto path = params.get(1);
				if (!utils::io::directory_exists(path))
				{
					console::info("Mod %s not found!\n", path);
					return;
				}

				console::info("Loading mod %s\n", path);
				set_mod(path);

				if ((mod_path.has_value() && mod_requires_restart(mod_path.value())) ||
					mod_requires_restart(path))
				{
					console::info("Restarting...\n");
					full_restart("+set fs_game \""s + path + "\"");
				}
				else
				{
					restart();
				}
			});

			command::add("unloadmod", [](const command::params& params)
			{
				if (!mod_path.has_value())
				{
					console::info("No mod loaded\n");
					return;
				}

				if (!game::Com_InFrontend() && (game::environment::is_mp() && !game::VirtualLobby_Loaded()))
				{
					console::info("Cannot unload mod while in-game!\n");
					game::CG_GameMessage(0, "^1Cannot unload mod while in-game!");
					return;
				}

				console::info("Unloading mod %s\n", mod_path.value().data());

				if (mod_requires_restart(mod_path.value()))
				{
					console::info("Restarting...\n");
					clear_mod();
					full_restart("");
				}
				else
				{
					clear_mod();
					restart();
				}
			});

			command::add("com_restart", []()
			{
				if (!game::Com_InFrontend() && (game::environment::is_mp() && !game::VirtualLobby_Loaded()))
				{
					return;
				}

				restart();
			});
		}
	};
}

REGISTER_COMPONENT(mods::component)
