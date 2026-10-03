#pragma once
#include "loader/component_loader.hpp"

class arena final : public component_interface
{
public:
	void post_unpack() override;

private:
	static constexpr int MAX_ARENAS = 128;
	static char* s_arena_infos_[MAX_ARENAS];

	static bool parse_arena(const std::string& path);
	static void load_arenas_stub();
};
