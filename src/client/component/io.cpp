#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "gsc.hpp"
#include "logfile.hpp"
#include "scheduler.hpp"

#include "gsc.hpp"

#include <utils/hook.hpp>
#include <utils/http.hpp>
#include <utils/io.hpp>

namespace io
{
	namespace
	{
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
			gsc::function::add("print", []()
			{
				const auto num = game::Scr_GetNumParam();
				std::string buffer{};

				for (auto i = 0; i < num; i++)
				{
					const auto str = game::Scr_GetString(i);
					buffer.append(str);
					buffer.append("\t");
				}

#ifdef DEBUG
				console::debug("[SCRIPT] %s\n", buffer.data());
#else
				console::info("[SCRIPT] %s\n", buffer.data());
#endif
			});

			gsc::function::add("assert", []()
			{
				const auto expr = gsc::get_argument(0).as<int>();
				if (!expr)
				{
					throw std::runtime_error("assert fail");
				}
			});

			gsc::function::add("assertex", []()
			{
				const auto expr = gsc::get_argument(0).as<int>();
				if (!expr)
				{
					const auto error = gsc::get_argument(1).as<std::string>();
					throw std::runtime_error(error);
				}
			});

			gsc::function::add("replacefunc", []()
			{
				const auto what = gsc::get_argument(0).get_raw();
				const auto with = gsc::get_argument(1).get_raw();

				if (what.type != game::SCRIPT_FUNCTION)
				{
					throw std::runtime_error("replaceFunc: parameter 1 must be a function");
					return;
				}

				if (with.type != game::SCRIPT_FUNCTION)
				{
					throw std::runtime_error("replaceFunc: parameter 2 must be a function");
					return;
				}

				logfile::set_gsc_hook(what.u.codePosValue, with.u.codePosValue);
			});

			gsc::function::add("toupper", []()
			{
				const auto string = gsc::get_argument(0).as<std::string>();
				game::Scr_AddString(utils::string::to_upper(string).data());
			});

			gsc::function::add("logprint", []()
			{
				std::string buffer{};

				const auto params = game::Scr_GetNumParam();
				for (auto i = 0; i < params; i++)
				{
					const auto string = game::Scr_GetString(i);
					buffer.append(string);
				}

				game::G_LogPrintf("%s", buffer.data());
			});

			gsc::function::add("va", []()
			{
				auto fmt = gsc::get_argument(0).as<std::string>();

				const auto params = game::Scr_GetNumParam();
				for (auto i = 1; i < params; i++)
				{
					const auto arg = gsc::get_argument(i).to_string();
					replace(fmt, "%s", arg);
				}

				game::Scr_AddString(fmt.data());
			});

			gsc::function::add("fileexists", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();

				game::Scr_AddBool(utils::io::file_exists(path));
			});

			gsc::function::add("writefile", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();
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
				const auto path = gsc::get_argument(0).as<std::string>();
				game::Scr_AddString(utils::io::read_file(path).data());
			});

			/*
			gsc::function::add("filesize", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();

				// return type is size_t, so we probably need to find a way to do that :P maybe just make it a string? lmao
				game::Scr_AddInt(utils::io::file_size(path));
			});
			*/

			gsc::function::add("createdirectory", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();
				game::Scr_AddBool(utils::io::create_directory(path));
			});

			gsc::function::add("directoryexists", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();
				game::Scr_AddBool(utils::io::directory_exists(path));
			});

			gsc::function::add("directoryisempty", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();
				game::Scr_AddBool(utils::io::directory_is_empty(path));
			});

			/*
			gsc::function::add("listfiles", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();
				const auto files = utils::io::list_files(path);

				scripting::array array{};
				for (const auto& file : files)
				{
					array.push(file);
				}

				return array;
			});
			*/

			gsc::function::add("copyfolder", []()
			{
				const auto source = gsc::get_argument(0).as<std::string>();
				const auto target = gsc::get_argument(1).as<std::string>();
				utils::io::copy_folder(source, target);
			});

			gsc::function::add("removefile", []()
			{
				const auto path = gsc::get_argument(0).as<std::string>();
				game::Scr_AddBool(utils::io::remove_file(path));
			});
		}
	};
}

REGISTER_COMPONENT(io::component)
