#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/scheduler.hpp"
#include "component/command.hpp"
#include "component/fastfiles.hpp"
#include "gui.hpp"
#include "asset_list.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

namespace gui::asset_list
{
	namespace
	{
		bool shown_assets[game::XAssetType::ASSET_TYPE_COUNT]{};
		std::string asset_type_filter;
		std::string assets_name_filter[game::XAssetType::ASSET_TYPE_COUNT];
		std::string assets_value_filter[game::XAssetType::ASSET_TYPE_COUNT];
		std::string zone_name_filter[game::XAssetType::ASSET_TYPE_COUNT];

		std::unordered_map<game::XAssetType, std::function<void(const std::string&)>> asset_view_callbacks;

		bool default_only[game::ASSET_TYPE_COUNT] = {};
		int asset_count[game::ASSET_TYPE_COUNT] = {};
		bool disabled_zones[game::ASSET_TYPE_COUNT][0x100] = {};
		bool show_asset_zone = true;

		void draw_table_row(game::XAssetType type, const game::XAssetEntry* entry, bool should_add_view_btn)
		{
			const auto asset = entry->asset;
			auto asset_name = game::DB_GetXAssetName(&asset);
			const auto is_default = entry->zoneIndex == 0;

			if (asset_name[0] == '\0' || disabled_zones[type][entry->zoneIndex] || default_only[type] && !is_default)
			{
				return;
			}

			if (is_default)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.43f, 0.15f, 0.15f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.98f, 0.26f, 0.26f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.98f, 0.06f, 0.06f, 1.f));
			}

			const auto _0 = gsl::finally([&]
			{
				if (is_default)
				{
					ImGui::PopStyleColor(3);
				}
			});

			auto col_index = 0;
			if (!utils::string::strstr_lower(asset_name, assets_name_filter[type].data()))
			{
				return;
			}

			if (type == game::ASSET_TYPE_LOCALIZE_ENTRY)
			{
				if (!utils::string::strstr_lower(entry->asset.header.localize->value, assets_value_filter[type].data()))
				{
					return;
				}
			}

			ImGui::TableNextRow();

			if (should_add_view_btn)
			{
				ImGui::TableSetColumnIndex(col_index++);
				ImGui::PushID(asset_count[type]);
				if (ImGui::Button("view"))
				{
					asset_view_callbacks.at(type)(asset_name);
				}
				ImGui::PopID();
			}

			if (show_asset_zone)
			{
				ImGui::TableSetColumnIndex(col_index++);
				if (entry->zoneIndex > 0)
				{
					ImGui::Text(fastfiles::get_zone_name((int)entry->zoneIndex).data());
				}
				else
				{
					ImGui::Text("default");
				}
			}

			ImGui::TableSetColumnIndex(col_index++);

			if (ImGui::Button(asset_name))
			{
				gui::copy_to_clipboard(asset_name);
			}

			if (type == game::ASSET_TYPE_LOCALIZE_ENTRY)
			{
				ImGui::TableSetColumnIndex(col_index++);

				if (ImGui::Button(entry->asset.header.localize->value))
				{
					gui::copy_to_clipboard(entry->asset.header.localize->value);
				}
			}
		}

		void draw_asset_type_list()
		{
			if (ImGui::TreeNode("loaded zones"))
			{
				for (auto i = 1u; i <= *game::mp::g_zoneCount; i++)
				{
					if (ImGui::Button(fastfiles::get_zone_name(i).data()))
					{
						gui::copy_to_clipboard(fastfiles::get_zone_name(i));
					}
				}

				ImGui::TreePop();
			}

			ImGui::Checkbox("show asset zone", &show_asset_zone);
			ImGui::InputText("asset type", &asset_type_filter);
			ImGui::BeginChild("asset type list");

			for (auto i = 0; i < game::XAssetType::ASSET_TYPE_COUNT; i++)
			{
				const auto name = game::g_assetNames[i];
				const auto type = static_cast<game::XAssetType>(i);

				if (asset_type_filter.size() == 0 || utils::string::strstr_lower(name, asset_type_filter.data()))
				{
					ImGui::Checkbox(name, &shown_assets[type]);
				}
			}

			ImGui::EndChild();
		}

		void draw_asset_list_filter(const game::XAssetType type)
		{
			ImGui::Text("count: %i / %i", asset_count[type], game::g_poolSize[type]);
			ImGui::InputText("name", &assets_name_filter[type]);

			if (type == game::ASSET_TYPE_LOCALIZE_ENTRY)
			{
				ImGui::InputText("value", &assets_value_filter[type]);
			}

			if (ImGui::InputText("zone name", &zone_name_filter[type]))
			{
				for (auto zone = 0u; zone <= *game::mp::g_zoneCount; zone++)
				{
					const auto zone_name = fastfiles::get_zone_name(zone);
					disabled_zones[type][zone] = !utils::string::strstr_lower(zone_name.data(), zone_name_filter[type].data());
				}
			}

			ImGui::Checkbox("default assets only", &default_only[type]);
		}

		constexpr auto get_table_flags()
		{
			constexpr auto flags =
				ImGuiTableFlags_BordersInnerH |
				ImGuiTableFlags_BordersOuterH |
				ImGuiTableFlags_BordersInnerV |
				ImGuiTableFlags_BordersOuterV |
				ImGuiTableFlags_RowBg |
				ImGuiTableFlags_ScrollX |
				ImGuiTableFlags_ScrollY;

			return flags;
		}

		void draw_asset_list_entries(const game::XAssetType type)
		{
			const auto should_add_view_btn = asset_view_callbacks.contains(type);
			constexpr auto flags = get_table_flags();

			asset_count[type] = 0;

			ImGui::BeginChild("assets list");

			auto column_count = 1;
			column_count += should_add_view_btn;
			column_count += show_asset_zone;
			column_count += type == game::ASSET_TYPE_LOCALIZE_ENTRY;

			if (ImGui::BeginTable("assets", column_count, flags))
			{
				fastfiles::enum_asset_entries(type, [&](const game::XAssetEntry* entry)
				{
					asset_count[type]++;
					draw_table_row(type, entry, should_add_view_btn);
				}, true);

				ImGui::EndTable();
			}

			ImGui::EndChild();
		}

		void draw_asset_list(const game::XAssetType type)
		{
			if (!shown_assets[type])
			{
				return;
			}

			const auto name = game::g_assetNames[type];

			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), io.DisplaySize);
			ImGui::Begin(name, &shown_assets[type]);

			draw_asset_list_filter(type);
			draw_asset_list_entries(type);

			ImGui::End();
		}

		void render_window()
		{
			static auto* enabled = &gui::enabled_menus["asset_list"];
			ImGui::Begin("Asset list", enabled);

			draw_asset_type_list();

			for (auto i = 0; i < game::XAssetType::ASSET_TYPE_COUNT; i++)
			{
				draw_asset_list(static_cast<game::XAssetType>(i));
			}

			ImGui::End();
		}
	}

	void add_view_button(int id, game::XAssetType type, const char* name)
	{
		if (asset_view_callbacks.contains(type))
		{
			ImGui::SameLine();
			ImGui::PushID(id);
			if (ImGui::Button("view"))
			{
				asset_view_callbacks.at(type)(name);
			}
			ImGui::PopID();
		}
	}

	void add_asset_view_callback(game::XAssetType type, const std::function<void(const std::string&)>& callback)
	{
		asset_view_callbacks.insert(std::make_pair(type, callback));
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			gui::register_menu("asset_list", "Asset List", render_window);
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::component)
#endif
