#include <std_include.hpp>

#ifdef DEBUG
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "component/fastfiles.hpp"
#include "../gui.hpp"
#include "../asset_list.hpp"

#include <utils/string.hpp>
#include <utils/compression.hpp>

namespace gui::asset_list::rawfile
{
	namespace
	{
		std::string& get_decompressed_buffer(game::RawFile* asset, bool force = false)
		{
			static std::unordered_map<std::string, std::string> buffers;

			if (!force)
			{
				if (const auto iter = buffers.find(asset->name); iter != buffers.end())
				{
					return iter->second;
				}
			}

			const auto decompressed = utils::compression::zlib::decompress({asset->buffer, asset->buffer + asset->compressedLen});
			buffers[asset->name] = decompressed;

			return get_decompressed_buffer(asset);
		}

		bool draw_asset(game::RawFile* asset)
		{
			const auto reset_buffer = ImGui::Button("reset");
			auto& buffer = get_decompressed_buffer(asset, reset_buffer);

			ImGui::SameLine();

			if (ImGui::Button("copy"))
			{
				gui::copy_to_clipboard(buffer);
			}

			ImGui::PushItemWidth(-1);
			ImGui::InputTextMultiline("buffer", &buffer, ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly);
			ImGui::PopItemWidth();

			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			gui::asset_list::add_asset_view<game::RawFile>(game::ASSET_TYPE_RAWFILE, draw_asset, ImVec2(450, 450));
		}
	};
}

REGISTER_COMPONENT(gui::asset_list::rawfile::component)
#endif
