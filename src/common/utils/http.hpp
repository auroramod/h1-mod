#pragma once

#include <string>
#include <optional>
#include <future>
#include <gsl/gsl>

#include <curl/curl.h>

namespace utils::http
{
	struct result
	{
		CURLcode code;
		std::string buffer;
	};

	std::optional<result> get_data(const std::string& url);
	std::future<std::optional<result>> get_data_async(const std::string& url);
}
