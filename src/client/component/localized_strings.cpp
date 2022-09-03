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

namespace localized_strings
{
	namespace
	{
		utils::hook::detour seh_string_ed_get_string_hook;

		struct localize_entry
		{
			std::string value{};
			bool volatile_{};
		};

		using localized_map = std::unordered_map<std::string, localize_entry>;
		utils::concurrency::container<localized_map> localized_overrides;

		const char* seh_string_ed_get_string(const char* reference)
		{
			return localized_overrides.access<const char*>([&](const localized_map& map)
			{
				const auto entry = map.find(reference);
				if (entry != map.end())
				{
					return utils::string::va("%s", entry->second.value.data());
				}

				return seh_string_ed_get_string_hook.invoke<const char*>(reference);
			});
		}

		game::XAssetHeader db_find_localize_entry_stub(game::XAssetType type, const char* name, int allow_create_default)
		{
			const auto value = localized_overrides.access<const char*>([&](const localized_map& map)
				-> const char*
			{
				const auto entry = map.find(name);
				if (entry != map.end())
				{
					return utils::string::va("%s", entry->second.value.data());
				}

				return nullptr;
			});

			if (value == nullptr)
			{
				return game::DB_FindXAssetHeader(type, name, allow_create_default);
			}

			static game::LocalizeEntry entry{};
			entry.value = value;
			entry.name = name;

			return static_cast<game::XAssetHeader>(&entry);
		}

		bool parse_localized_strings_file(const std::string& data)
		{
			rapidjson::Document j;
			j.Parse(data.data());

			if (!j.IsObject())
			{
				return false;
			}

			localized_overrides.access([&](localized_map& map)
			{
				const auto obj = j.GetObj();
				for (const auto& [key, value] : obj)
				{
					if (!key.IsString() || !value.IsString())
					{
						continue;
					}

					const auto name = key.GetString();
					const auto str = value.GetString();

					const auto entry = map.find(name);
					if (entry == map.end() || entry->second.volatile_)
					{
						map[name] = {str, true};
					}
				}
			});

			return true;
		}

		bool try_load_file(const std::string& path, const std::string& language)
		{
			const auto file = utils::string::va("%s/localizedstrings/%s.json", path.data(), language.data());
			if (!utils::io::file_exists(file))
			{
				return false;
			}

			console::info("Parsing %s\n", file);
			const auto data = utils::io::read_file(file);
			if (!parse_localized_strings_file(data))
			{
				console::error("Invalid language json file\n");
				return false;
			}

			return true;
		}

		void load_localized_strings()
		{
			bool found = false;

			const auto search_paths = filesystem::get_search_paths_rev();
			const auto language = game::SEH_GetCurrentLanguageName();

			for (const auto& path : search_paths)
			{
				bool found_in_current_path = false;

				if (try_load_file(path, "english"))
				{
					found_in_current_path = true;
					found = true;
				}

				if (language != "english"s && !try_load_file(path, language) && found_in_current_path)
				{
					console::warn("No valid language file found for '%s' in '%s/localizedstrings/', falling back to 'english'\n",
						language, path.data());
				}
				else
				{
					found = true;
				}
			}

			if (!found)
			{
				console::warn("[Localized strings] No valid language file found!\n");
			}
		}
	}

	void override(const std::string& key, const std::string& value, bool volatile_)
	{
		localized_overrides.access([&](localized_map& map)
		{
			map[key] = {value, volatile_};
		});
	}

	void clear()
	{
		localized_overrides.access([&](localized_map& map)
		{
			for (auto i = map.begin(); i != map.end();)
			{
				if (i->second.volatile_)
				{
					i = map.erase(i);
				}
				else
				{
					++i;
				}
			}
		});

		load_localized_strings();
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Change some localized strings
			seh_string_ed_get_string_hook.create(SELECT_VALUE(0x3E6CE0_b, 0x585DA0_b), &seh_string_ed_get_string);
			utils::hook::call(SELECT_VALUE(0x3E67C9_b, 0x585889_b), db_find_localize_entry_stub);
		}
	};
}

REGISTER_COMPONENT(localized_strings::component)