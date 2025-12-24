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
				const std::string override_name = utils::string::va("override/%s", name);
				if (result && game::DB_XAssetExists(game::ASSET_TYPE_TTF, override_name.data()))
				{
					const auto override_font = game::DB_FindXAssetHeader(type, override_name.data(), 0);
					if (override_font.ttfDef != nullptr)
					{
						return override_font.ttfDef;
					}
				}
			}
			return result;
		}

		int font_name_compare_stub(const char* a1, const char* a2)
		{
			if (!strncmp(a1, "override/", 9) && !strcmp(a1 + 9, a2))
			{
				return 0;
			}

			return utils::hook::invoke<int>(SELECT_VALUE(0x3CD370_b, 0x5AF5F0_b), a1, a2);
		}

		utils::hook::detour font_init_hook;
		utils::hook::detour ui_get_font_handle_hook;
		utils::hook::detour ui_get_font_handle_hook2;
		std::vector<game::Font_s*> custom_font_styles;
		constexpr const auto custom_font_styles_begin = 10;

		void font_init_stub()
		{
			font_init_hook.invoke<void>();
			custom_font_styles.clear();

			const auto font_styles = game::DB_FindXAssetHeader(game::ASSET_TYPE_STRINGTABLE, "ui/fontstyles.csv", 0).stringTable;
			if (font_styles->columnCount < 1 || font_styles->rowCount < 1)
			{
				return;
			}

			for (auto i = 0; i < font_styles->columnCount; i++)
			{
				if (font_styles->values[i].string == nullptr)
				{
					custom_font_styles.emplace_back(game::R_RegisterFont("fonts/default.otf", 20));
					continue;
				}

				const auto font = game::R_RegisterFont(font_styles->values[i].string, 20);
				if (font == nullptr)
				{
					custom_font_styles.emplace_back(game::R_RegisterFont("fonts/default.otf", 20));
					continue;
				}

				printf("Custom font style %s = %lli\n", font_styles->values[i].string, custom_font_styles.size() + custom_font_styles_begin);
				custom_font_styles.emplace_back(font);
			}
		}

		game::Font_s* get_custom_font(int font)
		{
			const auto custom_font_index = font - custom_font_styles_begin;
			if (custom_font_index >= 0 && custom_font_index < custom_font_styles.size())
			{
				return custom_font_styles[custom_font_index];
			}

			return nullptr;
		}

		game::Font_s* ui_get_font_handle_stub(void* a1, int font)
		{
			const auto custom_font = get_custom_font(font);
			if (custom_font != nullptr)
			{
				return custom_font;
			}

			return ui_get_font_handle_hook.invoke<game::Font_s*>(a1, font);
		}

		game::Font_s* ui_get_font_handle_stub2(void* a1, __int64 a2)
		{
			const auto font = *reinterpret_cast<int*>(a2 + 208);
			const auto custom_font = get_custom_font(font);
			if (custom_font != nullptr)
			{
				return custom_font;
			}

			return ui_get_font_handle_hook2.invoke<game::Font_s*>(a1, a2);
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

			utils::hook::call(SELECT_VALUE(0x4D4137_b, 0x67F667_b), font_name_compare_stub);
			utils::hook::call(SELECT_VALUE(0x55C596_b, 0x67F6E6_b), db_find_xasset_header_stub);

			if (game::environment::is_mp())
			{
				font_init_hook.create(0x1D9B30_b, font_init_stub);
				ui_get_font_handle_hook.create(0x1DF8B0_b, ui_get_font_handle_stub);
				ui_get_font_handle_hook2.create(0x192360_b, ui_get_font_handle_stub2);
			}
		}
	};
}

REGISTER_COMPONENT(fonts::component)
