#include "http.hpp"
#include "nt.hpp"
#include <atlcomcli.h>

namespace utils::http
{
	namespace
	{
		size_t write_callback(void* contents, const size_t size, const size_t nmemb, void* userp)
		{
			auto* buffer = static_cast<std::string*>(userp);

			const auto total_size = size * nmemb;
			buffer->append(static_cast<char*>(contents), total_size);
			return total_size;
		}
	}

	std::optional<result> get_data(const std::string& url)
	{
		curl_slist* header_list = nullptr;
		auto* curl = curl_easy_init();
		if (!curl)
		{
			return {};
		}

		auto _ = gsl::finally([&]()
		{
			curl_slist_free_all(header_list);
			curl_easy_cleanup(curl);
		});

		std::string buffer{};

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);

		const auto code = curl_easy_perform(curl);

		if (code == CURLE_OK)
		{
			result result;
			result.code = code;
			result.buffer = std::move(buffer);

			return result;
		}
		else
		{
			result result;
			result.code = code;

			return result;
		}
	}

	std::future<std::optional<result>> get_data_async(const std::string& url)
	{
		return std::async(std::launch::async, [url]()
		{
			return get_data(url);
		});
	}
}
