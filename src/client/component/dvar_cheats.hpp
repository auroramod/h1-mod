#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class dvar_cheats final : public component_interface
{
public:
	void post_unpack() override;

	static bool dvar_flag_checks(const game::dvar_t* dvar, game::DvarSetSource source, bool silent = false);

private:
	static void apply_sv_cheats(const game::dvar_t* dvar, game::DvarSetSource source, game::dvar_value* value);
	static void* get_dvar_flag_checks_stub();
};
