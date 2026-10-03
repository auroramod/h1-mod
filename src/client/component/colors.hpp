#pragma once
#include "loader/component_loader.hpp"

class colors final : public component_interface
{
public:
	void post_unpack() override;

private:
	static constexpr auto MAX_COLOR_INDEX = 15;

	struct hsv_color
	{
		unsigned char h;
		unsigned char s;
		unsigned char v;
	};

	enum color_mode_t
	{
		mode_original,
		mode_custom,
		mode_count,
	};

	static DWORD hsv_to_rgb(const hsv_color hsv);
	static int color_index(const char c);
	static char add(const std::int32_t mode, const uint8_t r, const uint8_t g, const uint8_t b);
	static void com_clean_name_stub(const char* in, char* out, const int out_size);
	static char* i_clean_str_stub(char* string);
	static size_t get_client_name_stub(const int local_client_num, const int index, char* buf, const int size,
		const size_t unk, const size_t unk2);
	static void rb_lookup_color_stub(const char index, DWORD* color);
};
