#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "config_strings.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/string.hpp>

namespace config_strings
{
	namespace
	{
		struct shifted_index
		{
			std::size_t address;
			std::int32_t value;
		};

		constexpr std::size_t image_base = 0x140000000;

		constexpr std::size_t net_const_string_table = 0x14082FBB0;

		constexpr std::size_t string_data = 0x142CA288C;

		constexpr std::size_t server_config_strings = 0x14CA9B9A4;

		constexpr std::size_t config_string_type_map = 0x1428BB270;

		constexpr std::size_t anim_config_string_max = 0x1400789AC;

		constexpr std::size_t server_config_strings_end = 0x1404872DB;

		constexpr std::size_t max_config_string_checks[] =
		{
			0x1401C97F4, 0x1401C9A10, 0x1401C9B30, 0x1401CA0F6, 0x1401CA1E0, 0x1401CA210,
			0x1401CA30E, 0x1401CA35D, 0x1401CA3D8, 0x140243E5A, 0x14035419A, 0x140355334,
			0x14048434D, 0x1404843D5, 0x140485856, 0x14048674C,
		};

		constexpr std::size_t last_config_string_checks[] =
		{
			0x140243C59, 0x14025902F,
		};

		constexpr shifted_index shifted_indices[] =
		{
			{0x140073450, 0x1351}, {0x14007349D, 0x1351}, {0x1400734B5, 0x1351},
			{0x1400C700F, 0x114D}, {0x14021E84D, 0x1352},
			{0x140231EC3, -0x114D}, {0x140231EFB, 0x134D}, {0x140232204, -0x134E},
			{0x14023221D, 0x1350}, {0x140232227, 0x134F}, {0x140232234, 0x134E},
			{0x140236436, 0x1350}, {0x140236440, 0x134F}, {0x14023644D, 0x134E},
			{0x14033F23D, 0x114D}, {0x140340716, 0x134D},
			{0x1403670FC, 0x134E}, {0x14036715F, 0x134E}, {0x14036717B, 0x134F}, {0x140367197, 0x1350},
		};

		constexpr std::size_t string_data_references[] =
		{
			0x140243E38, 0x14024491E, 0x140244948, 0x14024496E, 0x14024535B, 0x14024537D, 0x140258FFF,
		};

		constexpr std::size_t gamestate_char_checks[] =
		{
			0x140243E03, 0x140259094,
		};

		constexpr std::size_t string_offsets_sizes[] =
		{
			0x140243D76, 0x140243DD9, 0x140258FC8,
		};

		constexpr std::size_t server_config_string_references[] =
		{
			0x14047E944, 0x14048430B, 0x14048584F, 0x140485925, 0x140485B30, 0x140485B63,
			0x14048669D, 0x14048677A, 0x1404867AE, 0x1404872D4, 0x14048774A,
		};

		constexpr std::size_t type_map_references[] =
		{
			0x1401C97FA, 0x1401CA0EF, 0x1401CA1EA, 0x1401CA21A,
		};

		// image base relative accesses (NetConstStrings config string lookup and type map build)
		constexpr std::size_t type_map_rva_references[] =
		{
			0x1401CA12C, 0x1401CA3EE,
		};

		struct net_const_string_range
		{
			unsigned int start;
			unsigned int max;
		};

		constexpr std::uint32_t old_max_config_strings = 0x1371;
		constexpr std::uint32_t extra_config_strings = 0x80;
		constexpr std::uint32_t max_config_strings = old_max_config_strings + extra_config_strings;
		static_assert(max_config_strings == game::MAX_CONFIGSTRINGS);

		constexpr std::uint32_t first_shifted_index = 0x114D;
		constexpr std::size_t net_const_string_types = 0x1B;
		constexpr std::size_t anim_net_const_string_type = 0x13;
		constexpr std::uint32_t old_max_anim_config_strings = 0x7F;
		constexpr std::uint32_t max_anim_config_strings = 0xFF;

		constexpr std::uint32_t max_gamestate_chars = 0x20000;
		constexpr std::uint32_t config_string_size = sizeof(int);
		constexpr std::uint32_t grown_offsets_size = extra_config_strings * config_string_size;

		int* relocated_config_strings = nullptr;

		utils::hook::detour sv_clear_server_hook;

		void patch_value(const std::size_t address, const std::uint32_t old_value, const std::uint32_t new_value)
		{
			for (auto i = 1u; i <= 8; i++)
			{
				if (*reinterpret_cast<std::uint32_t*>(address + i) == old_value)
				{
					utils::hook::set<std::uint32_t>(address + i, new_value);
					return;
				}
			}

			throw std::runtime_error(utils::string::va("config string value %X not found (%llX)", old_value, address));
		}

		void shift_net_const_strings()
		{
			const auto table = reinterpret_cast<net_const_string_range*>(net_const_string_table);
			for (auto i = 0u; i < net_const_string_types; i++)
			{
				if (table[i].start == old_max_config_strings)
				{
					utils::hook::set<std::uint32_t>(&table[i].start, max_config_strings);
				}
				else if (table[i].start >= first_shifted_index)
				{
					utils::hook::set<std::uint32_t>(&table[i].start, table[i].start + extra_config_strings);
				}
			}

			utils::hook::set<std::uint32_t>(&table[anim_net_const_string_type].max, max_anim_config_strings);
			patch_value(anim_config_string_max, old_max_anim_config_strings, max_anim_config_strings);
		}

		void relocate_lea(const std::size_t address, const std::size_t old_target, const std::size_t new_target)
		{
			for (auto i = 2u; i <= 3; i++)
			{
				const auto end = address + i + 4;
				const auto disp = reinterpret_cast<std::int32_t*>(address + i);
				if (end + *disp != old_target)
				{
					continue;
				}

				const auto new_disp = static_cast<std::int64_t>(new_target) - static_cast<std::int64_t>(end);
				if (new_disp != static_cast<std::int32_t>(new_disp))
				{
					break;
				}

				utils::hook::set<std::int32_t>(disp, static_cast<std::int32_t>(new_disp));
				return;
			}

			throw std::runtime_error(utils::string::va("config string lea to %llX not found (%llX)", old_target, address));
		}

		void relocate_server_config_strings()
		{
			relocated_config_strings = static_cast<int*>(utils::memory::allocate_near(image_base, max_config_strings * config_string_size, PAGE_READWRITE));
			const auto new_address = reinterpret_cast<std::size_t>(relocated_config_strings);

			for (const auto address : server_config_string_references)
			{
				relocate_lea(address, server_config_strings, new_address);
			}

			relocate_lea(server_config_strings_end, server_config_strings + old_max_config_strings * config_string_size,
				reinterpret_cast<std::size_t>(relocated_config_strings + max_config_strings));
		}

		void relocate_rva(const std::size_t address, const std::size_t old_target, const std::size_t new_target)
		{
			const auto old_rva = static_cast<std::int64_t>(old_target - image_base);
			const auto new_rva = static_cast<std::int64_t>(new_target) - static_cast<std::int64_t>(image_base);
			if (new_rva != static_cast<std::int32_t>(new_rva))
			{
				throw std::runtime_error(utils::string::va("config string rva to %llX out of range (%llX)", new_target, address));
			}

			for (auto i = 3u; i <= 5; i++)
			{
				const auto disp = reinterpret_cast<std::int32_t*>(address + i);
				if (*disp == old_rva)
				{
					utils::hook::set<std::int32_t>(disp, static_cast<std::int32_t>(new_rva));
					return;
				}
			}

			throw std::runtime_error(utils::string::va("config string rva to %llX not found (%llX)", old_target, address));
		}

		void relocate_type_map()
		{
			const auto type_map = static_cast<std::uint8_t*>(utils::memory::allocate_near(image_base, max_config_strings, PAGE_READWRITE));
			std::memset(type_map, 0xFF, max_config_strings);

			for (const auto address : type_map_references)
			{
				relocate_lea(address, config_string_type_map, reinterpret_cast<std::size_t>(type_map));
			}

			for (const auto address : type_map_rva_references)
			{
				relocate_rva(address, config_string_type_map, reinterpret_cast<std::size_t>(type_map));
			}
		}

		void sv_clear_server_stub()
		{
			sv_clear_server_hook.invoke<void>();
			std::memset(relocated_config_strings, 0, max_config_strings * config_string_size);
		}
	}

	int* get_server_config_strings()
	{
		return relocated_config_strings;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			for (const auto address : max_config_string_checks)
			{
				patch_value(address, old_max_config_strings, max_config_strings);
			}

			for (const auto address : last_config_string_checks)
			{
				patch_value(address, old_max_config_strings - 1, max_config_strings - 1);
			}

			for (const auto& [address, value] : shifted_indices)
			{
				const auto shift = static_cast<std::int32_t>(extra_config_strings);
				const auto new_value = value > 0 ? value + shift : value - shift;
				patch_value(address, static_cast<std::uint32_t>(value), static_cast<std::uint32_t>(new_value));
			}

			shift_net_const_strings();

			const auto string_data_new = string_data + grown_offsets_size;
			for (const auto address : string_data_references)
			{
				relocate_lea(address, string_data, string_data_new);
			}

			for (const auto address : gamestate_char_checks)
			{
				patch_value(address, max_gamestate_chars, max_gamestate_chars - grown_offsets_size);
			}

			for (const auto address : string_offsets_sizes)
			{
				patch_value(address, old_max_config_strings * config_string_size, max_config_strings * config_string_size);
			}

			relocate_server_config_strings();
			relocate_type_map();

			sv_clear_server_hook.create(game::mp::SV_ClearServer, sv_clear_server_stub);
		}
	};
}

REGISTER_COMPONENT(config_strings::component)
