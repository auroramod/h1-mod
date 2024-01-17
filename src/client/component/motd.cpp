#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "materials.hpp"
#include "motd.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/string.hpp>

#define WEBSITE_DATA_URL "https://raw.githubusercontent.com/h1-mod/website/publish/data"

namespace motd
{
	namespace
	{
		bool waiting = true;
		int index = 0;

		featured_content_t featured_content;

		//std::optional<utils::http::result> motd_image_data;

		void get_featured_content(const std::string& content_name, const int content_index)
		{
			const auto name = utils::string::va(content_name.data(), content_index);
			const auto url = utils::string::va(WEBSITE_DATA_URL "/%s", name);
			const auto result = utils::http::get_data(url, {}, {}, {});
			if (result.has_value())
			{
				featured_content.insert_or_assign(name, result.value());
			}
		};

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
				get_featured_content(name, index);

				++index;
			}, scheduler::async);

			return scheduler::cond_continue;
		}

		/*
		void download_motd_image()
		{
			motd_image_data = utils::http::get_data(WEBSITE_DATA_URL "/motd.png", {}, {}, {});
			waiting = false;
		}

		bool setup_motd_image()
		{
			if (waiting)
			{
				return scheduler::cond_continue;
			}

			if (!motd_image_data.has_value())
			{
				printf("motd image doesn't have a value\n");
				return scheduler::cond_end;
			}

			printf("motd image loading...");

			const auto material = materials::create_material("motd_image");
			try
			{
				if (!materials::setup_material_image(material, motd_image_data.value().buffer))
				{
					materials::free_material(material);
				}

				printf("motd image loaded");
			}
			catch (const std::exception& e)
			{
				materials::free_material(material);
				console::error("Failed to load MOTD image: %s\n", e.what());
			}

			return scheduler::cond_end;
		}
		*/
	}

	featured_content_t& get_featured_content()
	{
		return featured_content;
	}

	class component final : public component_interface
	{
	public:
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
