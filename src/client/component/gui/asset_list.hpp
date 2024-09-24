#ifdef DEBUG
#pragma once
#include "game/structs.hpp"
#include "gui.hpp"

namespace gui::asset_list
{
	void add_asset_view_callback(game::XAssetType, const std::function<void(const std::string&)>& callback);
	void add_view_button(int id, game::XAssetType type, const char* name);

	template <typename T>
	void add_asset_view(game::XAssetType type, const std::function<bool(T*)>& draw_callback, ImVec2 min_size = ImVec2(0, 0))
	{
		static std::unordered_set<std::string> opened_assets;
		add_asset_view_callback(type, [](const std::string& name)
		{
			opened_assets.insert(name);
		});

		gui::register_callback([=]()
		{
			for (auto i = opened_assets.begin(); i != opened_assets.end(); )
			{
				const auto& name = *i;
				const auto header = reinterpret_cast<T*>(game::DB_FindXAssetHeader(type, name.data(), 0).data);
				if (header == nullptr)
				{
					i = opened_assets.erase(i);
					continue;
				}

				if (min_size.x != 0 && min_size.y != 0)
				{
					auto& io = ImGui::GetIO();
					ImGui::SetNextWindowSizeConstraints(min_size, ImVec2(io.DisplaySize.x, io.DisplaySize.y));
				}

				auto is_open = true;
				if (ImGui::Begin(name.data(), &is_open))
				{
					if (!draw_callback(header))
					{
						is_open = false;
					}
				}
				ImGui::End();

				if (is_open)
				{
					++i;
				}
				else
				{
					i = opened_assets.erase(i);
				}
			}
		}, false);
	}
}
#endif
