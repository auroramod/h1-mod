#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "gsc.hpp"
#include "logfile.hpp"
#include "scheduler.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include "game/scripting/execution.hpp"

#include <utils/hook.hpp>
#include <utils/http.hpp>
#include <utils/io.hpp>

namespace io
{
	namespace
	{
		void check_path(const std::filesystem::path& path)
		{
			if (path.generic_string().find("..") != std::string::npos)
			{
				throw std::runtime_error("io: directory traversal is not allowed");
			}
		}

		std::string convert_path(const std::filesystem::path& path)
		{
			check_path(path);
			static const auto fs_game = game::Dvar_FindVar("fs_game");

			if (fs_game->current.string && fs_game->current.string != ""s)
			{
				const std::filesystem::path fs_game_path(fs_game->current.string);
				return (fs_game_path / path).generic_string();
			}

			throw std::runtime_error("io: fs_game is not properly defined");
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gsc::function::add("fileexists", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				game::Scr_AddBool(utils::io::file_exists(path));
			});

			gsc::function::add("writefile", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				const auto data = gsc::get_argument(1).as<std::string>();
				const auto params = game::Scr_GetNumParam();

				auto append = false;
				if (params > 2)
				{
					append = gsc::get_argument(2).as<bool>();
				}

				game::Scr_AddBool(utils::io::write_file(path, data, append));
			});

			gsc::function::add("readfile", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				game::Scr_AddString(utils::io::read_file(path).data());
			});
			
			gsc::function::add("filesize", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				game::Scr_AddInt(static_cast<uint32_t>(utils::io::file_size(path)));
			});

			gsc::function::add("createdirectory", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				game::Scr_AddBool(utils::io::create_directory(path));
			});

			gsc::function::add("directoryexists", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				game::Scr_AddBool(utils::io::directory_exists(path));
			});

			gsc::function::add("directoryisempty", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				game::Scr_AddBool(utils::io::directory_is_empty(path));
			});

			gsc::function::add("listfiles", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				const auto files = utils::io::list_files(path);

				scripting::array array{};
				for (const auto& file : files)
				{
					array.push(file);
				}

				scripting::push_value(array);
			});

			gsc::function::add("copyfolder", []()
			{
				const auto source = convert_path(gsc::get_argument(0).as<std::string>());
				const auto target = convert_path(gsc::get_argument(1).as<std::string>());
				utils::io::copy_folder(source, target);
			});

			gsc::function::add("removefile", []()
			{
				const auto path = convert_path(gsc::get_argument(0).as<std::string>());
				game::Scr_AddBool(utils::io::remove_file(path));
			});
		}
	};
}

REGISTER_COMPONENT(io::component)
