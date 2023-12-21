#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "materials.hpp"

#include "game/game.hpp"

#include <utils/http.hpp>

namespace motd
{
	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			std::thread([]
			{
				//auto data = utils::http::get_data("https://h1.gg/data/motd.png");
				//if (data.has_value())
				//{
				//	materials::add("motd_image", data.value().buffer);
				//}
			}).detach();
		}

		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}
		}
	};
}

REGISTER_COMPONENT(motd::component)