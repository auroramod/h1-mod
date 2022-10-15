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
#include <utils/flags.hpp>
#include <utils/io.hpp>

namespace io
{
	namespace
	{
		bool use_root_folder = false;

		void check_path(const std::filesystem::path& path)
		{
			if (path.generic_string().find("..") != std::string::npos)
			{
				throw std::runtime_error("directory traversal is not allowed");
			}
		}

		std::string convert_path(const std::filesystem::path& path)
		{
			check_path(path);

			if (use_root_folder)
			{
				static const auto fs_base_game = game::Dvar_FindVar("fs_basepath");
				const std::filesystem::path fs_base_game_path(fs_base_game->current.string);
				return (fs_base_game_path / path).generic_string();
			}

			static const auto fs_game = game::Dvar_FindVar("fs_game");
			if (fs_game->current.string && fs_game->current.string != ""s)
			{
				const std::filesystem::path fs_game_path(fs_game->current.string);
				return (fs_game_path / path).generic_string();
			}

			throw std::runtime_error("fs_game is not properly defined");
		}

		void replace(std::string& str, const std::string& from, const std::string& to)
		{
			const auto start_pos = str.find(from);

			if (start_pos == std::string::npos)
			{
				return;
			}

			str.replace(start_pos, from.length(), to);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			use_root_folder = utils::flags::has_flag("io_game_dir");
			if (use_root_folder)
			{
				console::warn("GSC has access to your game folder. To prevent possible malicious code, remove the '-io_game_dir' launch flag.");
			}

			gsc::function::add("fileexists", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				return utils::io::file_exists(path);
			});

			gsc::function::add("writefile", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				const auto data = args[1].as<std::string>();

				auto append = false;
				if (args.size() > 2u)
				{
					append = args[2].as<bool>();
				}

				return utils::io::write_file(path, data, append);
			});

			gsc::function::add("readfile", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				return utils::io::read_file(path);
			});
			
			gsc::function::add("filesize", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				return static_cast<uint32_t>(utils::io::file_size(path));
			});

			gsc::function::add("createdirectory", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				return utils::io::create_directory(path);
			});

			gsc::function::add("directoryexists", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				return utils::io::directory_exists(path);
			});

			gsc::function::add("directoryisempty", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				return utils::io::directory_is_empty(path);
			});

			gsc::function::add("listfiles", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				const auto files = utils::io::list_files(path);

				scripting::array array{};
				for (const auto& file : files)
				{
					array.push(file);
				}

				return array;
			});

			gsc::function::add("copyfolder", [](const gsc::function_args& args)
			{
				const auto source = convert_path(args[0].as<std::string>());
				const auto target = convert_path(args[1].as<std::string>());
				utils::io::copy_folder(source, target);

				return scripting::script_value{};
			});

			gsc::function::add("removefile", [](const gsc::function_args& args)
			{
				const auto path = convert_path(args[0].as<std::string>());
				return utils::io::remove_file(path);
			});

			gsc::function::add("va", [](const gsc::function_args& args)
			{
				auto fmt = args[0].as<std::string>();

				for (auto i = 1u; i < args.size(); i++)
				{
					const auto arg = args[i].to_string();
					replace(fmt, "%s", arg);
				}

				return fmt;
			});
		}
	};
}

REGISTER_COMPONENT(io::component)
