#include <std_include.hpp>
#include "functions.hpp"

#include "../../component/console.hpp"
#include "../../component/gsc.hpp"

#include <xsk/gsc/types.hpp>
#include <xsk/resolver.hpp>
#include <xsk/utils/compression.hpp>

#include <utils/string.hpp>

namespace scripting
{
	namespace
	{
		int find_function_index(const std::string& name, const bool prefer_global)
		{
			const auto target = utils::string::to_lower(name);

			// could probably be cleaned up, but it works for now
			if (prefer_global)
			{
				const auto function_entry = xsk::gsc::h1::resolver::function_id(target);
				if (function_entry)
				{
					return function_entry;
				}

				const auto method_entry = xsk::gsc::h1::resolver::method_id(target);
				if (method_entry)
				{
					return method_entry;
				}
			}
			else
			{
				const auto method_entry = xsk::gsc::h1::resolver::method_id(target);
				if (method_entry)
				{
					return method_entry;
				}

				const auto function_entry = xsk::gsc::h1::resolver::function_id(target);
				if (function_entry)
				{
					return function_entry;
				}
			}

			return -1;
		}

		script_function get_function_by_index(const unsigned index)
		{
			static const auto function_table = &gsc::func_table;
			static const auto method_table = SELECT_VALUE(0xB8CDD60_b, 0xAC85070_b);

			if (index < 0x30A)
			{
				return reinterpret_cast<script_function*>(function_table)[index - 1];
			}

			return reinterpret_cast<script_function*>(method_table)[index - 0x8000];
		}

		unsigned int parse_token_id(const std::string& name)
		{
			if (name.starts_with("_ID"))
			{
				return static_cast<unsigned int>(std::strtol(name.substr(3).data(), nullptr, 10));
			}

			if (name.starts_with("_id_"))
			{
				return static_cast<unsigned int>(std::strtol(name.substr(4).data(), nullptr, 16));
			}

			return 0;
		}
	}

	std::vector<std::string> find_token(unsigned int id)
	{
		std::vector<std::string> results;

		results.push_back(utils::string::va("_ID%i", id)); // pretty sure fed told me this was here for old gsc-tool tokens or sum, can we just remove this? -mikey

		const auto result = xsk::gsc::h1::resolver::token_name(static_cast<std::uint16_t>(id)); // will return _id_%04X version if not found
		results.push_back(result);

		return results;
	}

	unsigned int find_token_id(const std::string& name)
	{
		const auto result = xsk::gsc::h1::resolver::token_id(name);
		if (result)
		{
			return result;
		}

		const auto parsed_id = parse_token_id(name);
		if (parsed_id)
		{
			return parsed_id;
		}

		return game::SL_GetCanonicalString(name.data());
	}

	script_function find_function(const std::string& name, const bool prefer_global)
	{
		const auto index = find_function_index(name, prefer_global);
		if (index < 0)
		{
			return nullptr;
		}

		return get_function_by_index(index);
	}
}
