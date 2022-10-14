#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

#include <xsk/gsc/types.hpp>
#include <xsk/gsc/interfaces/compiler.hpp>
#include <xsk/gsc/interfaces/decompiler.hpp>
#include <xsk/gsc/interfaces/assembler.hpp>
#include <xsk/gsc/interfaces/disassembler.hpp>
#include <xsk/resolver.hpp>
#include <interface.hpp>

namespace mapents
{
	namespace
	{
		std::optional<std::string> parse_mapents(const std::string& source)
		{
			std::string out_buffer{};

			const auto lines = utils::string::split(source, '\n');
			auto in_map_ent = false;
			auto empty = false;
			auto in_comment = false;

			for (auto i = 0; i < lines.size(); i++)
			{
				auto line = lines[i];
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				if (line.starts_with("/*"))
				{
					in_comment = true;
					continue;
				}

				if (line.ends_with("*/"))
				{
					in_comment = false;
					continue;
				}

				if (in_comment)
				{
					continue;
				}

				if (line.starts_with("//"))
				{
					continue;
				}

				if (line[0] == '{' && !in_map_ent)
				{
					in_map_ent = true;
					out_buffer.append("{\n");
					continue;
				}

				if (line[0] == '{' && in_map_ent)
				{
					console::error("[map_ents parser] Unexpected '{' on line %i\n", i);
					return {};
				}

				if (line[0] == '}' && in_map_ent)
				{
					if (empty)
					{
						out_buffer.append("\n}\n");
					}
					else if (i < static_cast<int>(lines.size()) - 1)
					{
						out_buffer.append("}\n");
					}
					else
					{
						out_buffer.append("}\0");
					}

					in_map_ent = false;
					continue;
				}

				if (line[0] == '}' && !in_map_ent)
				{
					console::error("[map_ents parser] Unexpected '}' on line %i\n", i);
					return {};
				}

				std::regex expr(R"~((.+) "(.*)")~");
				std::smatch match{};
				if (!std::regex_search(line, match, expr))
				{
					console::warn("[map_ents parser] Failed to parse line %i (%s)\n", i, line.data());
					continue;
				}

				auto key = utils::string::to_lower(match[1].str());
				const auto value = match[2].str();

				if (key.size() <= 0)
				{
					console::warn("[map_ents parser] Invalid key ('%s') on line %i (%s)\n", key.data(), i, line.data());
					continue;
				}

				if (value.size() <= 0)
				{
					continue;
				}

				empty = false;

				if (utils::string::is_numeric(key) || key.size() < 3 || !key.starts_with("\"") || !key.ends_with("\""))
				{
					out_buffer.append(line);
					out_buffer.append("\n");
					continue;
				}

				const auto key_ = key.substr(1, key.size() - 2);
				const auto id = xsk::gsc::h1::resolver::token_id(key_);
				if (id == 0)
				{
					console::warn("[map_ents parser] Key '%s' not found, on line %i (%s)\n", key_.data(), i, line.data());
					continue;
				}

				out_buffer.append(utils::string::va("%i \"%s\"\n", id, value.data()));
			}

			return {out_buffer};
		}

		std::string entity_string;
		const char* cm_entity_string_stub()
		{
			const auto original = utils::hook::invoke<const char*>(0x4CD140_b);
			const auto parsed = parse_mapents(original);
			if (parsed.has_value())
			{
				entity_string = parsed.value();
				return entity_string.data();
			}
			else
			{
				return original;
			}
		}

		void cm_unload_stub(void* clip_map)
		{
			entity_string.clear();
			utils::hook::invoke<void>(0x4CD0E0_b, clip_map);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::call(0x41F594_b, cm_entity_string_stub);
			utils::hook::call(0x399814_b, cm_unload_stub);
		}
	};
}

REGISTER_COMPONENT(mapents::component)
