#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "filesystem.hpp"
#include "localized_strings.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>

namespace
{
	using localized_map = std::unordered_map<std::string, std::string>;
}

static utils::hook::detour seh_string_ed_get_string_hook;
static utils::concurrency::container<localized_map> localized_overrides;

void localized_strings::post_unpack()
{
	// Change some localized strings
	seh_string_ed_get_string_hook.create(SELECT_VALUE(0x1403E6CE0, 0x1404BB2A0), &seh_string_ed_get_string);
}

void localized_strings::override(const std::string& key, const std::string& value)
{
	localized_overrides.access([&](localized_map& map)
	{
		map[key] = value;
	});
}

const char* localized_strings::seh_string_ed_get_string(const char* reference)
{
	return localized_overrides.access<const char*>([&](const localized_map& map)
	{
		const auto entry = map.find(reference);
		if (entry != map.end())
		{
			return utils::string::va("%s", entry->second.data());
		}

		return seh_string_ed_get_string_hook.invoke<const char*>(reference);
	});
}

REGISTER_COMPONENT(localized_strings)
