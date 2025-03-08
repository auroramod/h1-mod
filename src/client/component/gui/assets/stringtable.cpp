#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/fastfiles.hpp"
#include "component/gsc/script_loading.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"

#include "utils/mapents.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>

namespace gui::asset_list::stringtable
{
	namespace
	{
		void copy_table(game::StringTable* asset)
		{
			std::string buffer;
			for (auto row = 0; row < asset->rowCount; row++)
			{
				for (auto column = 0; column < asset->columnCount; column++)
				{
					const auto index = (row * asset->columnCount) + column;
					const auto string_value = asset->values[index].string;
					const auto last_char = (column == asset->columnCount - 1) ? "\n" : ",";

					if (string_value == nullptr)
					{
						buffer.append(last_char);
					}
					else
					{
						std::string str = string_value;
						auto added_quotes = false;
						if (str.contains(','))
						{
							added_quotes = true;
							str.insert(str.begin(), '"');
							str.insert(str.end(), '"');
						}

						if (str.contains('\"') && !added_quotes)
						{
							str = std::regex_replace(str, std::regex("\""), "\\\"");

							str.insert(str.begin(), '"');
							str.insert(str.end(), '"');
						}

						str = std::regex_replace(str, std::regex("\n"), "\\n");
						buffer.append(utils::string::va("%s%s", str.data(), last_char));
					}
				}
			}

			gui::copy_to_clipboard(buffer);
		}

		bool draw_asset(game::StringTable* asset)
		{
			if (asset->columnCount * asset->rowCount == 0)
			{
				ImGui::Text("empty table");
				return true;
			}

			constexpr auto flags =
				ImGuiTableFlags_BordersInnerH |
				ImGuiTableFlags_BordersOuterH |
				ImGuiTableFlags_BordersInnerV |
				ImGuiTableFlags_BordersOuterV |
				ImGuiTableFlags_RowBg |
				ImGuiTableFlags_ScrollX |
				ImGuiTableFlags_ScrollY;

			const auto size = ImGui::GetContentRegionAvail();

			if (ImGui::Button("copy all"))
			{
				copy_table(asset);
			}

			if (ImGui::BeginTable(asset->name, asset->columnCount, flags, size))
			{
				for (auto row = 0; row < asset->rowCount; row++)
				{
					ImGui::TableNextRow();

					for (auto column = 0; column < asset->columnCount; column++)
					{
						ImGui::TableSetColumnIndex(column);
						const auto index = (row * asset->columnCount) + column;
						const auto string_value = asset->values[index].string;
						if (string_value == nullptr)
						{
							ImGui::Text("");
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4(0, 0, 0, 0));
							ImGui::PushID(index);
							if (ImGui::Button(string_value))
							{
								gui::copy_to_clipboard(string_value);
							}
							ImGui::PopID();
							ImGui::PopStyleColor();
						}
					}
				}

				ImGui::EndTable();
			}

			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::StringTable>(game::ASSET_TYPE_STRINGTABLE, draw_asset, ImVec2(200, 200));
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::stringtable::component)
#endif
