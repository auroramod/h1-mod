#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class movement final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void pm_air_accelerate(game::vec3_t wishdir, float wishspeed, game::playerState_s* ps, game::pml_t* pml);
	static void pm_clip_velocity(game::vec3_t in, game::vec3_t normal, game::vec3_t out, float overbounce);
	static void pm_try_playermove(game::pmove_t* pm, game::pml_t* pml);
	static void pm_airmove_stub(game::pmove_t* pm, game::pml_t* pml);
};
