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
		game::dvar_t* jump_slowDownEnable;
		game::dvar_t* jump_enableFallDamage;

		void jump_apply_slowdown_stub(game::mp::playerState_s* ps)
		{
			if (jump_slowDownEnable->current.enabled)
			{
				utils::hook::invoke<void>(0x1401D5360, ps);
			}
		}

		void pm_crashland_stub(game::mp::playerState_s* ps, void* pml)
		{
			if (jump_enableFallDamage->current.enabled)
			{
				utils::hook::invoke<void>(0x1401E2D00, ps, pml);
			}
		}

		const auto pm_bouncing_stub_mp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto no_bounce = a.newLabel();
			const auto loc_1401EAF9D = a.newLabel();

			a.push(rax);

			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::pm_bouncing)));
			a.mov(al, byte_ptr(rax, 0x10));
			a.cmp(byte_ptr(rbp, -0x2D), al);

			a.pop(rax);
			a.jz(no_bounce);
			a.jmp(0x1401EB000);

			a.bind(no_bounce);
			a.cmp(dword_ptr(rsp, 0x70), 0);
			a.jnz(loc_1401EAF9D);
			a.jmp(0x1401EAFF1);

			a.bind(loc_1401EAF9D);
			a.jmp(0x1401EAF9D);
		});

		const auto client_end_frame_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rax);

			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::g_gravity)));
			a.mov(eax, dword_ptr(rax, 0x10));
			a.mov(word_ptr(rbx, 0x34), ax);

			a.pop(rax);

			// Game code hook skipped
			a.mov(eax, dword_ptr(rbx, 0x494C));
			a.mov(rdi, rcx);

			a.jmp(0x140322F82);
		});
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

			utils::hook::call(0x1401E8830, jump_apply_slowdown_stub);
			jump_slowDownEnable = dvars::register_bool("jump_slowDownEnable", true, game::DVAR_FLAG_REPLICATED, "Slow player movement after jumping");

			utils::hook::call(0x1401E490F, pm_crashland_stub);
			jump_enableFallDamage = dvars::register_bool("jump_enableFallDamage", true, game::DVAR_FLAG_REPLICATED, "Enable fall damage");

			// Implement bouncing dvar
			dvars::pm_bouncing = dvars::register_bool("pm_bouncing", false,
				game::DVAR_FLAG_REPLICATED, "Enable bouncing");
			utils::hook::jump(0x1401EAFE4, pm_bouncing_stub_mp, true);

			dvars::g_gravity = dvars::register_int("g_gravity", 800, std::numeric_limits<short>::min(),
				std::numeric_limits<short>::max(), game::DVAR_FLAG_REPLICATED, "");
			utils::hook::jump(0x140322F72, client_end_frame_stub, true);
			utils::hook::nop(0x140322F68, 1); // Nop skipped opcode
		}
	};
}

REGISTER_COMPONENT(gameplay::component)
