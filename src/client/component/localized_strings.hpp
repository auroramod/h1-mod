#pragma once
#include "loader/component_loader.hpp"

class localized_strings final : public component_interface
{
public:
	void post_unpack() override;

	static void override(const std::string& key, const std::string& value);

private:
	static const char* seh_string_ed_get_string(const char* reference);
};
