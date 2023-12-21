#include "flags.hpp"
#include "string.hpp"
#include "nt.hpp"

#include <shellapi.h>

namespace utils::flags
{
	namespace
	{
		bool parsed = false;

		using flag_map_t = std::unordered_map<std::string, std::optional<std::string>>;

		flag_map_t& get_flags()
		{
			static flag_map_t map = {};
			return map;
		}

		void parse_flags(flag_map_t& flags)
		{
			int num_args;
			auto* const argv = CommandLineToArgvW(GetCommandLineW(), &num_args);

			flags.clear();

			if (argv)
			{
				std::optional<std::string> last_flag{};
				for (auto i = 0; i < num_args; ++i)
				{
					std::wstring wide_flag(argv[i]);
					if (wide_flag[0] == L'-')
					{
						wide_flag.erase(wide_flag.begin());
						const auto flag = string::convert(wide_flag);

						last_flag = flag;
						flags[flag] = {};
					}
					else if (last_flag.has_value())
					{
						const auto& flag = last_flag.value();

						flags[flag] = string::convert(wide_flag);
						last_flag = {};
					}
				}

				LocalFree(argv);
			}
		}

		void check_parse_flags()
		{
			if (!parsed)
			{
				parse_flags(get_flags());
				parsed = true;
			}
		}
	}

	bool has_flag(const std::string& flag)
	{
		check_parse_flags();

		for (const auto& [name, value] : get_flags())
		{
			if (string::to_lower(name) == string::to_lower(flag))
			{
				return true;
			}
		}

		return false;
	}

	std::optional<std::string> get_flag(const std::string& flag)
	{
		check_parse_flags();

		for (const auto& [name, value] : get_flags())
		{
			if (string::to_lower(name) == string::to_lower(flag))
			{
				return value;
			}
		}

		return {};
	}

	std::optional<std::string> get_flag(const std::string& flag, const std::string& shortname)
	{
		auto value = get_flag(flag);
		if (!value.has_value())
		{
			value = get_flag(shortname);
		}
		return value;
	}

	std::string get_flag(const std::string& flag, const std::string& shortname,
		const std::string& default_)
	{
		const auto value = get_flag(flag, shortname);
		if (!value.has_value())
		{
			return default_;
		}
		return value.value();
	}
}
