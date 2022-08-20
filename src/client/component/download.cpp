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
			bool is_downloading{};
			bool download_successful{};
			size_t num_files{};
			std::string current_file{};
			size_t current_progress{};
			bool abort{};
			bool active{};
		};

		utils::concurrency::container<globals_t> globals;
		void set_download_status(bool downloading, bool download_status)
		{
			globals.access([&](globals_t& globals_)
			{
				globals_ = {};
				globals_.is_downloading = downloading;
				globals_.download_successful = download_status;
			});
		}

		bool download_aborted()
		{
			return globals.access<bool>([](globals_t& globals_)
			{
				return globals_.abort;
			});
		}

		void mark_finished()
		{
			globals.access([](globals_t& globals_)
			{
				globals_.active = false;
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
			console::debug("[Download] Progress %lli\n", progress);
			if (download_aborted())
			{
				return -1;
			}

			return 0;
		}

		void set_current_file(const std::string& name)
		{
			globals.access([&](globals_t& globals_)
			{
				globals_.current_file = name;
			});
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

		set_download_status(true, false);

		const auto base = info.get("sv_wwwBaseUrl");
		if (base.empty())
		{
			console::debug("[Download] Download failed: server doesn't have 'sv_wwwBaseUrl' set\n");
			set_download_status(true, false);
			return;
		}

		const auto mod = info.get("fs_game") + "/mod.ff";
		const auto url = base + "/" + mod;

		console::debug("[Download] Downloading %s from %s: %s\n", mod.data(), base.data(), url.data());

		scheduler::once([=]()
		{
			{
				const auto _0 = gsl::finally(&mark_finished);

				if (download_aborted())
				{
					return;
				}

				const auto data = utils::http::get_data(url, {}, {}, &progress_callback);
				if (!data.has_value())
				{
					console::debug("[Download] Download failed with unknown error\n");
					set_download_status(true, false);
					return;
				}

				if (download_aborted())
				{
					return;
				}

				const auto& result = data.value();
				if (result.code != CURLE_OK)
				{
					console::debug("[Download] Download failed with error %i %s\n", result.code, curl_easy_strerror(result.code));
					set_download_status(true, false);
					return;
				}

				utils::io::write_file(mod, result.buffer, false);
			}

			ui_scripting::notify("mod_download_done", {});

			scheduler::once([=]()
			{
				party::connect(target);
			}, scheduler::pipeline::main);
		}, scheduler::pipeline::async);
	}

	bool is_downloading_mod()
	{
		return globals.access<bool>([](globals_t& globals_)
		{
			return globals_.is_downloading;
		});
	}

	bool is_download_successful()
	{
		return globals.access<bool>([](globals_t& globals_)
		{
			return globals_.download_successful;
		});
	}

	void stop_download()
	{
		if (!download_active())
		{
			return;
		}

		console::debug("[Download] Download aborted\n");
		globals.access([&](globals_t& globals_)
		{
			globals_.abort = true;
		});
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
