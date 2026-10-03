#include <std_include.hpp>
#include "virtuallobby.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

static game::dvar_t* virtual_lobby_fovscale;

void virtuallobby::post_unpack()
{
	if (!game::environment::is_mp())
	{
		return;
	}

	virtual_lobby_fovscale = dvars::register_float_hashed("virtualLobby_fovScale", 0.7f, 0.0f, 2.0f,
		game::DVAR_ARCHIVE, "Field of view scaled for the virtual lobby");

	utils::hook::jump(0x1400B555C, utils::hook::assemble(get_fovscale_stub), true);
}

void virtuallobby::get_fovscale_stub(utils::hook::assembler& a)
{
	const auto ret = a.newLabel();
	const auto original = a.newLabel();

	a.pushad64();
	a.mov(rax, qword_ptr(0x1425F7210)); // virtualLobbyInFiringRange
	a.cmp(byte_ptr(rax, 0x10), 1);
	a.je(original);
	a.call_aligned(game::VirtualLobby_Loaded);
	a.cmp(al, 0);
	a.je(original);

	// virtuallobby
	a.popad64();
	a.mov(rax, ptr(reinterpret_cast<int64_t>(&virtual_lobby_fovscale)));
	a.jmp(ret);

	// original
	a.bind(original);
	a.popad64();
	a.mov(rax, qword_ptr(0x1413A8580)); // cg_fovScale
	a.jmp(ret);

	a.bind(ret);
	a.mov(rcx, 0x142935000); // cgameGlob
	a.jmp(0x1400B556A);
}

REGISTER_COMPONENT(virtuallobby)
