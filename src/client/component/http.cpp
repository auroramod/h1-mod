#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/structs.hpp"
#include "game/game.hpp"

#include "http.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "scripting.hpp"

#include <utils/http.hpp>
#include <curl/curl.h>

namespace http
{
	std::unordered_map<uint64_t, bool> active_requests{};
	uint64_t request_id{};

	unsigned int http_get(const std::string& url)
	{
		const auto id = request_id++;
		active_requests[id] = true;

		const auto object = scripting::array{};
		const auto object_id = object.get_entity_id();

		scheduler::once([id, object_id, url]()
		{
			const auto data = utils::http::get_data(url);
			scheduler::once([id, object_id, data]()
			{
				if (active_requests.find(id) == active_requests.end())
				{
					return;
				}

				if (!data.has_value())
				{
					scripting::notify(object_id, "done", {{}, false, "Unknown error"});
				}

				const auto& result = data.value();
				const auto code = result.code;
				const auto error = curl_easy_strerror(code);

				if (code != CURLE_OK)
				{
					scripting::notify(object_id, "done", {{}, false, error});
				}

				if (result.buffer.size() >= 0x5000)
				{
					console::warn("^3WARNING: HTTP result size bigger than 20480 bytes (%i), truncating!", static_cast<int>(result.buffer.size()));
				}

				scripting::notify(object_id, "done", {result.buffer.substr(0, 0x5000), true});
			}, scheduler::pipeline::server);
		}, scheduler::pipeline::async);

		return object_id;
	}

	unsigned int http_request(const std::string& url, const sol::variadic_args& va)
	{
		const auto id = request_id++;
		active_requests[id] = true;

		const auto object = scripting::array{};
		const auto object_id = object.get_entity_id();

		std::string fields_string{};
		std::unordered_map<std::string, std::string> headers_map{};

		if (va.size() > 0)
		{
			const scripting::array options = va[0].as<scripting::array>();

			const auto fields = options["parameters"];
			const auto body = options["body"];
			const auto headers = options["headers"];

			if (fields.is<scripting::array>())
			{
				const auto fields_ = fields.as<scripting::array>();
				const auto keys = fields_.get_keys();

				for (const auto& key : keys)
				{
					if (!key.is<std::string>())
					{
						continue;
					}

					const auto key_ = key.as<std::string>();
					const auto value = fields_[key].to_string();
					fields_string += key_ + "=" + value + "&";
				}
			}

			if (body.is<std::string>())
			{
				fields_string = body.as<std::string>();
			}

			if (headers.is<scripting::array>())
			{
				const auto headers_ = headers.as<scripting::array>();
				const auto keys = headers_.get_keys();

				for (const auto& key : keys)
				{
					if (!key.is<std::string>())
					{
						continue;
					}

					const auto key_ = key.as<std::string>();
					const auto value = headers_[key].to_string();

					headers_map[key_] = value;
				}
			}
		}

		scheduler::once([id, object_id, url]()
		{
			const auto data = utils::http::get_data(url);
			scheduler::once([id, object_id, data]()
			{
				if (active_requests.find(id) == active_requests.end())
				{
					return;
				}

				if (!data.has_value())
				{
					scripting::notify(object_id, "done", {{}, false, "Unknown error"});
				}

				const auto& result = data.value();
				const auto code = result.code;
				const auto error = curl_easy_strerror(code);

				if (code != CURLE_OK)
				{
					scripting::notify(object_id, "done", { {}, false, error });
				}

				if (result.buffer.size() >= 0x5000)
				{
					console::warn("^3WARNING: HTTP result size bigger than 20480 bytes (%i), truncating!", static_cast<int>(result.buffer.size()));
				}

				scripting::notify(object_id, "done", {result.buffer.substr(0, 0x5000), true});
			}, scheduler::pipeline::server);
		}, scheduler::pipeline::async);

		return object_id;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			scripting::on_shutdown([]()
			{
				active_requests.clear();
			});
		}
	};
}
