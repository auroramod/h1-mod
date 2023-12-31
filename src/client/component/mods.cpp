#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "dvars.hpp"
#include "filesystem.hpp"
#include "fonts.hpp"
#include "localized_strings.hpp"
#include "materials.hpp"
#include "mods.hpp"
#include "scheduler.hpp"
#include "game/demonware/services.hpp"

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
				command::execute("vid_restart");
				scheduler::once([]
				{
					mods::read_stats();
				}, scheduler::main);
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

		void set_filesystem_data(const std::string& path, bool change_fs_game)
		{
			if (mod_path.has_value())
			{
				filesystem::unregister_path(mod_path.value());
			}

			if (change_fs_game)
			{
				game::Dvar_SetFromStringByNameFromSource("fs_game", path.data(), game::DVAR_SOURCE_INTERNAL);
			}

			if (path != "")
			{
				filesystem::register_path(path);
			}
		}
	}

	void set_mod(const std::string& path, bool change_fs_game)
	{
		set_filesystem_data(path, change_fs_game);

		if (path != "")
		{
			mod_path = path;
		}
		else
		{
			mod_path.reset();
		}
	}

	std::optional<std::string> get_mod()
	{
		return mod_path;
	}

	void read_stats()
	{
		demonware::set_storage_path(mod_path.value_or(""));
		utils::hook::invoke<void>(0x4E6B60_b, 0); // read stats
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

			dvars::callback::on_new_value("fs_game", [](game::dvar_value* value)
			{
				console::warn("fs_game value changed to '%s'\n", value->string);
				set_mod(value->string, false);
			});

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
					full_restart("-mod \""s + path + "\"");
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
					set_mod("");
					full_restart("");
				}
				else
				{
					set_mod("");
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
