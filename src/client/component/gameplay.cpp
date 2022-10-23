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

		game::dvar_t* jump_slowDownEnable;
		game::dvar_t* jump_enableFallDamage;

		void jump_apply_slowdown_stub(game::mp::playerState_s* ps)
		{
			if (jump_slowDownEnable->current.enabled)
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
			if (jump_enableFallDamage->current.enabled)
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
				entity->client->pm_type = 2;
			}
			else if ((entity->client->flags & 2)) // ufo
			{
				entity->client->pm_type = 3;
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
			utils::hook::inject(0x17E69C_b, &timescale->current.value);// Com_SetSlowMotion
			utils::hook::inject(0x17EAD0_b, &timescale->current.value); // Com_TimeScaleMsec
			utils::hook::inject(0x17EFE2_b, &timescale->current.value); // Com_UpdateSlowMotion
			utils::hook::inject(0x17F00C_b, &timescale->current.value); //Com_UpdateSlowMotion

			dvars::jump_ladderPushVel = dvars::register_float("jump_ladderPushVel", 128.0f,
				0.0f, 1024.0f, game::DVAR_FLAG_REPLICATED, "The velocity of a jump off of a ladder");
			utils::hook::jump(0x2BD70C_b, jump_push_off_ladder(), true);
			utils::hook::nop(0x2BD718_b, 4); // Nop skipped opcodes

			dvars::jump_height = dvars::register_float("jump_height", 39.0f,
				0.0f, 1000.0f, game::DVAR_FLAG_REPLICATED, "The maximum height of a player\'s jump");
			utils::hook::call(0x2BD22D_b, jump_start_stub);

			jump_apply_slowdown_hook.create(0x2BD0B0_b, jump_apply_slowdown_stub);
			jump_slowDownEnable = dvars::register_bool("jump_slowDownEnable", true, game::DVAR_FLAG_REPLICATED, "Slow player movement after jumping");

			pm_crashland_hook.create(0x2CB070_b, pm_crashland_stub);
			jump_enableFallDamage = dvars::register_bool("jump_enableFallDamage", true, game::DVAR_FLAG_REPLICATED, "Enable fall damage");

			dvars::g_playerEjection = dvars::register_bool("g_playerEjection", true, game::DVAR_FLAG_REPLICATED,
				"Flag whether player ejection is on or off");
			stuck_in_client_hook.create(0x4035F0_b, stuck_in_client_stub);

			dvars::g_playerCollision = dvars::register_bool("g_playerCollision", true, game::DVAR_FLAG_REPLICATED,
				"Flag whether player collision is on or off");
			cm_transformed_capsule_trace_hook.create(0x4D63C0_b, cm_transformed_capsule_trace_stub);

			// Make noclip work
			client_end_frame_hook.create(0x3FF7D0_b, client_end_frame_stub2);
			g_damage_client_hook.create(0x414F10_b, g_damage_client_stub);
			g_damage_hook.create(0x414A10_b, g_damage_stub);
		}
	};
}

REGISTER_COMPONENT(gameplay::component)
