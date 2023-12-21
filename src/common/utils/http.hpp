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
		CURLcode code{};
		unsigned int response_code{};
		std::string buffer{};
	};

	using headers = std::unordered_map<std::string, std::string>;

	std::optional<result> get_data(const std::string& url, const std::string& fields = {},
		const headers& headers = {}, const std::function<int(size_t, size_t)>& callback = {}, int timeout = 0);
	std::future<std::optional<result>> get_data_async(const std::string& url, const std::string& fields = {},
		const headers& headers = {}, const std::function<int(size_t, size_t)>& callback = {});
}
