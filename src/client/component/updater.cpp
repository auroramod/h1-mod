#include <std_include.hpp>
#include "updater.hpp"

#include "console.hpp"
#include "fastfiles.hpp"
#include "dvars.hpp"
#include "scheduler.hpp"
#include "version.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"
#include "game/ui_scripting/execution.hpp"

#include <utils/concurrency.hpp>
#include <utils/cryptography.hpp>
#include <utils/io.hpp>
#include <utils/nt.hpp>
#include <utils/properties.hpp>
#include <utils/string.hpp>

#define FILES_PATH "files.json"
#define FILES_PATH_DEV "files-dev.json"

#define DATA_PATH "data/"
#define DATA_PATH_DEV "data-dev/"

#define ERR_UPDATE_CHECK_FAIL "Failed to check for updates:\n%s"
#define ERR_UPDATE_CHECK_FAIL_BAD_RESPONSE "Bad response"
#define ERR_DOWNLOAD_FAIL "Failed to download file %s:\n%s"
#define ERR_WRITE_FAIL "Failed to write file "

#define BINARY_NAME "h1-mod.exe"

namespace
{
	struct status
	{
		bool done;
		bool success;
	};

	struct file_data
	{
		std::string name;
		std::string data;
	};

	struct update_data_t
	{
		bool restart_required{};
		bool cancelled{};
		status check{};
		status download{};
		std::string error{};
		std::string current_file{};
		std::vector<std::string> required_files{};
		std::vector<std::string> garbage_files{};
	};
}

static game::dvar_t* cl_auto_update;
static bool has_tried_update = false;

// remove this at some point
static std::vector<std::string> old_data_files =
{
	{"./data"},
	{"./cdata"},
};

static utils::concurrency::container<update_data_t> update_data;

void updater::post_unpack()
{
	delete_old_file();
	cl_auto_update = dvars::register_bool("cg_auto_update", true, game::DVAR_ARCHIVE, 
		"Automatically check for updates");
}

std::optional<utils::http::result> updater::get_server_file(const std::string& endpoint)
{
	static std::vector<std::string> server_urls =
	{
		{"https://h1-mod.auroramod.dev/"},
	};

	const auto try_url = [&](const std::string& base_url)
	{
		const auto url = base_url + endpoint;
		console::debug("[HTTP] GET file \"%s\"\n", url.data());
		const auto result = utils::http::get_data(url);
		return result;
	};

	for (const auto& url : server_urls)
	{
		const auto result = try_url(url);
		if (result.has_value())
		{
			return result;
		}
	}

	return {};
}

void updater::relaunch()
{
	const auto mode = game::environment::is_mp() ? "-multiplayer" : "-singleplayer";
	utils::nt::relaunch_self(mode);
	utils::nt::terminate();
}

void updater::set_has_tried_update(bool tried)
{
	has_tried_update = tried;
}

bool updater::get_has_tried_update()
{
	return has_tried_update;
}

bool updater::auto_updates_enabled()
{
#ifdef _DEBUG
	return false;
#else
	return cl_auto_update->current.enabled;
#endif
}

bool updater::is_update_check_done()
{
	return update_data.access<bool>([](update_data_t& data_)
	{
		return data_.check.done;
	});
}

bool updater::is_update_download_done()
{
	return update_data.access<bool>([](update_data_t& data_)
	{
		return data_.download.done;
	});
}

bool updater::get_update_check_status()
{
	return update_data.access<bool>([](update_data_t& data_)
	{
		return data_.check.success;
	});
}

bool updater::get_update_download_status()
{
	return update_data.access<bool>([](update_data_t& data_)
	{
		return data_.download.success;
	});
}

bool updater::is_update_available()
{
	return update_data.access<bool>([](update_data_t& data_)
	{
		return data_.required_files.size() > 0 || data_.garbage_files.size() > 0 || has_old_data_files();
	});
}

bool updater::is_restart_required()
{
	return update_data.access<bool>([](update_data_t& data_)
	{
		return data_.restart_required;
	});
}

std::string updater::get_last_error()
{
	return update_data.access<std::string>([](update_data_t& data_)
	{
		return data_.error;
	});
}

std::string updater::get_current_file()
{
	return update_data.access<std::string>([](update_data_t& data_)
	{
		return data_.current_file;
	});
}

void updater::cancel_update()
{
	console::debug("[Updater] Cancelling update\n");

	return update_data.access([](update_data_t& data_)
	{
		data_.cancelled = true;
	});
}

void updater::start_update_check()
{
	cancel_update();
	reset_data();

	console::debug("[Updater] starting update check\n");

	scheduler::once([]()
	{
		const auto files_data = download_file_list();

		if (is_update_cancelled())
		{
			reset_data();
			return;
		}

		if (!files_data.has_value())
		{
			set_update_check_status(true, false, utils::string::va(ERR_UPDATE_CHECK_FAIL, "Unknown error"));
			return;
		}

		const auto& value = files_data.value();
		if (value.code != CURLE_OK)
		{
			const auto error = curl_error(value.code);
			set_update_check_status(true, false, utils::string::va(ERR_UPDATE_CHECK_FAIL, error.data()));
			return;
		}

		rapidjson::Document j;
		j.Parse(value.buffer.data());

		if (!j.IsArray())
		{
			set_update_check_status(true, false, ERR_UPDATE_CHECK_FAIL_BAD_RESPONSE);
			return;
		}

		std::vector<std::string> required_files;
		std::vector<std::string> update_files;

		const auto files = j.GetArray();
		for (const auto& file : files)
		{
			if (!file.IsArray() || file.Size() != 3 || !file[0].IsString() || !file[2].IsString())
			{
				continue;
			}

			const auto name = file[0].GetString();
			const auto sha = file[2].GetString();

			update_files.push_back(name);

			if (!check_file(name, sha))
			{
				if (get_binary_name() == name)
				{
					update_data.access([](update_data_t& data_)
					{
						data_.restart_required = true;
					});
				}

				std::string name_ = name;
				if (name_.ends_with(".ff"))
				{
					update_data.access([](update_data_t& data_)
					{
						data_.restart_required = true;
					});
				}

				console::debug("[Updater] need file %s\n", name);

				required_files.push_back(name);
			}
		}

		const auto garbage_files = find_garbage_files(update_files);

		update_data.access([&](update_data_t& data_)
		{
			data_.check.done = true;
			data_.check.success = true;
			data_.required_files = required_files;
			data_.garbage_files = garbage_files;
		});

		notify("update_check_done");
	}, scheduler::pipeline::async);
}

void updater::start_update_download()
{
	console::debug("[Updater] starting update download\n");

	if (!is_update_check_done() || !get_update_check_status() || is_update_cancelled())
	{
		return;
	}

	delete_old_data_files();

	const auto garbage_files = update_data.access<std::vector<std::string>>([](update_data_t& data_)
	{
		return data_.garbage_files;
	});

	update_data.access([](update_data_t& data_)
	{
		if (data_.restart_required)
		{
			fastfiles::close_fastfile_handles();
		}
	});

	for (const auto& file : garbage_files)
	{
		try
		{
			std::filesystem::remove_all(file);
		}
		catch (...)
		{
			console::error("Failed to delete %s\n", file.data());
		}
	}

	scheduler::once([]()
	{
		const auto required_files = update_data.access<std::vector<std::string>>([](update_data_t& data_)
		{
			return data_.required_files;
		});

		std::vector<file_data> downloads;

		for (const auto& file : required_files)
		{
			update_data.access([file](update_data_t& data_)
			{
				data_.current_file = file;
			});

			console::debug("[Updater] downloading file %s\n", file.data());

			const auto data = download_data_file(file);

			if (is_update_cancelled())
			{
				reset_data();
				return;
			}

			if (!data.has_value())
			{
				set_update_download_status(true, false, utils::string::va(ERR_DOWNLOAD_FAIL, file.data(), "Unknown error"));
				return;
			}

			const auto& value = data.value();
			if (value.code != CURLE_OK)
			{
				const auto error = curl_error(value.code);
				set_update_download_status(true, false, utils::string::va(ERR_DOWNLOAD_FAIL, file.data(), error.data()));
				return;
			}

			downloads.push_back({file, value.buffer});
		}

		for (const auto& download : downloads)
		{
			if (!write_file(download.name, download.data))
			{
				set_update_download_status(true, false, ERR_WRITE_FAIL + download.name);
				return;
			}
		}

		set_update_download_status(true, true);
	}, scheduler::pipeline::async);
}

std::string updater::select(const std::string& main, const std::string& develop)
{
	if (GIT_BRANCH == "develop"s)
	{
		return develop;
	}

	return main;
}

std::string updater::load_binary_name()
{
	utils::nt::library self;
	return self.get_name();
}

std::string updater::get_binary_name()
{
	static const auto name = load_binary_name();
	return name;
}

void updater::notify(const std::string& name)
{
	scheduler::once([=]()
	{
		ui_scripting::notify(name, {});
	}, scheduler::pipeline::lui);
}

void updater::set_update_check_status(bool done, bool success, const std::string& error)
{
	update_data.access([done, success, error](update_data_t& data_)
	{
		data_.check.done = done;
		data_.check.success = success;
		data_.error = error;

		notify("update_check_done");
	});
}

void updater::set_update_download_status(bool done, bool success, const std::string& error)
{
	update_data.access([done, success, error](update_data_t& data_)
	{
		data_.download.done = done;
		data_.download.success = success;
		data_.error = error;
		notify("update_done");
	});
}

bool updater::check_file(const std::string& name, const std::string& sha)
{
	std::string data;

	if (get_binary_name() == name)
	{
		if (!utils::io::read_file(name, &data))
		{
			return false;
		}
	}
	else
	{
		const auto appdata_folder = utils::properties::get_appdata_path();
		const auto path = (appdata_folder / name).generic_string();
		if (!utils::io::read_file(path, &data))
		{
			return false;
		}
	}

	if (utils::cryptography::sha1::compute(data, true) != sha)
	{
		return false;
	}

	return true;
}

std::string updater::get_time_str()
{
	return utils::string::va("%i", uint32_t(time(nullptr)));
}

std::optional<utils::http::result> updater::download_data_file(const std::string& name)
{
	const auto file = std::format("{}{}?{}", select(DATA_PATH, DATA_PATH_DEV), name, get_time_str());
	return updater::get_server_file(file);
}

std::optional<utils::http::result> updater::download_file_list()
{
	const auto file = std::format("{}?{}", select(FILES_PATH, FILES_PATH_DEV), get_time_str());
	return updater::get_server_file(file);
}

bool updater::has_old_data_files()
{
	bool has = false;
	for (const auto& file : old_data_files)
	{
		if (utils::io::directory_exists(file))
		{
			has = true;
		}
	}

	return has;
}

void updater::delete_old_data_files()
{
	for (const auto& file : old_data_files)
	{
		std::filesystem::remove_all(file);
	}
}

bool updater::is_update_cancelled()
{
	return update_data.access<bool>([](update_data_t& data_)
	{
		return data_.cancelled;
	});
}

bool updater::write_file(const std::string& name, const std::string& data)
{
	if (get_binary_name() == name && 
		utils::io::file_exists(name) && 
		!utils::io::move_file(name, name + ".old"))
	{
		return false;
	}

	if (get_binary_name() == name)
	{
		return utils::io::write_file(name, data);
	}
	else
	{
		const auto appdata_folder = utils::properties::get_appdata_path();
		const auto path = (appdata_folder / name).generic_string();
		return utils::io::write_file(path, data);
	}
}

void updater::delete_old_file()
{
	utils::io::remove_file(get_binary_name() + ".old");
}

void updater::reset_data()
{
	update_data.access([](update_data_t& data_)
	{
		data_ = {};
	});
}

std::vector<std::string> updater::find_garbage_files(const std::vector<std::string>& update_files)
{
	std::vector<std::string> garbage_files{};

	const auto appdata_folder = utils::properties::get_appdata_path();
	const auto path = (appdata_folder / CLIENT_DATA_FOLDER).generic_string();
	if (!utils::io::directory_exists(path))
	{
		return {};
	}

	const auto current_files = utils::io::list_files_recursively(path);
	for (const auto& file : current_files)
	{
		bool found = false;
		for (const auto& update_file : update_files)
		{
			const auto update_file_ = (appdata_folder / update_file).generic_string();
			const auto path_a = std::filesystem::path(file);
			const auto path_b = std::filesystem::path(update_file_);
			const auto is_directory = utils::io::directory_exists(file);
			const auto compare = path_a.compare(path_b);

			if ((is_directory && compare == -1) || compare == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			console::debug("[Updater] Found extra file %s\n", file.data());
			if (file.ends_with(".ff"))
			{
				update_data.access([](update_data_t& data_)
				{
					data_.restart_required = true;
				});
			}

			garbage_files.push_back(file);
		}
	}

	return garbage_files;
}

std::string updater::get_mode_flag()
{
	if (game::environment::is_mp())
	{
		return "-multiplayer";
	}

	if (game::environment::is_sp())
	{
		return "-singleplayer";
	}

	return {};
}

std::string updater::curl_error(CURLcode code)
{
	const auto str_error = curl_easy_strerror(code);
	return utils::string::va("%s (%i)", str_error, code);
}

REGISTER_COMPONENT(updater)
