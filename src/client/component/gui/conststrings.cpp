#include <std_include.hpp>

#ifdef _DEBUG

#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/scripting.hpp"
#include "gui.hpp"

namespace conststrings
{
	namespace
	{
		void render_window()
		{
			static auto* enabled = &gui::enabled_menus["configstrings"];

			ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(1000, 1000));
			if (!ImGui::Begin("ConfigString list", enabled))
			{
				ImGui::End();
				return;
			}

			ImGui::Text("count (%d):", game::MAX_CONFIGSTRINGS);

			ImGui::BeginChild("##cs_scroll", ImVec2(0, 0));
			ImGuiListClipper clipper;
			clipper.Begin(game::MAX_CONFIGSTRINGS);

			const auto config_strings = reinterpret_cast<int*>(0x2DD1E24_b);
			const auto mt_buffer = *reinterpret_cast<std::size_t*>(0xAC87D00_b);

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{
					const auto string_value = config_strings[i];
					const auto string = reinterpret_cast<const char*>(mt_buffer + 8 + 16 * string_value);
					if (string_value == 0)
					{
						ImGui::Text("%04d: ", i);
						ImGui::SameLine();
						ImGui::Button("(null)");
					}
					else
					{
						ImGui::Text("%04d: ", i);
						ImGui::SameLine();
						ImGui::PushID(i);
						if (ImGui::Button(string))
						{
							gui::copy_to_clipboard(string);
						}
						ImGui::PopID();
					}
				}
			}

			ImGui::EndChild();
			ImGui::End();
		}

		void ncs_window()
		{
			static auto* enabled = &gui::enabled_menus["netconststrings"];

			ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(1000, 1000));
			if (!ImGui::Begin("NetConstStrings List", enabled))
			{
				ImGui::End();
				return;
			}

			static const char* ncs_type_names[game::NETCONSTSTRINGTYPE_COUNT] =
			{
				"xmodel (mdl)",
				"material (mat)",
				"rumble (rmb)",
				"vehicles (veh)",
				"fx (vfx)",
				"locstring (loc)",
				"soundalias (snd)",
				"submix (sbx)",
				"soundalias_looping (snl)",
				"shock (shk)",
				"scriptmenu (mnu)",
				"client_tags (tag)",
				"headicon (hic)",
				"nameplate (nps)",
				"minimapicon (mic)",
				"locselmat (sel)",
				"weapon (wep)",
				"attachment (att)",
				"hintstring (hnt)",
				"anim (anm)",
				"tags (fxt)",
				"animclass (acl)",
				"lui (lui)",
				"laser (lsr)",
				"ncs24",
				"ncs25",
				"ncs26"
			};

			for (auto type = 0; type < 24; type++)
			{
				ImGui::PushID(type);
				if (ImGui::TreeNode(ncs_type_names[type]))
				{
					ImGui::Text("%i / %i", game::s_netConstStringMaps[type].ncsCount, game::s_oldConfigStringToNetStringMap[type].csMax);
					auto count = 0u;
					for (auto iter = game::s_netConstStringMaps[type].head; iter != nullptr; iter = iter->next)
					{
						for (auto i = 0u; i < iter->ncs->entryCount; i++)
						{
							ImGui::Text("%04d: ", count++);
							ImGui::SameLine();
							ImGui::PushID(count);
							if (ImGui::Button(iter->ncs->stringList[i]))
							{
								gui::copy_to_clipboard(iter->ncs->stringList[i]);
							}
							ImGui::PopID();
						}
					}

					if (count < game::s_oldConfigStringToNetStringMap[type].csMax)
					{
						for (auto i = 0u; i < game::s_oldConfigStringToNetStringMap[type].csMax - count; i++)
						{
							ImGui::Text("%04d: (null)", count + i);
						}
					}

					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			ImGui::End();
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
				return;

			gui::register_menu("configstrings", "ConfigString List", render_window);
			gui::register_menu("netconststrings", "NetConstStrings List", ncs_window);
		}
	};
}

REGISTER_COMPONENT(conststrings::component)
#endif