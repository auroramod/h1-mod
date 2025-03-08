#include <std_include.hpp>

#include "mapents.hpp"

#include <utils/string.hpp>

namespace mapents
{
	void mapents_entity::add_var(const spawn_var& var)
	{
		this->vars.push_back(var);
	}

	std::string mapents_entity::get(const std::string& key) const
	{
		for (const auto& var : this->vars)
		{
			if (var.key == key)
			{
				return var.value;
			}
		}

		return "";
	}

	std::vector<spawn_var> mapents_entity::get_var_list() const
	{
		return this->vars;
	}

	void mapents_entity::clear()
	{
		this->vars.clear();
	}

	mapents_list parse(const std::string& data, const token_name_callback& get_token_name)
	{
		mapents_list list;
		mapents_entity current_entity;

		const auto lines = utils::string::split(data, '\n');
		auto in_map_ent = false;
		auto in_comment = false;

		for (auto i = 0; i < lines.size(); i++)
		{
			auto line = lines[i];
			if (line.ends_with('\r'))
			{
				line.pop_back();
			}

			if (line.starts_with("/*") || line.ends_with("/*"))
			{
				in_comment = true;
				continue;
			}

			if (line.starts_with("*/") || line.ends_with("*/"))
			{
				in_comment = false;
				continue;
			}

			if (in_comment || line.starts_with("//") || line.empty())
			{
				continue;
			}

			if (line[0] == '{' && !in_map_ent)
			{
				current_entity.clear();
				in_map_ent = true;
				continue;
			}

			if (line[0] == '{' && in_map_ent)
			{
				throw std::runtime_error(utils::string::va("Unexpected '{' on line %i", i));
			}

			if (line[0] == '}' && in_map_ent)
			{
				list.entities.push_back(current_entity);
				in_map_ent = false;
				continue;
			}

			if (line[0] == '}' && !in_map_ent)
			{
				throw std::runtime_error(utils::string::va("Unexpected '}' on line %i", i));
			}

			if (line[0] == '\n' || line[0] == '\0' || line[0] == '\n')
			{
				continue;
			}

			spawn_var var{};

			if (line.starts_with("0 \""))
			{
				std::regex expr(R"~(0 "(.+)" "(.*)")~");
				std::smatch match{};
				if (!std::regex_search(line, match, expr))
				{
					throw std::runtime_error(utils::string::va("Failed to parse line %i (%s)", i, line.data()));
					continue;
				}

				var.key = utils::string::to_lower(match[1].str());
				var.value = match[2].str();
				var.sl_string = true;
			}
			else
			{
				std::regex expr(R"~((.+) "(.*)")~");
				std::smatch match{};
				if (!std::regex_search(line, match, expr))
				{
					throw std::runtime_error(
						utils::string::va("Failed to parse line %i (%s)", i, line.data()));
					continue;
				}

				var.key = utils::string::to_lower(match[1].str());
				var.value = match[2].str();

				if (utils::string::is_numeric(var.key) && !var.key.starts_with("\"") && !var.key.ends_with("\""))
				{
					var.key = get_token_name(static_cast<std::uint32_t>(std::atoi(var.key.data())));
				}
				else if (var.key.starts_with("\"") && var.key.ends_with("\"") && var.key.size() >= 3)
				{
					var.key = var.key.substr(1, var.key.size() - 2);
				}
				else
				{
					throw std::runtime_error(
						utils::string::va("Invalid key ('%s') on line %i (%s)", var.key.data(), i, line.data()));
					continue;
				}
			}

			if (var.key.size() <= 0)
			{
				throw std::runtime_error(
					utils::string::va("Invalid key ('%s') on line %i (%s)", var.key.data(), i, line.data()));
				continue;
			}

			if (var.value.size() <= 0)
			{
				continue;
			}

			current_entity.add_var(var);
		}

		return list;
	}
}
