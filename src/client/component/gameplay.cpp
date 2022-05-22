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
				const auto loc_1401EAF9D = a.newLabel();

				a.push(rax);

				a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::pm_bouncing)));
				a.mov(al, byte_ptr(rax, 0x10));
				a.cmp(al, 0);

				a.pop(rax);
				a.jz(no_bounce);
				a.jmp(0x2D39C0_b);

				a.bind(no_bounce);
				a.cmp(dword_ptr(rsp, 0x70), 0);
				a.jnz(loc_1401EAF9D);
				a.jmp(0x2D39B1_b);

				a.bind(loc_1401EAF9D);
				a.jmp(0x2D395D_b);
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

		void pm_player_trace_stub(game::pmove_t* pm, game::trace_t* trace, const float* f3,
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
			a.and_(dword_ptr(r15, 0x54), 0xFFFFFFFD);
			a.jmp(0x2C9F9D_b);

			a.bind(allsolid);
			a.jmp(0x2C9F9F_b);
		};
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			dvars::player_sustainAmmo = dvars::register_bool("player_sustainAmmo", false,
				game::DVAR_FLAG_REPLICATED, "Firing weapon will not decrease clip ammo");
			pm_weapon_use_ammo_hook.create(0x2DF830_b, &pm_weapon_use_ammo_stub);
			
			/*utils::hook::nop(0x4006AD_b, 15);
			utils::hook::jump(0x4006AD_b, g_speed_stub(), true);
			dvars::g_speed = dvars::register_int("g_speed", 190, 0, 1000,
				game::DVAR_FLAG_REPLICATED, "changes the speed of the player");

			dvars::pm_bouncing = dvars::register_bool("pm_bouncing", false,
				game::DVAR_FLAG_REPLICATED, "Enable bouncing");
			utils::hook::jump(0x2D39A4_b, pm_bouncing_stub_mp(), true);

			dvars::g_gravity = dvars::register_int("g_gravity", 800, 0, 1000, game::DVAR_FLAG_REPLICATED,
				"Game gravity in inches per second squared");
			utils::hook::jump(0x3FF812_b, client_end_frame_stub(), true);
			utils::hook::nop(0x3FF808_b, 1);

			// Influence PM_JitterPoint code flow so the trace->startsolid checks are 'ignored' 
			pm_player_trace_hook.create(0x2D14C0_b, &pm_player_trace_stub);
			// If g_enableElevators is 1 the 'ducked' flag will always be removed from the player state
			utils::hook::jump(0x2C9F90_b, utils::hook::assemble(pm_trace_stub), true);
			dvars::g_enableElevators = dvars::register_bool("g_enableElevators", false, game::DvarFlags::DVAR_FLAG_NONE, "Enables Elevators");

			auto* timescale = dvars::register_float("timescale", 1.0f, 0.1f, 50.0f, game::DVAR_FLAG_REPLICATED, "Changes Timescale of the game");
			utils::hook::inject(0x15B204_b, &timescale->current.value); // Com_GetTimeScale
			utils::hook::inject(0x17D241_b, &timescale->current.value); // Com_Restart
			utils::hook::inject(0x17E609_b, &timescale->current.value); // Com_SetSlowMotion
			utils::hook::inject(0x17E626_b, &timescale->current.value); // Com_SetSlowMotion
			utils::hook::inject(0x17E69C_b, &timescale->current.value); // Com_SetSlowMotion
			// utils::hook::inject(0x1400DB9CC, &timescale->current.value); // Com_ErrorCleanup_Shutdown (Inlined)
			utils::hook::inject(0x17EAD0_b, &timescale->current.value); // Com_TimeScaleMsec (Crash)
			utils::hook::inject(0x17EFE2_b, &timescale->current.value); // Com_UpdateSlowMotion
			utils::hook::inject(0x17F00C_b, &timescale->current.value); // Com_UpdateSlowMotion
			*/

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
		}
	};
}

REGISTER_COMPONENT(gameplay::component)
