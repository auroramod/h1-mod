#include "http.hpp"
#include "nt.hpp"
#include <atlcomcli.h>

namespace utils::http
{
	namespace
	{
		struct progress_helper
		{
			const std::function<int(size_t, size_t)>* callback{};
			std::exception_ptr exception{};
		};

		int progress_callback(void* clientp, const curl_off_t dltotal, const curl_off_t dlnow, const curl_off_t /*ultotal*/, const curl_off_t /*ulnow*/)
		{
			auto* helper = static_cast<progress_helper*>(clientp);

			try
			{
				if (*helper->callback && (*helper->callback)(dltotal, dlnow) == -1)
				{
					return -1;
				}
			}
			catch (...)
			{
				helper->exception = std::current_exception();
				return -1;
			}

			return 0;
		}

		size_t write_callback(void* contents, const size_t size, const size_t nmemb, void* userp)
		{
			auto* buffer = static_cast<std::string*>(userp);

			const auto total_size = size * nmemb;
			buffer->append(static_cast<char*>(contents), total_size);
			return total_size;
		}
	}

	std::optional<result> get_data(const std::string& url, const std::string& fields,
		const headers& headers, const std::function<int(size_t, size_t)>& callback, int timeout)
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

		for (const auto& header : headers)
		{
			auto data = header.first + ": " + header.second;
			header_list = curl_slist_append(header_list, data.data());
		}

		std::string buffer{};
		progress_helper helper{};
		helper.callback = &callback;

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &helper);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

		if (!fields.empty())
		{
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields.data());
		}

		const auto code = curl_easy_perform(curl);
		unsigned int response_code{};
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

		if (code == CURLE_OK)
		{
			result result;
			result.code = code;
			result.response_code = response_code;
			result.buffer = std::move(buffer);

			return result;
		}

		if (helper.exception)
		{
			std::rethrow_exception(helper.exception);
		}

		result result;
		result.code = code;

		return result;
	}

	std::future<std::optional<result>> get_data_async(const std::string& url, const std::string& fields,
		const headers& headers, const std::function<int(size_t, size_t)>& callback)
	{
		return std::async(std::launch::async, [url, fields, headers, callback]()
		{
			return get_data(url, fields, headers, callback);
		});
	}
}
