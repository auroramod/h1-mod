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

namespace gui::asset_list::mapents
{
	namespace
	{
		using entity_t = std::vector<std::pair<std::string, std::string>>;

		struct mapents_t
		{
			game::MapEnts* asset;
			std::string converted_mapents;
			std::thread parse_thread;
			std::atomic_bool is_parsing = false;
			std::atomic_bool done_parsing = false;
		};

		utils::concurrency::container<mapents_t, std::recursive_mutex> mapents;

		void parse_mapents(game::MapEnts* asset, mapents_t& data)
		{
			data.is_parsing = true;
			data.asset = asset;
			const auto str_data = std::string{asset->entityString, static_cast<size_t>(asset->numEntityChars)};
			data.parse_thread = std::thread([=]
			{
				const auto new_data = ::mapents::parse(str_data, [](const std::uint16_t id)
				{
					return gsc::gsc_ctx->token_name(id);
				});

				std::string converted_mapents;
				for (const auto& entity : new_data.entities)
				{
					converted_mapents.append("{\n");
					const auto var_list = entity.get_var_list();
					for (const auto& var : var_list)
					{
						if (var.sl_string)
						{
							converted_mapents.append(
								utils::string::va("0 \"%s\" \"%s\"\n", var.key.data(), var.value.data()));
						}
						else
						{
							converted_mapents.append(
								utils::string::va("\"%s\" \"%s\"\n", var.key.data(), var.value.data()));
						}
					}
					converted_mapents.append("}\n");
				}

				mapents.access([=](mapents_t& data)
				{
					data.is_parsing = false;
					data.done_parsing = true;
					data.converted_mapents = converted_mapents;
				});
			});
		}

		void draw_entity_string(game::MapEnts* asset)
		{
			ImGui::InputTextMultiline("entity_string", asset->entityString, asset->numEntityChars, 
				ImGui::GetWindowSize(), ImGuiInputTextFlags_ReadOnly);
		}

		bool create_and_open_mapents_file(mapents_t& data)
		{
			const auto current_path = std::filesystem::current_path().generic_string();
			const std::string path = utils::string::va("%s\\h2m-mod\\tmp\\%s", current_path.data(), data.asset->name);

			utils::io::write_file(path, data.converted_mapents, false);
			ShellExecuteA(nullptr, nullptr, path.data(), nullptr, nullptr, SW_SHOWNORMAL);

			return false;
		}

		bool draw_asset(game::MapEnts* asset)
		{
			return mapents.access<bool>([&](mapents_t& data)
			{
				if (!data.is_parsing && data.done_parsing)
				{
					if (data.parse_thread.joinable())
					{
						data.parse_thread.join();
					}
					else
					{
						create_and_open_mapents_file(data);
						return false;
					}
				}

				if (!data.is_parsing && !data.done_parsing)
				{
					parse_mapents(asset, data);
				}

				ImGui::Text("Parsing mapents...");
				return true;
			});
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::MapEnts>(game::ASSET_TYPE_MAP_ENTS, draw_asset);
		}

		void pre_destroy() override
		{
			utils::io::remove_directory("h2m-mod/tmp");
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::mapents::component)
#endif
