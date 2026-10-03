#pragma once
#include "loader/component_loader.hpp"

class bots final : public component_interface
{
public:
	void post_unpack() override;

private:
	static bool can_add();
	static void join_team(const int entity_num);
	static void spawn(const int entity_num);
	static void add();

	static void load_bot_data();
	static const char* get_random_bot_name();
};
