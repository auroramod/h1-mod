#include <std_include.hpp>
#include "engine.hpp"
#include "context.hpp"

#include "../../../component/ui_scripting.hpp"
#include "../../../component/game_module.hpp"

#include <utils/io.hpp>

namespace ui_scripting::lua::engine
{
	namespace
	{
		const auto lui_common = utils::nt::load_resource(LUI_COMMON);
		const auto lui_updater = utils::nt::load_resource(LUI_UPDATER);

		auto& get_scripts()
		{
			static std::vector<std::unique_ptr<context>> scripts{};
			return scripts;
		}

		void load_scripts(const std::string& script_dir)
		{
			if (!utils::io::directory_exists(script_dir))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir);

			for (const auto& script : scripts)
			{
				if (std::filesystem::is_directory(script) && utils::io::file_exists(script + "/__init__.lua"))
				{
					get_scripts().push_back(std::make_unique<context>(script, script_type::file));
				}
			}
		}

		void load_code(const std::string& code)
		{
			get_scripts().push_back(std::make_unique<context>(code, script_type::code));
		}
	}

	void start()
	{
		clear_converted_functions();
		get_scripts().clear();

		load_code(lui_common);
		load_code(lui_updater);

		load_scripts(game_module::get_host_module().get_folder() + "/data/ui_scripts/");
		load_scripts("h1-mod/ui_scripts/");
		load_scripts("data/ui_scripts/");
	}

	void stop()
	{
		clear_converted_functions();
		get_scripts().clear();
	}

	void run_frame()
	{
		for (auto& script : get_scripts())
		{
			script->run_frame();
		}
	}
}
