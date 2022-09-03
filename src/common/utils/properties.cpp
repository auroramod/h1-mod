#include "io.hpp"
#include "properties.hpp"
#include <gsl/gsl>
#include <ShlObj.h>

namespace utils::properties
{
	std::filesystem::path get_appdata_path()
	{
		PWSTR path;
		if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path)))
		{
			throw std::runtime_error("Failed to read APPDATA path!");
		}

		auto _ = gsl::finally([&path]
		{
			CoTaskMemFree(path);
		});

		static auto appdata = std::filesystem::path(path) / "h2-mod";
		return appdata;
	}
}
