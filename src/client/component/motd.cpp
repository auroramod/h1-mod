#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "motd.hpp"

#include "console.hpp"
#include "materials.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/string.hpp>

#define WEBSITE_DATA_URL "https://raw.githubusercontent.com/auroramod/website/publish/data"

namespace motd
{
	namespace
	{
		bool waiting = true;
		int index = 0;

		featured_content_t featured_content;

		void fetch_featured_content(const std::string& content_name, const int content_index)
		{
			const auto name = utils::string::va(content_name.data(), content_index);
			const auto url = utils::string::va(WEBSITE_DATA_URL "/%s", name);
			const auto result = utils::http::get_data(url, {}, {}, {});
			if (result.has_value())
			{
				featured_content.insert_or_assign(name, result.value());
			}
		}

		bool handle_featured_content()
		{
			if (index > 3)
			{
				return scheduler::cond_end;
			}

			scheduler::once([&]()
			{
				if (index > 3)
				{
					return;
				}

				const auto name = (index == 0 ? "motd.json" : "featured%d.json");
				fetch_featured_content(name, index);

				++index;
			}, scheduler::async);

			return scheduler::cond_continue;
		}
	}

	featured_content_t& get_featured_content()
	{
		return featured_content;
	}

	class component final : public component_interface
	{
	public:
		//std::optional<utils::http::result> motd_image_data;

		void post_load() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			//scheduler::once(download_motd_image, scheduler::async);
			//scheduler::schedule(setup_motd_image, scheduler::main);
			scheduler::schedule(handle_featured_content, scheduler::async);
		}
	};
}

REGISTER_COMPONENT(motd::component)
