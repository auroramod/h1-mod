#include <std_include.hpp>
#include "functions.hpp"

#include "component/console.hpp"
#include "component/gsc/script_extension.hpp"

#include "component/gsc/script_loading.hpp"

#include <utils/string.hpp>

namespace scripting
{
	namespace
	{
		const xsk::u16 func_id_wrapper(const std::string& name)
		{
			return gsc::gsc_ctx->func_id(name);
		}

		const xsk::u16 meth_id_wrapper(const std::string& name)
		{
			return gsc::gsc_ctx->meth_id(name);
		}

		int find_function_index(const std::string& name, const bool prefer_global)
		{
			const auto target = utils::string::to_lower(name);
			auto first = func_id_wrapper;
			auto second = meth_id_wrapper;
			if (!prefer_global)
			{
				std::swap(first, second);
			}

			const auto first_res = first(target);
			if (first_res)
			{
				return first_res;
			}

			const auto second_res = second(target);
			if (second_res)
			{
				return second_res;
			}

			return -1;
		}

		script_function get_function_by_index(const unsigned index)
		{
			if (index < 0x1000)
			{
				return reinterpret_cast<script_function*>(gsc::func_table)[index - 1];
			}

			return reinterpret_cast<script_function*>(gsc::meth_table)[index - 0x8000];
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

	std::string find_token(unsigned int id)
	{
		return gsc::gsc_ctx->token_name(id);
	}

	unsigned int find_token_id(const std::string& name)
	{
		const auto result = gsc::gsc_ctx->token_id(name);
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
