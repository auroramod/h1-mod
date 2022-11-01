#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace virtuallobby
{
	namespace
	{
		game::dvar_t* virtual_lobby_fovscale;

		void get_get_fovscale_stub(utils::hook::assembler& a)
		{
			const auto ret = a.newLabel();
			const auto original = a.newLabel();

			a.pushad64();
			a.mov(rax, qword_ptr(0x2999CE8_b)); // virtualLobbyInFiringRange
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
			a.mov(rax, qword_ptr(0x14C4EC8_b));
			a.jmp(ret);

			a.bind(ret);
			a.mov(rdi, rax);
			a.mov(ecx, 8);
			a.jmp(0x104545_b);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			virtual_lobby_fovscale = dvars::register_float_hashed("virtualLobby_fovScale", 0.7f, 0.0f, 2.0f, 
				game::DVAR_FLAG_SAVED, "Field of view scaled for the virtual lobby");

			utils::hook::jump(0x104539_b, utils::hook::assemble(get_get_fovscale_stub), true);
		}
	};
}

REGISTER_COMPONENT(virtuallobby::component)
