#pragma once
#include "loader/component_loader.hpp"

#include <utils/http.hpp>

class motd final : public component_interface
{
public:
	void post_load() override;

	using featured_content_t = std::unordered_map<std::string, utils::http::result>;

	static featured_content_t& get_featured_content();

private:
	static void fetch_featured_content(const std::string& content_name, const int content_index);
	static bool handle_featured_content();
};
