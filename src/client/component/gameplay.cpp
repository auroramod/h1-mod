#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "dvars.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/nt.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace gameplay
{
	namespace
	{
		utils::hook::detour pm_weapon_use_ammo_hook;
		utils::hook::detour pm_player_trace_hook;
		utils::hook::detour pm_crashland_hook;
		utils::hook::detour jump_apply_slowdown_hook;
		utils::hook::detour stuck_in_client_hook;
		utils::hook::detour cm_transformed_capsule_trace_hook;

		utils::hook::detour client_end_frame_hook;
		utils::hook::detour g_damage_client_hook;
		utils::hook::detour g_damage_hook;
		utils::hook::detour weapon_rocket_fire_hook;

		utils::hook::detour pm_weapon_check_for_sprint_hook;
		utils::hook::detour pm_sprint_ending_buttons_hook;
		utils::hook::detour begin_weapon_change_hook;
		utils::hook::detour start_weapon_anim_hook;
		utils::hook::detour pm_sprint_start_interfering_buttons_hook;

		game::dvar_t* pm_iw4_mechanics = nullptr;
		game::dvar_t* pm_glide_on_inspect = nullptr;

		void jump_apply_slowdown_stub(game::mp::playerState_s* ps)
		{
			if (dvars::jump_slowDownEnable->current.enabled)
			{
				jump_apply_slowdown_hook.invoke<void>(ps);
			}
		}

		void stuck_in_client_stub(void* entity)
		{
			if (dvars::g_playerEjection->current.enabled)
			{
				stuck_in_client_hook.invoke<void>(entity);
			}
		}

		void cm_transformed_capsule_trace_stub(game::trace_t* results, const float* start, const float* end,
			game::Bounds* bounds, game::Bounds* capsule, int contents, const float* origin, const float* angles)
		{
			if (dvars::g_playerCollision->current.enabled)
			{
				cm_transformed_capsule_trace_hook.invoke<void>(results, start, end, 
					bounds, capsule, contents, origin, angles);
			}
		}

		void pm_crashland_stub(game::mp::playerState_s* ps, void* pml)
		{
			if (dvars::jump_enableFallDamage->current.enabled)
			{
				pm_crashland_hook.invoke<void>(ps, pml);
			}
		}

		void pm_weapon_use_ammo_stub(game::playerState_s* ps, game::Weapon weapon,
			bool is_alternate, int amount, game::PlayerHandIndex hand)
		{
			if (!dvars::player_sustainAmmo->current.enabled)
			{
				pm_weapon_use_ammo_hook.invoke<void>(ps, weapon, is_alternate, amount, hand);
			}
		}

		void* pm_bouncing_stub_mp()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				const auto no_bounce = a.newLabel();
				const auto loc_2D395D = a.newLabel();

				a.push(rax);

				a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::pm_bouncing)));
				a.mov(al, byte_ptr(rax, 0x10));
				a.cmp(byte_ptr(rbp, SELECT_VALUE(-0x5D, -0x7D)), al);

				a.pop(rax);
				a.jz(no_bounce);
				a.jmp(SELECT_VALUE(0x4A2E81_b, 0x2D39C0_b));

				a.bind(no_bounce);
				a.cmp(dword_ptr(rsp, 0x44), 0);
				a.jnz(loc_2D395D);
				a.jmp(SELECT_VALUE(0x4A2E6F_b, 0x2D39B1_b));

				a.bind(loc_2D395D);
				a.jmp(SELECT_VALUE(0x4A2F18_b, 0x2D395D_b));
			});
		}

		void* g_speed_stub()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::g_speed)));
				a.mov(eax, dword_ptr(rax, 0x10));

				// original code
				a.mov(dword_ptr(r14, 0x36), ax);
				a.movzx(eax, word_ptr(r14, 0x3A));

				a.jmp(0x4006BC_b);
			});
		}

		void* client_end_frame_stub()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.push(rax);

				a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::g_gravity)));
				a.mov(eax, dword_ptr(rax, 0x10));
				a.mov(word_ptr(rbx, 0x34), ax);

				a.pop(rax);

				// Game code hook skipped
				a.mov(eax, dword_ptr(rbx, 0x495C));
				a.mov(rdi, rcx);

				a.jmp(0x3FF822_b);
			});
		}

		void pm_player_trace_stub(game::mp::pmove_t* pm, game::trace_t* trace, const float* f3,
			const float* f4, const game::Bounds* bounds, int a6, int a7)
		{
			pm_player_trace_hook.invoke<void>(pm, trace, f3, f4, bounds, a6, a7);

			// By setting startsolid to false we allow the player to clip through solid objects above their head
			if (dvars::g_enableElevators->current.enabled)
			{
				trace->startsolid = false;
			}
		}

		void pm_trace_stub(utils::hook::assembler& a)
		{
			const auto stand = a.newLabel();
			const auto allsolid = a.newLabel();

			a.call(rsi); // Game code 

			a.push(rax);

			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::g_enableElevators)));
			a.mov(al, byte_ptr(rax, 0x10));
			a.cmp(al, 1);

			a.pop(rax);

			a.jz(stand); // Always stand up

			a.cmp(byte_ptr(rsp, 0x89), 0); // Game code trace[0].allsolid == false
			a.jnz(allsolid);

			a.bind(stand);
			a.and_(dword_ptr(SELECT_VALUE(r14, r15), 0x54), 0xFFFFFFFD);
			a.jmp(SELECT_VALUE(0x499628_b, 0x2C9F9D_b));

			a.bind(allsolid);
			a.jmp(SELECT_VALUE(0x6878D4_b, 0x2C9F9F_b));
		};

		void client_end_frame_stub2(game::mp::gentity_s* entity)
		{
			client_end_frame_hook.invoke<void>(entity);

			if ((entity->client->flags & 1)) // noclip
			{
				entity->client->ps.pm_type = 2;
			}
			else if ((entity->client->flags & 2)) // ufo
			{
				entity->client->ps.pm_type = 3;
			}
		}

		void g_damage_client_stub(game::mp::gentity_s* targ, const game::mp::gentity_s* inflictor, game::mp::gentity_s* attacker, 
			const float* dir, const float* point, int damage, int dflags, int mod, 
			const unsigned int weapon, bool is_alternate, unsigned int hit_loc, int time_offset)
		{
			if ((targ->client->flags & 1) || (targ->client->flags & 2)) // noclip, ufo
			{
				return;
			}

			g_damage_client_hook.invoke<void>(targ, inflictor, attacker, dir, point, damage, dflags, mod, 
				weapon, is_alternate, hit_loc, time_offset);
		}

		void g_damage_stub(game::mp::gentity_s* targ, const game::mp::gentity_s* inflictor, game::mp::gentity_s* attacker,
			const float* dir, const float* point, int damage, int dflags, int mod,
			const unsigned int weapon, bool is_alternate, unsigned int hit_loc,
			unsigned int model_index, unsigned int part_name, int time_offset, int a15)
		{
			if (targ->flags & 1) // godmode
			{
				return;
			}

			if (targ->flags & 2) // demigod
			{
				damage = 1;
			}

			g_damage_hook.invoke<void>(targ, inflictor, attacker, dir, point, damage, dflags, mod, weapon,
				is_alternate, hit_loc, model_index, part_name, time_offset, a15);
		}

		void* jump_push_off_ladder()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.push(rax);

				a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::jump_ladderPushVel)));
				a.mulss(xmm7, dword_ptr(rax, 0x10));
				a.mulss(xmm6, dword_ptr(rax, 0x10));

				a.pop(rax);

				a.jmp(0x2BD71C_b);
			});
		}

		void jump_start_stub(game::pmove_t* pm, game::pml_t* pml, float /*height*/)
		{
			utils::hook::invoke<void>(0x2BD800_b, pm, pml, dvars::jump_height->current.value);
		}

		void pm_project_velocity_stub(const float* vel_in, const float* normal, float* vel_out)
		{
			const auto length_squared_2d = vel_in[0] * vel_in[0] + vel_in[1] * vel_in[1];

			if (std::fabsf(normal[2]) < 0.001f || length_squared_2d == 0.0)
			{
				vel_out[0] = vel_in[0];
				vel_out[1] = vel_in[1];
				vel_out[2] = vel_in[2];
				return;
			}

			auto new_z = vel_in[0] * normal[0] + vel_in[1] * normal[1];
			new_z = -new_z / normal[2];

			const auto length_scale = std::sqrtf((vel_in[2] * vel_in[2] + length_squared_2d)
				/ (new_z * new_z + length_squared_2d));

			if (dvars::pm_bouncingAllAngles->current.enabled
				|| (length_scale < 1.f || new_z < 0.f || vel_in[2] > 0.f))
			{
				vel_out[0] = vel_in[0] * length_scale;
				vel_out[1] = vel_in[1] * length_scale;
				vel_out[2] = new_z * length_scale;
			}
		}

		void* pm_can_start_sprint_stub()
		{
			return utils::hook::assemble([=](utils::hook::assembler& a)
			{
				const auto skip_jz = a.newLabel();
				const auto loc_2C98EF = a.newLabel();

				// save rax's original value
				a.push(rax);

				// move dvar pointer to rax
				a.mov(rax, qword_ptr(reinterpret_cast<uint64_t>(&dvars::pm_sprintInAir)));

				// move *(rax + 16) into al
				a.mov(al, byte_ptr(rax, 0x10));

				// compare al with 1
				a.cmp(al, 1);

				// restore rax to its original value
				a.pop(rax);

				// jz == jump zero, jumps if the two operands in cmp are equal
				a.jz(skip_jz); // skip the last cmp & jz

				// execute original code at 0x2C98C0 & 0x2C98C6
				// necessary because our jump overwrites 12 bytes after it
				a.mov(eax, 0x7FF); // rax got overwritted by our long jump (it does mov rax, <jmpaddr>; jmp rax)
				a.cmp(word_ptr(rbx, 0x22), ax);
				a.jz(loc_2C98EF);

				a.bind(skip_jz);

				// execute original code from 0x2C98C6 to 0x2C98CC
				a.mov(edx, dword_ptr(rdi, 0x8));
				a.mov(rcx, rbx);

				// the section of code that was overwritten by our jump is finished so we can jump back to the game code
				a.jmp(0x2C98CC_b);

				// original code
				a.bind(loc_2C98EF);
				a.jmp(0x2C98EF_b);
			});
		}

		void weapon_rocket_launcher_fire_stub(utils::hook::assembler& a)
		{
			const auto loc_463D2A = a.newLabel();
			a.mov(rax, rcx);
			a.push(rax);

			a.mov(rcx, qword_ptr(rsi, 0x158));
			a.test(rcx, rcx);
			a.jz(loc_463D2A);
			a.movss(xmm1, dword_ptr(rdi));
			a.movss(xmm0, dword_ptr(rcx, 0x84));

			a.mov(rax, qword_ptr(reinterpret_cast<uint64_t>(&dvars::g_rocketJumpScale)));
			a.movss(xmm3, qword_ptr(rax, 0x10));
			a.mulss(xmm1, xmm3);

			a.pop(rax);
			a.jmp(0x463CE4_b);

			a.bind(loc_463D2A);
			a.pop(rax);
			a.jmp(0x463D2A_b);
		}

		// https://github.com/REVLIIS/IW4-mechanics-for-H2M
		bool check_for_righty_tighty(game::mp::pmove_t* pm)
		{
			if ((pm->oldcmd.buttons & game::BUTTON_USERELOAD) == 0 && ((pm->cmd.buttons & game::BUTTON_USERELOAD) != 0) ||
				((pm->oldcmd.buttons & game::BUTTON_RELOAD) == 0 && ((pm->cmd.buttons & game::BUTTON_RELOAD) != 0)))
			{
				if ((pm->ps->sprintState.lastSprintEnd - pm->ps->sprintState.lastSprintStart) < 50)
				{
					if (game::PM_Weapon_AllowReload(pm->ps, game::WEAPON_HAND_RIGHT) && !game::PM_Weapon_AllowReload(pm->ps, game::WEAPON_HAND_LEFT))
					{
						game::PM_SetReloadingState(pm->ps, game::WEAPON_HAND_RIGHT);
						return true;
					}
				}
			}

			return false;
		}

		bool check_for_wrist_twist(game::mp::pmove_t* pm)
		{
			if ((pm->cmd.buttons & game::BUTTON_USERELOAD) == 0 && ((pm->oldcmd.buttons & game::BUTTON_USERELOAD) != 0) ||
				(pm->cmd.buttons & game::BUTTON_RELOAD) == 0 && ((pm->oldcmd.buttons & game::BUTTON_RELOAD) != 0))
			{
				// if we are allowed to reload our left gun, and NOT allowed to reload right gun, start wrist twist
				if (game::PM_Weapon_AllowReload(pm->ps, game::WEAPON_HAND_LEFT) && !game::PM_Weapon_AllowReload(pm->ps, game::WEAPON_HAND_RIGHT))
				{
					game::PM_SetReloadingState(pm->ps, game::WEAPON_HAND_LEFT);
					pm->ps->torsoAnim = 3181; // reload anim, overrides the reset in BG_ClearReloadAnim
					return true;
				}
			}

			return false;
		}

		void sprint_drop(game::mp::pmove_t* pm)
		{
			game::mp::playerState_s* ps = pm->ps;
			auto handIndex = game::BG_PlayerLastWeaponHand(ps);

			for (auto i = 0; i <= handIndex; i++)
			{
				if (i == game::WEAPON_HAND_LEFT && check_for_righty_tighty(pm))
				{
					continue;
				}

				ps->weapState[i].weaponState = game::WEAPON_SPRINT_DROP;
				ps->weapState[i].weaponTime = game::BG_SprintOutTime(ps->weapCommon.weapon, false, ps->weapCommon.lastWeaponHand == game::WEAPON_HAND_LEFT);
				ps->weapState[i].weaponDelay = 0;

				if (ps->pm_type != game::PM_DEAD && ps->pm_type != game::PM_DEAD_LINKED)
				{
					ps->weapState[i].weapAnim = ps->weapState[i].weaponState & ANIM_TOGGLEBIT | game::WEAP_ANIM_SPEED_RELOAD;
				}
			}
		}

		void sprint_raise(game::mp::pmove_t* pm)
		{
			game::mp::playerState_s* ps = pm->ps;
			auto handIndex = game::BG_PlayerLastWeaponHand(ps);

			for (auto i = 0; i <= handIndex; i++)
			{
				ps->weapState[i].weaponState = game::WEAPON_SPRINT_RAISE;
				ps->weapState[i].weaponTime = game::BG_SprintInTime(ps->weapCommon.weapon, false, ps->weapCommon.lastWeaponHand == game::WEAPON_HAND_LEFT);
				ps->weapState[i].weaponDelay = 0;

				if (ps->pm_type != game::PM_DEAD && ps->pm_type != game::PM_DEAD_LINKED)
				{
					ps->weapState[i].weapAnim = ps->weapState[i].weaponState & ANIM_TOGGLEBIT | game::WEAP_ANIM_FAST_RELOAD_END;
				}

				if (ps->weapCommon.lastWeaponHand == game::WEAPON_HAND_LEFT)
				{
					if (i == game::WEAPON_HAND_RIGHT)
					{
						check_for_righty_tighty(pm);
					}
					else if (i == game::WEAPON_HAND_LEFT)
					{
						check_for_wrist_twist(pm);
					}
				}
			}
		}

		// reversed from IW4
		void pm_weapon_check_for_sprint_stub(game::mp::pmove_t* pm)
		{
			if (!pm->cmd.weapon.data)
			{
				return;
			}

			int weaponStateRight = pm->ps->weapState[game::WEAPON_HAND_RIGHT].weaponState;
			int weaponStateLeft = pm->ps->weapState[game::WEAPON_HAND_LEFT].weaponState;

			// don't override the inspection animation while sprinting (added for HMW)
			if (weaponStateRight == game::WEAPON_HEAT_COOLDOWN_END || weaponStateLeft == game::WEAPON_HEAT_COOLDOWN_END)
			{
				return;
			}

			if (weaponStateRight != game::WEAPON_FIRING && weaponStateRight != game::WEAPON_RECHAMBERING && weaponStateRight != game::WEAPON_MELEE_WAIT_FOR_RESULT && weaponStateRight != game::WEAPON_MELEE_FIRE && weaponStateRight != game::WEAPON_MELEE_END)
			{
				if (weaponStateLeft != game::WEAPON_FIRING && weaponStateLeft != game::WEAPON_RECHAMBERING
					&& weaponStateLeft != game::WEAPON_MELEE_WAIT_FOR_RESULT && weaponStateLeft != game::WEAPON_MELEE_FIRE && weaponStateLeft != game::WEAPON_MELEE_END
					&& weaponStateRight != game::WEAPON_RAISING && weaponStateRight != game::WEAPON_RAISING_ALTSWITCH
					&& weaponStateRight != game::WEAPON_DROPPING && weaponStateRight != game::WEAPON_DROPPING_QUICK && weaponStateRight != game::WEAPON_DROPPING_ALT
					&& weaponStateRight != game::WEAPON_OFFHAND_INIT && weaponStateRight != game::WEAPON_OFFHAND_PREPARE && weaponStateRight != game::WEAPON_OFFHAND_HOLD && weaponStateRight != game::WEAPON_OFFHAND_HOLD_PRIMED && weaponStateRight != game::WEAPON_OFFHAND_END
					)
				{
					if (((pm->ps->pm_flags & game::PMF_SPRINTING) != 0) && (weaponStateRight != game::WEAPON_SPRINT_RAISE && weaponStateRight != game::WEAPON_SPRINT_LOOP && weaponStateRight != game::WEAPON_SPRINT_DROP))
					{
						sprint_raise(pm);
					}
					else if (((pm->ps->pm_flags & game::PMF_SPRINTING) == 0) && (weaponStateRight == game::WEAPON_SPRINT_RAISE || weaponStateRight == game::WEAPON_SPRINT_LOOP))
					{
						sprint_drop(pm);
					}
				}
			}
		}

		/*
			this detour fixes an issue when you're on servers and trying to wrist twist with +usereload
			i added an additional check to see if you're pressing the usereload button when the sprint raise event is happening,
			so if your connection isn't perfect you dont stop halfway trough a wrist twist
		*/
		bool pm_sprint_ending_buttons_stub(game::mp::playerState_s* ps, int8_t forwardSpeed, int buttons)
		{
			if ((ps->pm_flags & (game::POF_PLAYER | game::POF_THERMAL_VISION_OVERLAY_FOF | game::POF_THERMAL_VISION)) != 0)
			{
				return true;
			}

			if (forwardSpeed <= 105)
			{
				return true;
			}

			//// mwr code
			//int cancel_on_buttons =
			//	game::BUTTON_MELEEZOOM | game::BUTTON_UNK1 |
			//	game::BUTTON_PRONE | game::BUTTON_DUCK |
			//	game::BUTTON_GOSTAND | game::BUTTON_ADS |
			//	game::BUTTON_FRAG | game::BUTTON_SMOKE;

			//int is_ball_carrier = game::BG_HasPerk(ps->perks, game::PERK_BALLCARRIER);
			//if (!is_ball_carrier)
			//	cancel_on_buttons |= game::BUTTON_ATTACK;

			//int has_low_profile = game::BG_HasPerk(ps->perks, game::PERK_LOWPROFILE);
			//if (has_low_profile)
			//	cancel_on_buttons &= ~(game::BUTTON_DUCK | game::BUTTON_USERELOAD | game::BUTTON_RELOAD);

			//// this completely disables part of the previous statement, is the low profile perk even used?
			//cancel_on_buttons |= game::BUTTON_USERELOAD | game::BUTTON_RELOAD;

			// original iw4 checks
			int cancel_on_buttons =
				game::BUTTON_ATTACK | game::BUTTON_MELEEZOOM |
				game::BUTTON_RELOAD | game::BUTTON_USERELOAD |
				game::BUTTON_PRONE | game::BUTTON_DUCK |
				game::BUTTON_GOSTAND | game::BUTTON_ADS |
				game::BUTTON_FRAG | game::BUTTON_SMOKE;

			int weapon_state = ps->weapState[game::WEAPON_HAND_RIGHT].weaponState;
			if ((buttons & cancel_on_buttons) != 0)
			{
				// +usereload high ping fix
				if (ps->weapCommon.lastWeaponHand == game::WEAPON_HAND_LEFT && (buttons & game::BUTTON_USERELOAD) == 0 && weapon_state == game::WEAPON_SPRINT_RAISE)
				{
					return false;
				}

				return true;
			}

			bool is_in_melee_or_nade_throw = (weapon_state - game::WEAPON_MELEE_WAIT_FOR_RESULT) <= (game::WEAPON_OFFHAND_END - game::WEAPON_MELEE_WAIT_FOR_RESULT);
			bool is_in_nightvision_equip = (weapon_state - game::WEAPON_NIGHTVISION_WEAR) <= (game::WEAPON_NIGHTVISION_REMOVE - game::WEAPON_NIGHTVISION_WEAR);
			bool is_in_blast_or_hybrid_scope = (weapon_state - game::WEAPON_BLAST_IMPACT) <= (game::WEAPON_HEAT_COOLDOWN_START - game::WEAPON_BLAST_IMPACT);

			return is_in_melee_or_nade_throw || is_in_nightvision_equip || is_in_blast_or_hybrid_scope;
		}

		void begin_weapon_change_stub(game::mp::pmove_t* pm, game::Weapon new_weap, bool is_new_alt, bool quick, unsigned int* holdrand)
		{
			if (!pm_iw4_mechanics || !pm_iw4_mechanics->current.enabled)
			{
				begin_weapon_change_hook.invoke<void>(pm, new_weap, is_new_alt, quick, holdrand);
				return;
			}

			auto right_anim = pm->ps->weapState[game::WEAPON_HAND_RIGHT].weapAnim;
			auto left_anim = pm->ps->weapState[game::WEAPON_HAND_LEFT].weapAnim;

			auto stall_anim = (pm->ps->sprintState.lastSprintStart > pm->ps->sprintState.lastSprintEnd);

			begin_weapon_change_hook.invoke<void>(pm, new_weap, is_new_alt, quick, holdrand);

			if (stall_anim)
			{
				pm->ps->weapState[game::WEAPON_HAND_RIGHT].weapAnim = right_anim;
				pm->ps->weapState[game::WEAPON_HAND_LEFT].weapAnim = left_anim;
			}
		}

		inline bool is_previous_anim(int anim)
		{
			return	(anim == game::WEAP_ANIM_IDLE || anim == game::WEAP_ANIM_FAST_RELOAD_END ||
				anim == (game::WEAP_ANIM_IDLE | ANIM_TOGGLEBIT) || anim == (game::WEAP_ANIM_FAST_RELOAD_END | ANIM_TOGGLEBIT));
		}

		void start_weapon_anim_stub(uint64_t local_client_num, game::Weapon weapon_idx, game::PlayerHandIndex player_hand_idx,
			game::weapAnimFiles_t blend_in_anim_index, game::weapAnimFiles_t blend_out_anim_index, float transition_time)
		{
			auto* cg_array = game::CG_GetLocalClientGlobals();
			auto* playerstate = &cg_array[local_client_num].predictedPlayerState;

			auto should_sprint = (playerstate->sprintState.lastSprintStart < playerstate->sprintState.lastSprintEnd);

			auto do_glide =
				blend_out_anim_index == game::WEAP_ANIM_SPRINT_IN || // allow glides on sprint drop
				blend_out_anim_index == game::WEAP_ANIM_SPRINT_LOOP; // allow glides on sprint loop

			if (pm_glide_on_inspect && pm_glide_on_inspect->current.enabled)
			{
				do_glide |= blend_out_anim_index == game::WEAP_ANIM_INSPECTION;
			}

			if (do_glide && is_previous_anim(playerstate->weapState[player_hand_idx].weapAnim) && should_sprint)
			{
				blend_out_anim_index = game::WEAP_ANIM_QUICK_DROP;
				transition_time = 0.5f;
			}

			start_weapon_anim_hook.invoke<void>(local_client_num, weapon_idx, player_hand_idx, blend_in_anim_index, blend_out_anim_index, transition_time);
		}

		bool pm_sprint_start_interfering_buttons_stub(game::mp::playerState_s* ps, int forward_speed, int buttons)
		{
			if ((ps->pm_flags & game::PMF_LADDER) || forward_speed <= 105)
			{
				return true;
			}

			int interfere_on_buttons =
				game::BUTTON_ATTACK | game::BUTTON_MELEEZOOM |
				game::BUTTON_RELOAD | game::BUTTON_USERELOAD |
				game::BUTTON_GOSTAND | game::BUTTON_ADS |
				game::BUTTON_FRAG | game::BUTTON_SMOKE;

			if (buttons & interfere_on_buttons)
			{
				return true;
			}

			if (ps->pm_flags & (game::PMF_SHELLSHOCKED | game::PMF_SIGHT_AIMING |
				game::PMF_LADDER | game::PMF_MANTLE))
			{
				return true;
			}

			if (ps->pm_flags & game::PMF_JUMPING && ps->pm_time == 0)
			{
				return false;
			}

			auto weapon_state = ps->weapState[game::WEAPON_HAND_RIGHT].weaponState;
			if (weapon_state != game::WEAPON_MELEE_WAIT_FOR_RESULT
				&& weapon_state != game::WEAPON_MELEE_FIRE
				&& weapon_state != game::WEAPON_MELEE_END
				&& (weapon_state < game::WEAPON_OFFHAND_INIT || weapon_state > game::WEAPON_OFFHAND_END))
			{
				return false;
			}

			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			dvars::player_sustainAmmo = dvars::register_bool("player_sustainAmmo", false,
				game::DVAR_FLAG_REPLICATED, "Firing weapon will not decrease clip ammo");
			pm_weapon_use_ammo_hook.create(SELECT_VALUE(0x4AF600_b, 0x2DF830_b), &pm_weapon_use_ammo_stub);

			// Influence PM_JitterPoint code flow so the trace->startsolid checks are 'ignored'
			pm_player_trace_hook.create(SELECT_VALUE(0x4A0A90_b, 0x2D14C0_b), &pm_player_trace_stub);

			// If g_enableElevators is 1 the 'ducked' flag will always be removed from the player state
			utils::hook::jump(SELECT_VALUE(0x499617_b, 0x2C9F90_b), utils::hook::assemble(pm_trace_stub), true);
			dvars::g_enableElevators = dvars::register_bool("g_enableElevators", false, game::DVAR_FLAG_REPLICATED, "Enables Elevators");

			dvars::pm_bouncing = dvars::register_bool("pm_bouncing", false,
				game::DVAR_FLAG_REPLICATED, "Enable bouncing");
			utils::hook::jump(SELECT_VALUE(0x4A2E5E_b, 0x2D39A4_b), pm_bouncing_stub_mp(), true);

			if (game::environment::is_sp())
			{
				return;
			}
			
			utils::hook::nop(0x4006AD_b, 15);
			utils::hook::jump(0x4006AD_b, g_speed_stub(), true);
			dvars::g_speed = dvars::register_int("g_speed", 190, 0, 1000,
				game::DVAR_FLAG_REPLICATED, "changes the speed of the player");

			dvars::pm_bouncingAllAngles = dvars::register_bool("pm_bouncingAllAngles", false,
				game::DvarFlags::DVAR_FLAG_REPLICATED, "Enable bouncing from all angles");
			utils::hook::call(0x2D3A74_b, pm_project_velocity_stub);

			dvars::g_gravity = dvars::register_int("g_gravity", 800, 0, 1000, game::DVAR_FLAG_REPLICATED,
				"Game gravity in inches per second squared");
			utils::hook::jump(0x3FF812_b, client_end_frame_stub(), true);
			utils::hook::nop(0x3FF808_b, 1);

			dvars::pm_sprintInAir = dvars::register_bool("pm_sprintInAir", false,
				game::DVAR_FLAG_REPLICATED, "Enable Mid-Air Sprinting");
			utils::hook::jump(0x2C98C0_b, pm_can_start_sprint_stub(), true);

			auto* timescale = dvars::register_float("timescale", 1.0f, 0.1f, 50.0f, game::DVAR_FLAG_REPLICATED, "Changes Timescale of the game");
			utils::hook::inject(0x15B204_b, &timescale->current.value); // Com_GetTimeScale
			utils::hook::inject(0x17D243_b, &timescale->current.value); // Com_Restart
			utils::hook::inject(0x17E609_b, &timescale->current.value); // Com_SetSlowMotion
			utils::hook::inject(0x17E626_b, &timescale->current.value); // Com_SetSlowMotion
			utils::hook::inject(0x17E69C_b, &timescale->current.value); // Com_SetSlowMotion
			utils::hook::inject(0x17EAD0_b, &timescale->current.value); // Com_TimeScaleMsec
			utils::hook::inject(0x17EFE2_b, &timescale->current.value); // Com_UpdateSlowMotion
			utils::hook::inject(0x17F00C_b, &timescale->current.value); // Com_UpdateSlowMotion

			dvars::jump_ladderPushVel = dvars::register_float("jump_ladderPushVel", 128.0f,
				0.0f, 1024.0f, game::DVAR_FLAG_REPLICATED, "The velocity of a jump off of a ladder");
			utils::hook::jump(0x2BD70C_b, jump_push_off_ladder(), true);
			utils::hook::nop(0x2BD718_b, 4); // Nop skipped opcodes

			dvars::jump_height = dvars::register_float("jump_height", 39.0f,
				0.0f, 1000.0f, game::DVAR_FLAG_REPLICATED, "The maximum height of a player\'s jump");
			utils::hook::call(0x2BD22D_b, jump_start_stub);

			jump_apply_slowdown_hook.create(0x2BD0B0_b, jump_apply_slowdown_stub);
			dvars::jump_slowDownEnable = dvars::register_bool("jump_slowDownEnable", true, game::DVAR_FLAG_REPLICATED, "Slow player movement after jumping");

			pm_crashland_hook.create(0x2CB070_b, pm_crashland_stub);
			dvars::jump_enableFallDamage = dvars::register_bool("jump_enableFallDamage", true, game::DVAR_FLAG_REPLICATED, "Enable fall damage");

			dvars::g_playerEjection = dvars::register_bool("g_playerEjection", true, game::DVAR_FLAG_REPLICATED,
				"Flag whether player ejection is on or off");
			stuck_in_client_hook.create(0x4035F0_b, stuck_in_client_stub);

			dvars::g_playerCollision = dvars::register_bool("g_playerCollision", true, game::DVAR_FLAG_REPLICATED,
				"Flag whether player collision is on or off");
			cm_transformed_capsule_trace_hook.create(0x4D63C0_b, cm_transformed_capsule_trace_stub);

			dvars::g_rocketJumpScale = dvars::register_float("g_rocketJumpScale", 64.0f, 0.0f, 1000.0f, game::DVAR_FLAG_REPLICATED, "Adjust rocket jump scale");
			utils::hook::set<std::uint8_t>(0x463CC7_b, 0x48); // save rax
			utils::hook::set<std::uint8_t>(0x463CC8_b, 0x89);
			utils::hook::set<std::uint8_t>(0x463CC9_b, 0xC1);
			utils::hook::jump(0x463CCA_b, utils::hook::assemble(weapon_rocket_launcher_fire_stub), true);

			// Make noclip work
			client_end_frame_hook.create(0x3FF7D0_b, client_end_frame_stub2);
			g_damage_client_hook.create(0x414F10_b, g_damage_client_stub);
			g_damage_hook.create(0x414A10_b, g_damage_stub);
			
			// let moveSpeedScale be used in GSC
			utils::hook::set<uint32_t>(0x4406FE_b, 0x1DC);

			pm_iw4_mechanics = dvars::register_bool("pm_iw4Mechanics", false, game::DVAR_FLAG_REPLICATED, "Use IW4 mechanics");
			pm_glide_on_inspect = dvars::register_bool("pm_glideOnInspect", true, game::DVAR_FLAG_NONE, "Do a gliding animation on inspects?");

			// stall animations on sprints
			begin_weapon_change_hook.create(0x2D57E0_b, begin_weapon_change_stub);

			// glides (thank you @girlmachinery for the help on this)
			start_weapon_anim_hook.create(0x1D5CA0_b, start_weapon_anim_stub);

			// sprint raise & drop events, don't stop inspecting on sprinting
			pm_weapon_check_for_sprint_hook.create(0x2D9A10_b, pm_weapon_check_for_sprint_stub);
			pm_sprint_ending_buttons_hook.create(0x2CEE40_b, pm_sprint_ending_buttons_stub);

			// removes the slight "delay" for sprint whenever u shot making some still swaps possible again (from @Patoke)
			pm_sprint_start_interfering_buttons_hook.create(0x2CEEC0_b, pm_sprint_start_interfering_buttons_stub);
		}
	};
}

REGISTER_COMPONENT(gameplay::component)
