#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "anims.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

void anims::post_unpack()
{
	if (!game::environment::is_mp())
	{
		return;
	}

	utils::hook::jump(0x1401CFD6C, utils::hook::assemble(bg_parse_commands_stub), true);

	// prevent division by zero 
	utils::hook::jump(0x1400F3E65, utils::hook::assemble(set_anim_rate_stub), true);
}

void anims::root_motion_stub(animScriptItem_t* item, animScriptData_t* data, int index)
{
	const auto command = item->commands[index].animIndex;
	data->animations[command].flags |= 0x80000u;
}

void anims::bg_parse_commands_stub(utils::hook::assembler& a)
{
	a.pushad64();
	a.mov(r8, rsi);
	a.mov(rdx, rbx);
	a.mov(rcx, rdi);
	a.call_aligned(root_motion_stub);
	a.popad64();

	a.jmp(0x1401CF834);
}

void anims::set_anim_rate_stub(utils::hook::assembler& a)
{
	const auto is_zero = a.newLabel();

	a.xorps(xmm1, xmm1);
	a.ucomiss(xmm8, xmm1);
	a.jnp(is_zero);

	a.divss(xmm7, xmm8);
	a.mov(edx, edi);
	a.mov(rcx, rsi);
	a.mulss(xmm7, xmm9);
	a.jmp(0x1400F3E74);

	a.bind(is_zero);
	a.jmp(0x1400F3E7C);
}

REGISTER_COMPONENT(anims)
