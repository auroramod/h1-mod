#include <std_include.hpp>

#ifdef _DEBUG

#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/scripting.hpp"
#include "gui.hpp"

enum ConfigString : __int32
{
	CS_FIRST = 0x0,
	MAX_CONFIGSTRINGS = 5617,
};

namespace
{
	static std::vector<std::string> g_cs_strings;
	static int g_cs_count = 5617;

	void refresh_configstrings()
	{
		g_cs_strings.assign(g_cs_count, "(null)");

		if (!game::SV_Loaded())
		{
			return;
		}

		char buffer[1024]{};
		for (int i = 0; i < g_cs_count; ++i)
		{
			buffer[0] = '\0';
			game::SV_GetConfigstring(i, buffer, sizeof(buffer));
			g_cs_strings[i] = (buffer[0] != '\0') ? buffer : "(null)";
		}
	}

	void render_window()
	{
		static auto* enabled = &gui::enabled_menus["conststrings"];

		ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(1000, 1000));
		if (!ImGui::Begin("ConstStrings List", enabled))
		{
			ImGui::End();
			return;
		}

		ImGui::TextUnformatted("This is a read-only view of the server config strings.");
		ImGui::TextUnformatted("Useful for debugging custom gametypes/mods.");
		ImGui::Separator();

		// --- Refresh button ---
		if (ImGui::Button("Refresh"))
		{
			refresh_configstrings();
		}

		ImGui::Text("Config Strings (%d):", g_cs_count);

		ImGui::BeginChild("##cs_scroll", ImVec2(0, 0));
		ImGuiListClipper clipper;
		clipper.Begin(static_cast<int>(g_cs_strings.size()));
		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				ImGui::Text("%04d: %s", i, g_cs_strings[i].c_str());
		}
		ImGui::EndChild();

		ImGui::End();
	}
}

namespace conststrings
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
				return;

			gui::register_menu("conststrings", "ConstStrings List", render_window);

			// Pre-fill once so list isn't empty at startup
			refresh_configstrings();
		}
	};
}

REGISTER_COMPONENT(conststrings::component)
#endif