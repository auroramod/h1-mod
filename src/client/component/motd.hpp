#pragma once

#include <utils/http.hpp>

namespace motd
{
	using featured_content_t = std::unordered_map<std::string, utils::http::result>;

	featured_content_t& get_featured_content();
}
