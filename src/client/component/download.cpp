#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "download.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "party.hpp"

#include "game/ui_scripting/execution.hpp"

#include <utils/concurrency.hpp>
#include <utils/http.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

namespace download
{
	namespace
	{
		struct globals_t
		{
			bool abort{};
			bool active{};
		};

		utils::concurrency::container<globals_t> globals;

		bool download_aborted()
		{
			return globals.access<bool>([](globals_t& globals_)
			{
				return globals_.abort;
			});
		}

		void mark_unactive()
		{
			globals.access([](globals_t& globals_)
			{
				globals_.active = false;
			});
		}

		void mark_active()
		{
			globals.access([](globals_t& globals_)
			{
				globals_.active = true;
			});
		}

		bool download_active()
		{
			return globals.access<bool>([](globals_t& globals_)
			{
				return globals_.active;
			});
		}

		auto last_update = std::chrono::high_resolution_clock::now();
		int progress_callback(size_t total, size_t progress)
		{
			const auto now = std::chrono::high_resolution_clock::now();
			if (now - last_update > 20ms)
			{
				last_update = std::chrono::high_resolution_clock::now();
				auto fraction = 0.f;
				if (total > 0)
				{
					fraction = static_cast<float>(static_cast<double>(progress) /
						static_cast<double>(total));
				}

				scheduler::once([=]
				{
					ui_scripting::notify("mod_download_progress",
					{
						{"fraction", fraction},
					});
				}, scheduler::pipeline::lui);
			}

			console::debug("Download progress: %lli/%lli\n", progress, total);
			if (download_aborted())
			{
				return -1;
			}

			return 0;
		}

		void menu_error(const std::string& error)
		{
			scheduler::once([=]
			{
				party::menu_error(error);
			}, scheduler::pipeline::lui);
		}
	}

	void start_download(const game::netadr_s& target, const utils::info_string& info, const std::vector<file_t>& files)
	{
		if (download_active())
		{
			scheduler::schedule([=]
			{
				if (!download_active())
				{
					start_download(target, info, files);
					return scheduler::cond_end;
				}

				return scheduler::cond_continue;
			}, scheduler::pipeline::main);

			return;
		}

		globals.access([&](globals_t& globals_)
		{
			globals_ = {};
		});

		const auto base = info.get("sv_wwwBaseUrl");
		if (base.empty())
		{
			menu_error("Download failed: Server doesn't have 'sv_wwwBaseUrl' dvar set.");
			return;
		}

		scheduler::once([]
		{
			ui_scripting::notify("mod_download_start", {});
		}, scheduler::pipeline::lui);

		scheduler::once([=]
		{
			{
				const auto _0 = gsl::finally(&mark_unactive);
				mark_active();

				if (download_aborted())
				{
					return;
				}

				for (const auto& file : files)
				{
					scheduler::once([=]
					{
						const ui_scripting::table data_table{};
						data_table.set("name", file.name.data());

						ui_scripting::notify("mod_download_set_file",
						{
							{"request", data_table}
						});
					}, scheduler::pipeline::lui);

					const auto url = utils::string::va("%s/%s", base.data(), file.name.data());
					console::debug("Downloading %s from %s: %s\n", file.name.data(), base.data(), url);
					const auto data = utils::http::get_data(url, {}, {}, &progress_callback);
					if (!data.has_value())
					{
						menu_error("Download failed: An unknown error occurred, please try again.");
						return;
					}

					if (download_aborted())
					{
						return;
					}

					const auto& result = data.value();
					if (result.code != CURLE_OK)
					{
						menu_error(utils::string::va("Download failed: %s (%i)\n", 
							curl_easy_strerror(result.code), result.code));
						return;
					}

					if (result.response_code >= 400)
					{
						menu_error(utils::string::va("Download failed: Server returned bad response code %i\n", 
							result.response_code));
						return;
					}

					const auto hash = utils::cryptography::sha1::compute(result.buffer, true);
					if (hash != file.hash)
					{
						menu_error(utils::string::va("Download failed: file hash doesn't match the server's (%s: %s != %s)\n", 
							file.name.data(), hash.data(), file.hash.data()));
						return;
					}

					utils::io::write_file(file.name, result.buffer, false);
				}
			}

			scheduler::once([]
			{
				ui_scripting::notify("mod_download_done", {});
			}, scheduler::pipeline::lui);

			scheduler::once([target]
			{
				party::connect(target);
			}, scheduler::pipeline::main);
		}, scheduler::pipeline::async);
	}

	void stop_download()
	{
		if (!download_active())
		{
			return;
		}

		globals.access([&](globals_t& globals_)
		{
			globals_.abort = true;
		});

		scheduler::once([]
		{
			ui_scripting::notify("mod_download_done", {});
			party::menu_error("Download for server mod has been cancelled.");
		}, scheduler::pipeline::lui);
	}
}
