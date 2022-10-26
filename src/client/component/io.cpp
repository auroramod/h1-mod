#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "component/console.hpp"
#include "component/logfile.hpp"
#include "component/scheduler.hpp"
#include "component/gsc/script_extension.hpp"

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
		std::unordered_map<std::uint64_t, bool> active_requests{};
		std::uint64_t request_id{};

		bool allow_root_io = false;
		bool allow_http_access = false;

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

			if (allow_root_io)
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

		bool can_use_http()
		{
			if (game::environment::is_dedi() && allow_http_access)
			{
				return true;
			}

			return false;
		}

		scripting::script_value http_get(const gsc::function_args& args)
		{
			if (!can_use_http())
			{
				throw std::runtime_error("internet access is not allowed (only on dedicated server)");
			}

			const auto id = request_id++;
			active_requests[id] = true;

			const auto url = args[0].as<std::string>();
			const auto object = scripting::entity(scripting::make_object());

			scheduler::once([id, object, url]()
			{
				const auto data = utils::http::get_data(url.data());
				scheduler::once([id, object, data]()
				{
					if (active_requests.find(id) == active_requests.end())
					{
						return;
					}

					if (!data.has_value())
					{
						scripting::notify(object, "done", {{}, false, "Unknown error"});
					}

					const auto& result = data.value();
					const auto error = curl_easy_strerror(result.code);

					if (result.code != CURLE_OK)
					{
						scripting::notify(object, "done", {{}, false, error});
					}

					if (result.buffer.size() >= 0x5000)
					{
						console::warn("WARNING: http result size bigger than 20480 bytes (%i), truncating!", static_cast<int>(result.buffer.size()));
					}

					scripting::notify(object, "done", {result.buffer.substr(0, 0x5000), true});
				}, scheduler::pipeline::server);
			}, scheduler::pipeline::async);

			return object;
		}

		scripting::script_value http_post(const gsc::function_args& args)
		{
			if (!can_use_http())
			{
				throw std::runtime_error("internet access is not allowed (only on dedicated server)");
			}

			const auto id = request_id++;
			active_requests[id] = true;

			const auto url = args[0].as<std::string>();

			const auto object = scripting::entity(scripting::make_object());

			std::string fields_string{};
			std::unordered_map<std::string, std::string> headers_map{};

			if (args.size() > 1)
			{
				const auto options = args[1].as<scripting::array>();

				const auto fields = options["parameters"];
				const auto body = options["body"];
				const auto headers = options["headers"];

				if (fields.is<scripting::array>())
				{
					const auto fields_ = fields.as<scripting::array>();
					const auto keys = fields_.get_keys();

					for (const auto& key : keys)
					{
						if (!key.is<std::string>())
						{
							continue;
						}

						const auto key_ = key.as<std::string>();
						const auto value = fields_[key].to_string();
						fields_string += key_ + "=" + value + "&";
					}
				}

				if (body.is<std::string>())
				{
					fields_string = body.as<std::string>();
				}

				if (headers.is<scripting::array>())
				{
					const auto headers_ = headers.as<scripting::array>();
					const auto keys = headers_.get_keys();

					for (const auto& key : keys)
					{
						if (!key.is<std::string>())
						{
							continue;
						}

						const auto key_ = key.as<std::string>();
						const auto value = headers_[key].to_string();

						headers_map[key_] = value;
					}
				}
			}

			scheduler::once([id, object, url, fields_string, headers_map]()
			{
					const auto data = utils::http::get_data(url, fields_string, headers_map);
					scheduler::once([data, object, id]
					{
						if (active_requests.find(id) == active_requests.end())
						{
							return;
                        }

						if (!data.has_value())
						{
                            scripting::notify(object, "done", {{}, false, "Unknown error"});
							return;
						}

						const auto& result = data.value();
						const auto error = curl_easy_strerror(result.code);

						if (result.code != CURLE_OK)
						{
							scripting::notify(object, "done", {{}, false, error});
							return;
						}

						if (result.buffer.size() >= 0x5000)
						{
							console::warn("WARNING: http result size bigger than 20480 bytes (%i), truncating!", static_cast<int>(result.buffer.size()));
						}

						scripting::notify(object, "done", {result.buffer.substr(0, 0x5000), true});
				}, scheduler::pipeline::server);
			}, scheduler::pipeline::async);

			return object;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			allow_root_io = utils::flags::has_flag("allow_root_io");
			if (allow_root_io)
			{
				console::warn("GSC has access to your game folder. Remove the '-allow_root_io' launch parameter to disable this feature.");
			}

			allow_http_access = utils::flags::has_flag("allow_http");
			if (allow_http_access)
			{
				console::warn("GSC has access to external internet usage. Remove the '-allow_http' launch parameter to disable this feature.");
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

			gsc::function::add("httpget", http_get);
			gsc::function::add("curl", http_get);

			gsc::function::add("httppost", http_post);
			gsc::function::add("httprequest", http_post);
		}
	};
}

REGISTER_COMPONENT(io::component)
