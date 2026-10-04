#include <std_include.hpp>
#include "functions.hpp"

#include <utils/string.hpp>

#include "component/gsc/script_extension.hpp"
#include "component/gsc/script_loading.hpp"

namespace scripting
{
	namespace
	{
		int find_function_index(const std::string& name, [[maybe_unused]] const bool prefer_global)
		{
			const auto target = utils::string::to_lower(name);
			auto const& first = gsc::gsc_ctx->func_map();
			auto const& second = gsc::gsc_ctx->meth_map();

			if (!prefer_global)
			{
				if (const auto itr = second.find(name); itr != second.end())
				{
					return static_cast<int>(itr->second);
				}

				if (const auto itr = first.find(name); itr != first.end())
				{
					return static_cast<int>(itr->second);
				}
			}

			if (const auto itr = first.find(name); itr != first.end())
			{
				return static_cast<int>(itr->second);
			}

			if (const auto itr = second.find(name); itr != second.end())
			{
				return static_cast<int>(itr->second);
			}

			return -1;
		}
	}

	std::uint32_t parse_token_id(const std::string& name)
	{
		if (name.starts_with("_ID"))
		{
			return static_cast<std::uint32_t>(std::strtol(name.substr(3).data(), nullptr, 10));
		}

		if (name.starts_with("_id_"))
		{
			return static_cast<std::uint32_t>(std::strtol(name.substr(4).data(), nullptr, 16));
		}

		return 0;
	}

	std::string find_token(std::uint32_t id)
	{
		return gsc::gsc_ctx->token_name(id);
	}

	std::string find_token_single(std::uint32_t id)
	{
		return gsc::gsc_ctx->token_name(id);
	}

	unsigned int find_token_id(const std::string& name)
	{
		const auto id = gsc::gsc_ctx->token_id(name);
		if (id)
		{
			return id;
		}

		const auto parsed_id = parse_token_id(name);
		if (parsed_id)
		{
			return parsed_id;
		}

		return game::SL_GetCanonicalString(name.data());
	}

	script_function get_function_by_index(const std::uint32_t index)
	{
		static const auto function_table = &gsc::func_table;
		static const auto method_table = &gsc::meth_table;

		if (index < 0x1000)
		{
			return reinterpret_cast<script_function*>(function_table)[index - 1];
		}

		return reinterpret_cast<script_function*>(method_table)[index - 0x8000];
	}

	script_function find_function(const std::string& name, const bool prefer_global)
	{
		const auto index = find_function_index(name, prefer_global);
		if (index < 0) return nullptr;

		return get_function_by_index(index);
	}
}