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
	if (game::environment::is_sp())
	{
		seh_string_ed_get_string_hook.create(0x1403E6CE0, &seh_string_ed_get_string);
	}
	else
	{
		utils::hook::jump(0x1404BB2A0, &seh_string_ed_get_string_mp);
	}
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

const char* localized_strings::seh_string_ed_get_string_mp(const char* reference)
{
	return localized_overrides.access<const char*>([&](const localized_map& map)
	{
		if (const auto entry = map.find(reference); entry != map.end())
		{
			return utils::string::va("%s", entry->second.data());
		}

		const auto loc_translate = *reinterpret_cast<game::dvar_t**>(0x14CEF9650); // loc_translate
		if (!loc_translate || !loc_translate->current.enabled || !*reference)
		{
			return reference;
		}

		if (!reference[1])
		{
			if (*reference < 0)
			{
				return static_cast<const char*>(&game::mp::s_singleCharUTF8Rep[3 * (*reference & 0x7F)]);
			}

			return reference;
		}

		const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_LOCALIZE_ENTRY, reference, 0);
		if (asset.localize)
		{
			return asset.localize->value;
		}

		return static_cast<const char*>(nullptr);
	});
}

REGISTER_COMPONENT(localized_strings)
