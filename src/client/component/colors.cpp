#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace colors
{
	struct hsv_color
	{
		unsigned char h;
		unsigned char s;
		unsigned char v;
	};

	namespace
	{
		std::vector<DWORD> color_table;

		DWORD hsv_to_rgb(const hsv_color hsv)
		{
			DWORD rgb;

			if (hsv.s == 0)
			{
				return RGB(hsv.v, hsv.v, hsv.v);
			}

			// converting to 16 bit to prevent overflow
			const unsigned int h = hsv.h;
			const unsigned int s = hsv.s;
			const unsigned int v = hsv.v;

			const auto region = static_cast<uint8_t>(h / 43);
			const auto remainder = (h - (region * 43)) * 6;

			const auto p = static_cast<uint8_t>((v * (255 - s)) >> 8);
			const auto q = static_cast<uint8_t>(
				(v * (255 - ((s * remainder) >> 8))) >> 8);
			const auto t = static_cast<uint8_t>(
				(v * (255 - ((s * (255 - remainder)) >> 8))) >> 8);

			switch (region)
			{
			case 0:
				rgb = RGB(v, t, p);
				break;
			case 1:
				rgb = RGB(q, v, p);
				break;
			case 2:
				rgb = RGB(p, v, t);
				break;
			case 3:
				rgb = RGB(p, q, v);
				break;
			case 4:
				rgb = RGB(t, p, v);
				break;
			default:
				rgb = RGB(v, p, q);
				break;
			}

			return rgb;
		}

		int color_index(const char c)
		{
			const auto index = c - 48;
			return index >= 0xC ? 7 : index;
		}

		char add(const uint8_t r, const uint8_t g, const uint8_t b)
		{
			const char index = '0' + static_cast<char>(color_table.size());
			color_table.push_back(RGB(r, g, b));
			return index;
		}

		void com_clean_name_stub(const char* in, char* out, const int out_size)
		{
			strncpy_s(out, out_size, in, _TRUNCATE);
		}

		char* i_clean_str_stub(char* string)
		{
			utils::string::strip(string, string, static_cast<int>(strlen(string)) + 1);

			return string;
		}

		size_t get_client_name_stub(const int local_client_num, const int index, char* buf, const int size,
			const size_t unk, const size_t unk2)
		{
			// CL_GetClientName (CL_GetClientNameAndClantag?)
			const auto result = utils::hook::invoke<size_t>(0x343BA0_b, local_client_num, index, buf, size, unk, unk2);

			utils::string::strip(buf, buf, size);

			return result;
		}

		void rb_lookup_color_stub(const char index, DWORD* color)
		{
			*color = RGB(255, 255, 255);

			if (index == '8')
			{
				*color = *reinterpret_cast<DWORD*>(SELECT_VALUE(0x0, 0xEA749B4_b)); // 1.15
			}
			else if (index == '9')
			{
				*color = *reinterpret_cast<DWORD*>(SELECT_VALUE(0x0, 0xEA749B8_b)); // 1.15
			}
			else if (index == ':')
			{
				*color = hsv_to_rgb({static_cast<uint8_t>((game::Sys_Milliseconds() / 100) % 256), 255, 255});
			}
			else if (index == ';')
			{
				*color = *reinterpret_cast<DWORD*>(SELECT_VALUE(0x0, 0xEA749C0_b)); // 1.15
			}
			else if (index == '<')
			{
				*color = 0xFFFCFF80;
			}
			else
			{
				*color = color_table[color_index(index)];
			}
		}
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

			if (!game::environment::is_sp())
			{
				// allows colored name in-game
				utils::hook::jump(0x5AEDF0_b, com_clean_name_stub, true);

				// don't apply colors to overhead names
				/*utils::hook::jump(0xF7B6A_b, utils::hook::assemble([](utils::hook::assembler& a)
				{
					a.lea(rax, qword_ptr(rbp, -0x10));
					a.mov(dword_ptr(rsp, 0x28), 0xB);
					a.mov(r9d, 0x20);
					a.mov(qword_ptr(rsp, 0x20), rax);
					a.lea(r8, qword_ptr(rbp));

					a.push(rax);
					a.pushad64();
					a.call_aligned(get_client_name_stub);
					a.mov(qword_ptr(rsp, 0x80), rax);
					a.popad64();
					a.pop(rax);

					a.jmp(0xF7B8A_b);
				}), true);*/

				// patch I_CleanStr
				utils::hook::jump(0x5AF2E0_b, i_clean_str_stub, true);
			}

			// force new colors
			utils::hook::jump(SELECT_VALUE(0x5B17E0_b, 0x6C9460_b), rb_lookup_color_stub, true);

			// add colors
			add(0, 0, 0); // 0  - Black
			add(255, 49, 49); // 1  - Red
			add(134, 192, 0); // 2  - Green
			add(255, 173, 34); // 3  - Yellow
			add(0, 135, 193); // 4  - Blue
			add(32, 197, 255); // 5  - Light Blue
			add(151, 80, 221); // 6  - Pink
			add(255, 255, 255); // 7  - White

			add(0, 0, 0); // 8  - Team color (axis?)
			add(0, 0, 0); // 9  - Team color (allies?)

			add(0, 0, 0); // 10 - Rainbow (:)
			add(0, 0, 0);
			// 11 - Server color (;) - using that color in infostrings (e.g. your name) fails, ';' is an illegal character!
		}
	};
}

REGISTER_COMPONENT(colors::component)
