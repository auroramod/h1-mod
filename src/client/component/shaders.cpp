#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"
#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/nt.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace shaders
{
	namespace
	{
		game::dvar_t* disable_shader_caching = nullptr;

		bool shader_should_show_dialog_stub()
		{
			return !disable_shader_caching->current.enabled;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			const auto has_flag = utils::flags::has_flag("noshadercaching");

			disable_shader_caching = dvars::register_bool("disable_shader_caching", has_flag, 
				game::DVAR_FLAG_SAVED, "Disable shader caching");
			if (has_flag)
			{
				dvars::override::set_bool("disable_shader_caching", 1);
				dvars::override::set_from_string("disable_shader_caching", "1");
			}

			utils::hook::jump(0x14007E710, shader_should_show_dialog_stub);
		}
	};
}

//REGISTER_COMPONENT(shaders::component)
