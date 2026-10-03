#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

class gameplay final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void jump_apply_slowdown_stub(game::playerState_s* ps);
	static void stuck_in_client_stub(void* entity);
	static void cm_transformed_capsule_trace_stub(game::trace_t* results, const float* start, const float* end, game::Bounds* bounds, game::Bounds* capsule, int contents, const float* origin, const float* angles);
	static void pm_crashland_stub(game::playerState_s* ps, void* pml);
	static void pm_weapon_use_ammo_stub(game::playerState_s* ps, game::Weapon weapon, bool is_alternate, int amount, game::PlayerHandIndex hand);
	static void* pm_bouncing_stub_sp();
	static void* pm_bouncing_stub_mp();
	static void* g_speed_stub();
	static void* client_end_frame_stub();
	static void pm_player_trace_stub(game::pmove_t* pm, game::trace_t* trace, const float* f3, const float* f4, const game::Bounds* bounds, int a6, int a7);
	static void pm_trace_stub_sp(utils::hook::assembler& a);
	static void pm_trace_stub(utils::hook::assembler& a);
	static void client_end_frame_stub2(game::gentity_s* entity);
	static void g_damage_client_stub(game::gentity_s* targ, const game::gentity_s* inflictor, game::gentity_s* attacker, const float* dir, const float* point, int damage, int dflags, int mod, const unsigned int weapon, bool is_alternate, unsigned int hit_loc, int time_offset);
	static void g_damage_stub(game::gentity_s* targ, const game::gentity_s* inflictor, game::gentity_s* attacker, const float* dir, const float* point, int damage, int dflags, int mod, const unsigned int weapon, bool is_alternate, unsigned int hit_loc, unsigned int model_index, unsigned int part_name, int time_offset, int a15);
	static void* jump_push_off_ladder();
	static void jump_start_stub(game::pmove_t* pm, game::pml_t* pml, float /*height*/);
	static void pm_project_velocity_stub(const float* vel_in, const float* normal, float* vel_out);
	static void* pm_can_start_sprint_stub();
	static void weapon_rocket_launcher_fire_stub(utils::hook::assembler& a);
	static bool check_for_righty_tighty(game::pmove_t* pm);
	static bool check_for_wrist_twist(game::pmove_t* pm);
	static void sprint_drop(game::pmove_t* pm);
	static void sprint_raise(game::pmove_t* pm);
	static void pm_weapon_check_for_sprint_stub(game::pmove_t* pm);
	static bool pm_sprint_ending_buttons_stub(game::playerState_s* ps, int8_t forwardSpeed, int buttons);
	static void begin_weapon_change_stub(game::pmove_t* pm, game::Weapon new_weap, bool is_new_alt, bool quick, unsigned int* holdrand);
	static inline bool is_previous_anim(int anim);
	static void start_weapon_anim_stub(uint64_t local_client_num, game::Weapon weapon_idx, game::PlayerHandIndex player_hand_idx, game::weapAnimFiles_t blend_in_anim_index, game::weapAnimFiles_t blend_out_anim_index, float transition_time);
	static bool pm_sprint_start_interfering_buttons_stub(game::playerState_s* ps, int forward_speed, int buttons);
};
