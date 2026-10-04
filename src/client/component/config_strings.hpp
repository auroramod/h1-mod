#pragma once
#include "loader/component_loader.hpp"

class config_strings final : public component_interface
{
public:
	void post_unpack() override;

	static int* get_server_config_strings();

private:
	struct shifted_index
	{
		std::size_t address;
		std::int32_t value;
	};

	static constexpr std::size_t image_base = 0x140000000;

	static constexpr std::size_t net_const_string_table = 0x14082FBB0;
	static constexpr std::size_t string_data = 0x142CA288C;
	static constexpr std::size_t server_config_strings = 0x14CA9B9A4;
	static constexpr std::size_t config_string_type_map = 0x1428BB270;

	static constexpr std::size_t anim_config_string_max = 0x1400789AC;
	static constexpr std::size_t server_config_strings_end = 0x1404872DB;

	static constexpr std::size_t max_config_string_checks[] =
	{
		0x1401C97F4, 0x1401C9A10, 0x1401C9B30, 0x1401CA0F6, 0x1401CA1E0, 0x1401CA210,
		0x1401CA30E, 0x1401CA35D, 0x1401CA3D8, 0x140243E5A, 0x14035419A, 0x140355334,
		0x14048434D, 0x1404843D5, 0x140485856, 0x14048674C,
	};

	static constexpr std::size_t last_config_string_checks[] =
	{
		0x140243C59, 0x14025902F,
	};

	static constexpr shifted_index shifted_indices[] =
	{
		{0x140073450, 0x1351}, {0x14007349D, 0x1351}, {0x1400734B5, 0x1351},
		{0x1400C700F, 0x114D}, {0x14021E84D, 0x1352},
		{0x140231EC3, -0x114D}, {0x140231EFB, 0x134D}, {0x140232204, -0x134E},
		{0x14023221D, 0x1350}, {0x140232227, 0x134F}, {0x140232234, 0x134E},
		{0x140236436, 0x1350}, {0x140236440, 0x134F}, {0x14023644D, 0x134E},
		{0x14033F23D, 0x114D}, {0x140340716, 0x134D},
		{0x1403670FC, 0x134E}, {0x14036715F, 0x134E}, {0x14036717B, 0x134F}, {0x140367197, 0x1350},
	};

	static constexpr std::size_t string_data_references[] =
	{
		0x140243E38, 0x14024491E, 0x140244948, 0x14024496E, 0x14024535B, 0x14024537D, 0x140258FFF,
	};

	static constexpr std::size_t gamestate_char_checks[] =
	{
		0x140243E03, 0x140259094,
	};

	static constexpr std::size_t string_offsets_sizes[] =
	{
		0x140243D76, 0x140243DD9, 0x140258FC8,
	};

	static constexpr std::size_t server_config_string_references[] =
	{
		0x14047E944, 0x14048430B, 0x14048584F, 0x140485925, 0x140485B30, 0x140485B63,
		0x14048669D, 0x14048677A, 0x1404867AE, 0x1404872D4, 0x14048774A,
	};

	static constexpr std::size_t type_map_references[] =
	{
		0x1401C97FA, 0x1401CA0EF, 0x1401CA1EA, 0x1401CA21A,
	};

	// image base relative accesses (NetConstStrings config string lookup and type map build)
	static constexpr std::size_t type_map_rva_references[] =
	{
		0x1401CA12C, 0x1401CA3EE,
	};

	struct net_const_string_range
	{
		unsigned int start;
		unsigned int max;
	};

	static void shift_net_const_strings();
	static void relocate_server_config_strings();
	static void relocate_type_map();

	static void patch_value(std::size_t address, std::uint32_t old_value, std::uint32_t new_value);
	static void relocate_lea(std::size_t address, std::size_t old_target, std::size_t new_target);
	static void relocate_rva(std::size_t address, std::size_t old_target, std::size_t new_target);

	static void sv_clear_server_stub();
};
