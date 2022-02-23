#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"

#include <utils/hook.hpp>

namespace lui
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			// Don't show create cod account popup
			//utils::hook::set<uint32_t>(0x14017C957, 0); // H1(1.4)

//#ifdef _DEBUG
			// Enable development menus (causes issues in sp)
			//utils::hook::set<uint32_t>(SELECT_VALUE(0x1400B4ABC, 0x1401AB779), 1);
//#endif

			command::add("lui_open", [](const command::params& params)
			{
				if (params.size() <= 1)
				{
					console::info("usage: lui_open <name>\n");
					return;
				}

				game::LUI_OpenMenu(0, params[1], 0, 0, 0);
			});

			command::add("lui_open_popup", [](const command::params& params)
			{
				if (params.size() <= 1)
				{
					console::info("usage: lui_open_popup <name>\n");
					return;
				}

				game::LUI_OpenMenu(0, params[1], 1, 0, 0);
			});

			command::add("runMenuScript", [](const command::params& params)
			{
				const auto args_str = params.join(1);
				const auto* args = args_str.data();
				game::UI_RunMenuScript(0, &args);
			});
		}
	};
}

REGISTER_COMPONENT(lui::component)
