#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "gsc/script_extension.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace slowmotion
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			gsc::function::add("setslowmotion", [](const gsc::function_args& args)
			{
				if (args.size() == 0)
				{
					return scripting::script_value{};
				}

				const auto start = args[0].as<float>();
				const auto end = (args.size() > 0 ? args[1].as<float>() : 1.0f);
				const auto duration = (args.size() > 1 ? args[2].as<int>() : 1) * 1000;

				game::SV_SetConfigstring(10, utils::string::va("%i %i %g %g", *game::mp::gameTime, duration, start, end));
				game::Com_SetSlowMotion(start, end, duration);

				return scripting::script_value{};
			});
		}
	};
}

REGISTER_COMPONENT(slowmotion::component)
