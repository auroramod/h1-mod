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

		int progress_callback(size_t progress)
		{
			console::debug("Download progress: %lli\n", progress);
			if (download_aborted())
			{
				return -1;
			}

			return 0;
		}
	}

	void start_download(const game::netadr_s& target, const utils::info_string& info)
	{
		if (download_active())
		{
			scheduler::schedule([=]()
			{
				if (!download_active())
				{
					start_download(target, info);
					return scheduler::cond_end;
				}

				return scheduler::cond_continue;
			});

			return;
		}

		globals.access([&](globals_t& globals_)
		{
			globals_ = {};
		});

		const auto base = info.get("sv_wwwBaseUrl");
		if (base.empty())
		{
			party::menu_error("Download failed: Server doesn't have 'sv_wwwBaseUrl' dvar set.");
			return;
		}

		const auto mod = info.get("fs_game") + "/mod.ff";
		const auto url = base + "/" + mod;

		console::debug("Downloading %s from %s: %s\n", mod.data(), base.data(), url.data());

		scheduler::once([=]()
		{
			const ui_scripting::table mod_data_table{};
			mod_data_table.set("name", mod.data());

			ui_scripting::notify("mod_download_start",
			{
				{"request", mod_data_table}
			});
		}, scheduler::pipeline::lui);

		scheduler::once([=]()
		{
			{
				const auto _0 = gsl::finally(&mark_unactive);

				mark_active();

				if (download_aborted())
				{
					return;
				}

				const auto data = utils::http::get_data(url, {}, {}, &progress_callback);
				if (!data.has_value())
				{
					party::menu_error("Download failed: An unknown error occurred, please try again.");
					return;
				}

				if (download_aborted())
				{
					return;
				}

				const auto& result = data.value();
				if (result.code != CURLE_OK)
				{
					party::menu_error(utils::string::va("Download failed: %s (%i)\n", result.code, curl_easy_strerror(result.code)));
					return;
				}

				utils::io::write_file(mod, result.buffer, false);
			}

			scheduler::once([]()
			{
				ui_scripting::notify("mod_download_done", {});
			}, scheduler::pipeline::lui);

			// reconnect back to target after download of mod is finished
			scheduler::once([=]()
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

		scheduler::once([]()
		{
			ui_scripting::notify("mod_download_done", {});
		}, scheduler::pipeline::lui);

		party::menu_error("Download for server mod has been cancelled.");
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{

		}
	};
}

REGISTER_COMPONENT(download::component)
