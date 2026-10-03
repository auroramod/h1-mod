#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "config_strings.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/string.hpp>

static constexpr std::uint32_t old_max_config_strings = 0x1371;
static constexpr std::uint32_t extra_config_strings = 0x80;
static constexpr std::uint32_t max_config_strings = old_max_config_strings + extra_config_strings;
static_assert(max_config_strings == game::MAX_CONFIGSTRINGS);

static constexpr std::uint32_t first_shifted_index = 0x114D;
static constexpr std::size_t net_const_string_types = 0x1B;
static constexpr std::size_t anim_net_const_string_type = 0x13;
static constexpr std::uint32_t old_max_anim_config_strings = 0x7F;
static constexpr std::uint32_t max_anim_config_strings = 0xFF;

static constexpr std::uint32_t max_gamestate_chars = 0x20000;
static constexpr std::uint32_t config_string_size = sizeof(int);
static constexpr std::uint32_t grown_offsets_size = extra_config_strings * config_string_size;

static int* relocated_config_strings = nullptr;

static utils::hook::detour sv_clear_server_hook;

void config_strings::post_unpack()
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

int* config_strings::get_server_config_strings()
{
	return relocated_config_strings;
}

void config_strings::shift_net_const_strings()
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

void config_strings::relocate_server_config_strings()
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

void config_strings::relocate_type_map()
{
	const auto type_map = static_cast<std::uint8_t*>(utils::memory::allocate_near(image_base, max_config_strings, PAGE_READWRITE));
	std::memset(type_map, 0xFF, max_config_strings);

	for (const auto address : type_map_references)
	{
		relocate_lea(address, config_string_type_map, reinterpret_cast<std::size_t>(type_map));
	}
}

void config_strings::patch_value(const std::size_t address, const std::uint32_t old_value, const std::uint32_t new_value)
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

void config_strings::relocate_lea(const std::size_t address, const std::size_t old_target, const std::size_t new_target)
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

void config_strings::sv_clear_server_stub()
{
	sv_clear_server_hook.invoke<void>();
	std::memset(relocated_config_strings, 0, max_config_strings * config_string_size);
}

REGISTER_COMPONENT(config_strings)
