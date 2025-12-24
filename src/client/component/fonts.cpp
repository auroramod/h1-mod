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

		struct font_style_t
		{
			std::int32_t index;
			game::Font_s* handle;
		};

		enum font_style_csv
		{
			col_index = 0,
			col_font = 1,
			col_name = 2,
			col_count
		};

		std::vector<font_style_t> custom_font_styles;

		std::array<const char*, 11> default_hudelem_fonts =
		{
			"default",
			"bigfixed",
			"smallfixed",
			"objective",
			"big",
			"small",
			"hudbig",
			"hudsmall",
			"buttonprompt",
			"subtitle",
			"timer",
		};

		std::vector<const char*> custom_hudelem_fonts;

		void font_init_stub()
		{
			font_init_hook.invoke<void>();
			custom_font_styles.clear();
			custom_hudelem_fonts.clear();

			const auto font_styles = game::DB_FindXAssetHeader(game::ASSET_TYPE_STRINGTABLE, "ui/fontstyles.csv", 0).stringTable;
			if (font_styles->columnCount != col_count || font_styles->rowCount < 1)
			{
				return;
			}

			for (auto i = 0u; i < default_hudelem_fonts.size(); i++)
			{
				custom_hudelem_fonts.emplace_back(default_hudelem_fonts[i]);
			}

			const auto default_font = game::R_RegisterFont("fonts/default.otf", 20);

			for (auto i = 0; i < font_styles->rowCount; i++)
			{
				const auto index = game::StringTable_GetColumnValueForRow(font_styles, i, col_index);
				const auto font_name = game::StringTable_GetColumnValueForRow(font_styles, i, col_font);
				const auto hudelem_name = game::StringTable_GetColumnValueForRow(font_styles, i, col_name);

				auto font = game::R_RegisterFont(font_name, 20);
				if (font == nullptr)
				{
					font = default_font;
				}

				font_style_t style{};
				style.index = std::atoi(index);
				style.handle = font;
				custom_font_styles.emplace_back(style);
				custom_hudelem_fonts.emplace_back(hudelem_name);
			}
		}

		game::Font_s* get_custom_font(int font)
		{
			for (const auto& font_style : custom_font_styles)
			{
				if (font_style.index == font)
				{
					return font_style.handle;
				}
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

		int get_font_handle_index(int hudelem_font_index, int current)
		{
			const auto custom_index = hudelem_font_index - default_hudelem_fonts.size();
			if (custom_index >= 0 && custom_index < custom_font_styles.size())
			{
				return custom_font_styles[custom_index].index;
			}

			return current;
		}

		void get_hud_elem_info_stub(utils::hook::assembler& a)
		{
			a.push(ebx);
			a.pushad64();
			a.mov(edx, ebx);
			a.mov(ecx, dword_ptr(rsi, 4));
			a.call_aligned(get_font_handle_index);
			a.mov(dword_ptr(rsp, 0x80), eax);
			a.popad64();
			a.pop(ebx);

			a.mov(edx, dword_ptr(rdi, 0x238));
			a.lea(r8, qword_ptr(rsp, 0x98));

			a.jmp(0x181877_b);
		}

		void hudelem_setfont_stub(__int64 a1, __int64 a2, __int64 /*a3*/, int /*a4*/)
		{
			utils::hook::invoke<void>(0x41A8B0_b, a1, a2, custom_hudelem_fonts.data(), custom_hudelem_fonts.size());
		}

		void* hudelem_getfont_stub_get_fonts()
		{
			return custom_hudelem_fonts.data();
		}

		void hudelem_getfont_stub(utils::hook::assembler& a)
		{
			a.push(rcx);
			a.pushad64();
			a.call_aligned(hudelem_getfont_stub_get_fonts);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rcx);

			a.movsxd(rax, dword_ptr(r8, 0x1C));
			a.and_(rdx, rax);
			a.mov(rcx, qword_ptr(rcx, rdx, 3));
			a.jmp(0x50EC50_b);
		}

		void hudelem_set_font_handle_stub(utils::hook::assembler& a)
		{
			a.push(rbx);
			a.pushad64();
			a.mov(edx, ebx);
			a.xor_(rcx, rcx);
			a.call_aligned(0x1DF8B0_b);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rbx);

			a.jmp(0x1818F1_b);
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

				utils::hook::jump(0x181869_b, utils::hook::assemble(get_hud_elem_info_stub), true);
				utils::hook::jump(0x41868E_b, hudelem_setfont_stub);
				utils::hook::jump(0x4186C5_b, utils::hook::assemble(hudelem_getfont_stub), true);
				utils::hook::jump(0x1818A4_b, utils::hook::assemble(hudelem_set_font_handle_stub), true);
			}
		}
	};
}

REGISTER_COMPONENT(fonts::component)
