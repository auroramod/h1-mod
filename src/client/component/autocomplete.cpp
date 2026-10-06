#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "autocomplete.hpp"
#include "fastfiles.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/string.hpp>

namespace autocomplete
{
	namespace
	{
		constexpr std::size_t max_history = 64;

		const std::unordered_set<std::string> set_commands = {"set", "seta", "sets", "setu"};
		const std::unordered_set<std::string> dvar_commands = {"toggle", "togglep", "reset", "setfromdvar"};
		const std::unordered_set<std::string> list_hidden_commands = {"map", "devmap", "ui_mapname"};

		struct context
		{
			std::string command;
			std::vector<std::string> args;
			std::size_t arg_index{};
		};

		struct cached_list
		{
			std::mutex mutex;
			std::vector<match> items;
			std::chrono::steady_clock::time_point updated{};
			bool pending{};
		};
		using provider = std::function<std::vector<match>(const context&)>;

		struct cycle_state
		{
			std::string output;
			result base;
		};

		std::unordered_map<std::string, provider> providers;
		std::atomic<std::uint32_t> generation{};

		std::mutex cycle_mutex;
		cycle_state cycle{};

		std::mutex history_mutex;
		std::deque<std::string> history;

		cached_list maps_cache;
		cached_list gametypes_cache;
		cached_list weapons_cache;

		bool is_separator(const char c)
		{
			return c == ' ' || c == '\t';
		}

		std::size_t find_segment_start(const std::string& input)
		{
			auto in_quotes = false;
			std::size_t start = 0;

			for (std::size_t i = 0; i < input.size(); i++)
			{
				if (input[i] == '"')
				{
					in_quotes = !in_quotes;
				}
				else if (input[i] == ';' && !in_quotes)
				{
					start = i + 1;
				}
			}

			return start;
		}

		std::vector<std::string> split_words(const std::string& text)
		{
			std::vector<std::string> words;
			std::string current;

			for (const auto c : text)
			{
				if (is_separator(c))
				{
					if (!current.empty())
					{
						words.emplace_back(std::move(current));
						current.clear();
					}
				}
				else
				{
					current.push_back(c);
				}
			}

			if (!current.empty())
			{
				words.emplace_back(std::move(current));
			}

			return words;
		}

		std::string quote_if_needed(const std::string& value)
		{
			if (value.empty() || value.find(' ') != std::string::npos)
			{
				return "\"" + value + "\"";
			}

			return value;
		}

		std::string dvar_value(game::dvar_t* dvar, const game::dvar_value& value)
		{
			const auto* str = game::Dvar_ValueToString(dvar, true, value);
			return str ? str : "";
		}

		std::vector<match> get_cached(cached_list& list, const std::chrono::milliseconds max_age,
			const scheduler::pipeline pipeline, std::vector<match>(*refresh)())
		{
			std::lock_guard _(list.mutex);

			const auto now = std::chrono::steady_clock::now();
			if (!list.pending && (list.updated == std::chrono::steady_clock::time_point{} || now - list.updated > max_age))
			{
				list.pending = true;
				scheduler::once([&list, refresh]
				{
					std::vector<match> items{};
					try
					{
						items = refresh();
					}
					catch (...)
					{
					}

					std::lock_guard lock(list.mutex);
					list.items = std::move(items);
					list.updated = std::chrono::steady_clock::now();
					list.pending = false;
					++generation;
				}, pipeline);
			}

			return list.items;
		}

		std::vector<match> get_dvar_names()
		{
			std::vector<match> names;
			names.reserve(dvars::dvar_map.size());

			for (const auto& [hash, info] : dvars::dvar_map)
			{
				if (!info.name.empty() && game::Dvar_FindMalleableVar(hash))
				{
					names.emplace_back(info.name, "", match_type::dvar);
				}
			}

			return names;
		}

		std::vector<match> get_dvar_values(game::dvar_t* dvar)
		{
			std::vector<match> values;

			const auto current = dvar_value(dvar, dvar->current);
			const auto reset = dvar_value(dvar, dvar->reset);

			const auto describe = [&](const std::string& value, const char* fallback = "")
			{
				if (value == current && value == reset) return "current, default";
				if (value == current) return "current";
				if (value == reset) return "default";
				return fallback;
			};

			switch (dvar->type)
			{
			case game::dvar_type::boolean:
			case game::dvar_type::boolean_hashed:
				if (dvar->current.enabled)
				{
					values.emplace_back("0", describe("0", "false"), match_type::argument);
					values.emplace_back("1", describe("1", "true"), match_type::argument);
				}
				else
				{
					values.emplace_back("1", describe("1", "true"), match_type::argument);
					values.emplace_back("0", describe("0", "false"), match_type::argument);
				}
				break;
			case game::dvar_type::enumeration:
				for (auto i = 0; i < dvar->domain.enumeration.stringCount; i++)
				{
					const auto* value = dvar->domain.enumeration.strings[i];
					if (value)
					{
						values.emplace_back(quote_if_needed(value), describe(value), match_type::argument);
					}
				}
				break;
			default:
				values.emplace_back(quote_if_needed(current), current == reset ? "current, default" : "current", match_type::argument);
				if (current != reset)
				{
					values.emplace_back(quote_if_needed(reset), "default", match_type::argument);
				}
				break;
			}

			return values;
		}

		std::vector<match> refresh_maps()
		{
			std::vector<match> maps;
			std::error_code ec;

			for (auto* map = fastfiles::get_maps(); map->unk; ++map)
			{
				maps.emplace_back(map->name, "", match_type::argument);
			}

			for (const auto& entry : std::filesystem::directory_iterator("usermaps", ec))
			{
				const auto name = utils::string::to_lower(entry.path().filename().string());
				if (entry.is_directory(ec) && std::filesystem::exists(entry.path() / (name + ".ff"), ec) &&
					std::ranges::none_of(maps, [&](const match& map) { return map.name == name; }))
				{
					maps.emplace_back(name, "usermap", match_type::argument);
				}
			}

			return maps;
		}

		std::vector<match> refresh_gametypes()
		{
			std::vector<match> gametypes;

			fastfiles::enum_assets(game::ASSET_TYPE_SCRIPTFILE, [&](const game::XAssetHeader header)
			{
				const auto* name = header.scriptfile ? header.scriptfile->name : nullptr;
				if (!name || !*name)
				{
					return;
				}

				const std::string_view path = name;
				constexpr std::string_view folder = "mp/gametypes/";
				const auto pos = path.find(folder);
				if (pos == std::string_view::npos)
				{
					return;
				}

				const auto gametype = path.substr(pos + folder.size());
				if (gametype.empty() || gametype[0] == '_' || gametype.find('/') != std::string_view::npos)
				{
					return;
				}

				if (std::ranges::none_of(gametypes, [&](const match& m) { return m.name == gametype; }))
				{
					gametypes.emplace_back(std::string(gametype), "", match_type::argument);
				}
			}, true);

			std::ranges::sort(gametypes, {}, &match::name);
			return gametypes;
		}

		std::vector<match> refresh_weapons()
		{
			std::vector<match> weapons;

			fastfiles::enum_assets(game::ASSET_TYPE_WEAPON, [&](const game::XAssetHeader header)
			{
				const game::XAsset asset{game::ASSET_TYPE_WEAPON, header};
				const auto* name = game::DB_GetXAssetName(&asset);
				if (name && *name)
				{
					weapons.emplace_back(name, "", match_type::argument);
				}
			}, true);

			std::ranges::sort(weapons, {}, &match::name);
			return weapons;
		}

		std::vector<match> get_maps()
		{
			return get_cached(maps_cache, 30s, scheduler::pipeline::async, refresh_maps);
		}

		std::vector<match> get_gametypes()
		{
			return get_cached(gametypes_cache, 10s, scheduler::pipeline::main, refresh_gametypes);
		}

		std::vector<match> get_weapons()
		{
			return get_cached(weapons_cache, 5s, scheduler::pipeline::main, refresh_weapons);
		}

		std::vector<match> get_keys()
		{
			std::vector<match> keys;
			std::unordered_set<std::string> seen;

			for (auto i = 0; i < game::keyNum_t::K_LAST_KEY; i++)
			{
				const auto* name = game::Key_KeynumToString(i, 0, 1);
				if (!name || !*name || !strncmp(name, "0x", 2) || seen.contains(name))
				{
					continue;
				}

				seen.insert(name);
				keys.emplace_back(name, "", match_type::argument);
			}

			return keys;
		}

		std::vector<match> get_dvar_arguments(const std::string& dvar_name, game::dvar_t* dvar)
		{
			if (dvar_name == "g_gametype" || dvar_name == "ui_gametype")
			{
				return get_gametypes();
			}

			if (dvar_name == "ui_mapname")
			{
				return get_maps();
			}

			return get_dvar_values(dvar);
		}

		std::vector<match> get_arguments(const context& ctx)
		{
			if (const auto itr = providers.find(ctx.command); itr != providers.end())
			{
				return itr->second(ctx);
			}

			if (ctx.arg_index == 1)
			{
				if (auto* dvar = game::Dvar_FindVar(ctx.command.data()))
				{
					return get_dvar_arguments(ctx.command, dvar);
				}
			}

			return {};
		}

		std::vector<match> filter(std::vector<match> candidates, const std::string& token,
			const bool keep_order)
		{
			const auto lower_token = utils::string::to_lower(token);

			std::vector<match> prefix;
			std::vector<match> contains;

			for (auto& candidate : candidates)
			{
				const auto lower_name = utils::string::to_lower(candidate.name);
				const auto pos = lower_name.find(lower_token);

				if (pos == 0)
				{
					prefix.emplace_back(std::move(candidate));
				}
				else if (pos != std::string::npos)
				{
					contains.emplace_back(std::move(candidate));
				}
			}

			if (!keep_order)
			{
				const auto by_name = [](const match& a, const match& b)
				{
					return _stricmp(a.name.data(), b.name.data()) < 0;
				};

				std::ranges::sort(prefix, by_name);
				std::ranges::sort(contains, by_name);
			}

			prefix.insert(prefix.end(), std::make_move_iterator(contains.begin()), std::make_move_iterator(contains.end()));

			// resolve dvar values only for the survivors, converting every dvar each keystroke is expensive
			for (auto& m : prefix)
			{
				if (m.type == match_type::dvar && m.description.empty())
				{
					if (auto* dvar = game::Dvar_FindVar(m.name.data()))
					{
						m.description = dvar_value(dvar, dvar->current);
					}
				}
			}

			return prefix;
		}

		std::vector<match> find_commands(const std::string& token)
		{
			auto candidates = get_dvar_names();

			for (auto* cmd = *game::cmd_functions; cmd; cmd = cmd->next)
			{
				if (cmd->name && *cmd->name)
				{
					candidates.emplace_back(cmd->name, "", match_type::command);
				}
			}

			return filter(std::move(candidates), token, false);
		}

		result build_result(const std::string& input)
		{
			result result{};

			const auto segment_start = find_segment_start(input);
			auto command_start = segment_start;

			while (command_start < input.size() && is_separator(input[command_start]))
			{
				command_start++;
			}

			if (command_start < input.size() && (input[command_start] == '/' || input[command_start] == '\\'))
			{
				command_start++;
			}

			const auto segment = input.substr(command_start);
			auto words = split_words(segment);

			const auto ends_with_space = !segment.empty() && is_separator(segment.back());
			if (ends_with_space || words.empty())
			{
				result.token.clear();
			}
			else
			{
				result.token = words.back();
				words.pop_back();
			}

			result.head = input.substr(0, input.size() - result.token.size());
			result.arg_index = words.size();

			if (result.arg_index == 0)
			{
				if (!result.token.empty())
				{
					result.command = result.token;
					result.matches = find_commands(result.token);
				}

				return result;
			}

			result.command = utils::string::to_lower(words.front());

			context ctx{};
			ctx.command = result.command;
			ctx.args.assign(words.begin() + 1, words.end());
			ctx.arg_index = result.arg_index;

			result.matches = filter(get_arguments(ctx), result.token, true);
			result.show_list = !list_hidden_commands.contains(result.command);
			return result;
		}

		bool should_append_space(const result& result)
		{
			if (result.arg_index == 0)
			{
				return true;
			}

			return result.arg_index == 1 && (set_commands.contains(result.command) || result.command == "setfromdvar" ||
				result.command == "bind");
		}

		void register_providers()
		{
			const auto dvar_name_provider = [](const context& ctx) -> std::vector<match>
			{
				if (ctx.arg_index == 1 || (ctx.command == "setfromdvar" && ctx.arg_index == 2))
				{
					return get_dvar_names();
				}

				if (ctx.arg_index == 2 && set_commands.contains(ctx.command))
				{
					const auto dvar_name = utils::string::to_lower(ctx.args[0]);
					if (auto* dvar = game::Dvar_FindVar(dvar_name.data()))
					{
						return get_dvar_arguments(dvar_name, dvar);
					}
				}

				return {};
			};

			for (const auto& command : set_commands)
			{
				providers[command] = dvar_name_provider;
			}

			for (const auto& command : dvar_commands)
			{
				providers[command] = dvar_name_provider;
			}

			const auto map_provider = [](const context& ctx) -> std::vector<match>
			{
				return ctx.arg_index == 1 ? get_maps() : std::vector<match>{};
			};

			providers["map"] = map_provider;
			providers["devmap"] = map_provider;

			providers["give"] = [](const context& ctx) -> std::vector<match>
			{
				return ctx.arg_index == 1 ? get_weapons() : std::vector<match>{};
			};

			providers["take"] = providers["give"];

			providers["listassetpool"] = [](const context& ctx) -> std::vector<match>
			{
				std::vector<match> pools;
				if (ctx.arg_index != 1)
				{
					return pools;
				}

				for (auto i = 0; i < game::ASSET_TYPE_COUNT; i++)
				{
					const auto* name = game::g_assetNames[i];
					pools.emplace_back(std::to_string(i), name ? name : "", match_type::argument);
				}

				return pools;
			};

			const auto key_provider = [](const context& ctx) -> std::vector<match>
			{
				return ctx.arg_index == 1 ? get_keys() : std::vector<match>{};
			};

			providers["bind"] = key_provider;
			providers["unbind"] = key_provider;
		}
	}

	result query(const std::string& input)
	{
		{
			std::lock_guard _(cycle_mutex);
			if (!cycle.output.empty() && input == cycle.output)
			{
				return cycle.base;
			}
		}

		return build_result(input);
	}

	std::string get_ghost_text(const result& result)
	{
		if (result.selected >= 0 || result.matches.empty() || (result.token.empty() && result.arg_index == 0))
		{
			return {};
		}

		const auto& best = result.matches.front().name;
		if (best.size() <= result.token.size() || _strnicmp(best.data(), result.token.data(), result.token.size()) != 0)
		{
			return {};
		}

		return best.substr(result.token.size());
	}

	std::string complete(const std::string& input, const bool reverse)
	{
		std::lock_guard _(cycle_mutex);

		if (!cycle.output.empty() && input == cycle.output && !cycle.base.matches.empty())
		{
			const auto count = static_cast<int>(cycle.base.matches.size());
			cycle.base.selected = (cycle.base.selected + (reverse ? -1 : 1) + count) % count;
			cycle.output = cycle.base.head + cycle.base.matches[cycle.base.selected].name;
			return cycle.output;
		}

		cycle = {};

		auto base = build_result(input);
		if (base.matches.empty())
		{
			return input;
		}

		if (base.matches.size() == 1)
		{
			auto output = base.head + base.matches.front().name;
			if (should_append_space(base))
			{
				output.push_back(' ');
			}

			return output;
		}

		base.selected = reverse ? static_cast<int>(base.matches.size()) - 1 : 0;
		cycle.output = base.head + base.matches[base.selected].name;
		cycle.base = std::move(base);

		return cycle.output;
	}

	std::string prepare_command(const std::string& input)
	{
		auto start = input.find_first_not_of(" \t");
		if (start == std::string::npos)
		{
			return {};
		}

		if (input[start] == '/' || input[start] == '\\')
		{
			start++;
		}

		const auto command = input.substr(start);
		if (command.find('"') != std::string::npos)
		{
			return command;
		}

		const auto to_bool_value = [](const std::string& value) -> const char*
		{
			const auto lower = utils::string::to_lower(value);
			if (lower == "true" || lower == "on" || lower == "yes") return "1";
			if (lower == "false" || lower == "off" || lower == "no") return "0";
			return nullptr;
		};

		const auto is_bool_dvar = [](const std::string& name)
		{
			const auto* dvar = game::Dvar_FindVar(name.data());
			return dvar && (dvar->type == game::dvar_type::boolean || dvar->type == game::dvar_type::boolean_hashed);
		};

		std::string output;
		for (const auto& segment : utils::string::split(command, ';'))
		{
			if (!output.empty())
			{
				output.push_back(';');
			}

			auto words = split_words(segment);
			auto changed = false;

			std::size_t value_index = 1;
			if (words.size() >= 3 && set_commands.contains(utils::string::to_lower(words[0])))
			{
				value_index = 2;
			}

			if (words.size() == value_index + 1 && is_bool_dvar(words[value_index - 1]))
			{
				if (const auto* value = to_bool_value(words[value_index]))
				{
					words[value_index] = value;
					changed = true;
				}
			}

			if (!changed)
			{
				output.append(segment);
				continue;
			}

			for (std::size_t i = 0; i < words.size(); i++)
			{
				output.append(i ? " " : "").append(words[i]);
			}
		}

		return output;
	}

	std::uint32_t get_generation()
	{
		return generation;
	}

	void add_history(const std::string& input)
	{
		if (input.empty())
		{
			return;
		}

		std::lock_guard _(history_mutex);

		std::erase(history, input);
		history.push_front(input);

		if (history.size() > max_history)
		{
			history.pop_back();
		}
	}

	std::deque<std::string> get_history()
	{
		std::lock_guard _(history_mutex);
		return history;
	}

	void clear_history()
	{
		std::lock_guard _(history_mutex);
		history.clear();
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

			register_providers();
		}
	};
}

REGISTER_COMPONENT(autocomplete::component)
