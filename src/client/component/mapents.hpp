#pragma once
#include "loader/component_loader.hpp"

class mapents final : public component_interface
{
public:
	void post_unpack() override;

private:
	static std::optional<std::string> parse_mapents(const std::string& source);
	static bool load_raw_mapents();
	static const char* cm_entity_string_stub();
	static void cm_unload_stub(void* clip_map);
};
