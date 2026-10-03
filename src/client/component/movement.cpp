#include <std_include.hpp>
#include "movement.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/vector.hpp>

// https://github.com/xoxor4d/iw3xo-dev/blob/develop/src/components/modules/movement.cpp :)

static utils::hook::detour pm_airmove_hook;

static game::dvar_t* pm_cs_airAccelerate;
static game::dvar_t* pm_cs_airSpeedCap;
static game::dvar_t* pm_cs_strafing;

void movement::post_unpack()
{
	if (game::environment::is_sp())
	{
		return;
	}

	pm_airmove_hook.create(0x1401E1230, pm_airmove_stub); // PM_AirMove

	pm_cs_airAccelerate = dvars::register_float("pm_cs_airAccelerate", 100.0f, 1.0f, 500.0f,
		game::DvarFlags::DVAR_CODINFO,
		"Defines player acceleration mid-air");

	pm_cs_airSpeedCap = dvars::register_float("pm_cs_airSpeedCap", 30.0f, 1.0f, 500.0f,
		game::DvarFlags::DVAR_CODINFO,
		"Maximum speed mid-air");

	pm_cs_strafing = dvars::register_bool("pm_cs_strafing", false,
		game::DvarFlags::DVAR_CODINFO,
		"Enable CS like strafing");
}

void movement::pm_air_accelerate(game::vec3_t wishdir, float wishspeed, game::playerState_s* ps, game::pml_t* pml)
{
	float wishspd = wishspeed, accelspeed, currentspeed, addspeed;

	auto accel = pm_cs_airAccelerate->current.value;
	auto airspeedcap = pm_cs_airSpeedCap->current.value;

	if (wishspd > airspeedcap)
	{
		wishspd = airspeedcap;
	}

	currentspeed = utils::vector::product(ps->velocity, wishdir);
	addspeed = wishspd - currentspeed;

	if (addspeed > 0)
	{
		accelspeed = pml->frametime * accel * wishspeed * 1.0f;

		if (accelspeed > addspeed)
		{
			accelspeed = addspeed;
		}

		for (auto i = 0; i < 3; i++)
		{
			ps->velocity[i] += wishdir[i] * accelspeed;
		}
	}
}

void movement::pm_clip_velocity(game::vec3_t in, game::vec3_t normal, game::vec3_t out, float overbounce)
{
	float backoff, change, angle, adjust;

	angle = normal[2];
	backoff = utils::vector::product(in, normal) * overbounce;

	for (auto i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
	}

	adjust = utils::vector::product(out, normal);

	if (adjust < 0)
	{
		game::vec3_t reduce{};

		utils::vector::scale(normal, adjust, reduce);
		utils::vector::subtract(out, reduce, out);
	}
}

void movement::pm_try_playermove(game::pmove_t* pm, game::pml_t* pml)
{
	const auto surf_slope = 0.7f;
	const auto ps = pm->ps;

	game::vec3_t end{};
	game::trace_t trace{};

	if (utils::vector::length(ps->velocity) == 0)
	{
		return;
	}

	utils::vector::ma(ps->origin, pml->frametime, ps->velocity, end);
	utils::hook::invoke<void>(0x1401E8BE0, pm, &trace, ps->origin, end, 
		&pm->bounds, ps->clientNum, pm->tracemask); // PM_playerTrace

	if (trace.fraction == 1)
	{
		return;
	}

	if (trace.normal[2] > surf_slope)
	{
		return;
	}

	pm_clip_velocity(ps->velocity, trace.normal, ps->velocity, 1.0f);
}

void movement::pm_airmove_stub(game::pmove_t* pm, game::pml_t* pml)
{
	if (!pm_cs_strafing->current.enabled)
	{
		return pm_airmove_hook.invoke<void>(pm, pml);
	}

	const auto ps = pm->ps;

	ps->sprintState.sprintButtonUpRequired = 1;

	float fmove{}, smove{}, wishspeed{};
	game::vec3_t wishvel{}, wishdir{};

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	pml->forward[2] = 0.0f;
	pml->right[2] = 0.0f;

	utils::vector::normalize(pml->forward);
	utils::vector::normalize(pml->right);

	for (auto i = 0; i < 2; i++)
	{
		wishvel[i] = pml->forward[i] * fmove + pml->right[i] * smove;
	}

	wishvel[2] = 0;

	utils::vector::copy(wishvel, wishdir);
	wishspeed = utils::vector::normalize(wishdir);

	if (wishspeed != 0 && (wishspeed > 320.0f))
	{
		utils::vector::scale(wishvel, 320.0f / wishspeed, wishvel);
		wishspeed = 320.0f;
	}

	pm_air_accelerate(wishdir, wishspeed, ps, pml);

	utils::hook::invoke<void>(0x1401EA9C0, pm, pml, 1, 1); // PM_StepSlideMove

	pm_try_playermove(pm, pml);
}

REGISTER_COMPONENT(movement)
