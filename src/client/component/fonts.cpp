#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "fonts.hpp"
#include "filesystem.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/image.hpp>
#include <utils/concurrency.hpp>

namespace fonts
{
	namespace
	{
		struct font_data_t
		{
			std::unordered_map<std::string, game::TTFDef*> fonts;
			std::unordered_map<std::string, std::string> raw_fonts;
		};

		utils::concurrency::container<font_data_t> font_data;

		game::TTFDef* create_font(const std::string& name, const std::string& data)
		{
			const auto font = utils::memory::get_allocator()->allocate<game::TTFDef>();
			font->name = utils::memory::get_allocator()->duplicate_string(name);
			font->file = utils::memory::get_allocator()->duplicate_string(data);
			font->fileLen = static_cast<int>(data.size());
			font->ftFace = nullptr;
			return font;
		}

		void free_font(game::TTFDef* font)
		{
			utils::memory::get_allocator()->free(font->file);
			utils::memory::get_allocator()->free(font->name);
			utils::memory::get_allocator()->free(font);
		}

		game::TTFDef* load_font(const std::string& name)
		{
			return font_data.access<game::TTFDef*>([&](font_data_t& data_) -> game::TTFDef*
			{
				if (const auto i = data_.fonts.find(name); i != data_.fonts.end())
				{
					return i->second;
				}

				std::string data{};
				if (const auto i = data_.raw_fonts.find(name); i != data_.raw_fonts.end())
				{
					data = i->second;
				}

				if (data.empty() && !filesystem::read_file(name, &data))
				{
					return nullptr;
				}

				const auto material = create_font(name, data);
				data_.fonts[name] = material;

				return material;
			});
		}

		game::TTFDef* try_load_font(const std::string& name)
		{
			try
			{
				return load_font(name);
			}
			catch (const std::exception& e)
			{
				console::error("Failed to load font %s: %s\n", name.data(), e.what());
			}

			return nullptr;
		}

		game::TTFDef* db_find_xasset_header_stub(game::XAssetType type, const char* name, int create_default)
		{
			auto result = try_load_font(name);
			if (result == nullptr)
			{
				result = game::DB_FindXAssetHeader(type, name, create_default).ttfDef;
			}
			return result;
		}
	}

	void add(const std::string& name, const std::string& data)
	{
		font_data.access([&](font_data_t& data_)
		{
			data_.raw_fonts[name] = data;
		});
	}

	void clear()
	{
		font_data.access([&](font_data_t& data_)
		{
			for (auto& font : data_.fonts)
			{
				free_font(font.second);
			}

			data_.fonts.clear();
			utils::hook::set<int>(SELECT_VALUE(0xF793E38_b, 0xE962188_b), 0); // reset registered font count
		});
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			utils::hook::call(SELECT_VALUE(0x55C596_b, 0x67F6E6_b), db_find_xasset_header_stub);
		}
	};
}

REGISTER_COMPONENT(fonts::component)
