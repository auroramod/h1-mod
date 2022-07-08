#include "flags.hpp"
#include "string.hpp"
#include "nt.hpp"

#include <shellapi.h>

namespace utils::flags
{
	void parse_flags(std::vector<std::string>& flags)
	{
		int num_args;
		auto* const argv = CommandLineToArgvW(GetCommandLineW(), &num_args);

		flags.clear();

		if (argv)
		{
			for (auto i = 0; i < num_args; ++i)
			{
				std::wstring wide_flag(argv[i]);
				if (wide_flag[0] == L'-')
				{
					wide_flag.erase(wide_flag.begin());
					const auto flag = string::convert(wide_flag);
					flags.emplace_back(string::to_lower(flag));
				}
			}

			LocalFree(argv);
		}
	}

	bool has_flag(const std::string& flag)
	{
		static auto parsed = false;
		static std::vector<std::string> enabled_flags;

		if (!parsed)
		{
			parse_flags(enabled_flags);
			parsed = true;
		}

		return std::ranges::any_of(enabled_flags.cbegin(), enabled_flags.cend(),
			[flag](const auto& elem) { return elem == string::to_lower(flag); });
	}
}
