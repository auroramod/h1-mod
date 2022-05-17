#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace virtuallobby
{
	namespace
	{
		game::dvar_t* virtualLobby_fovscale;

		const auto get_fovscale_stub = utils::hook::assemble([](utils::hook::assembler& a)
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
			a.mov(rax, ptr(reinterpret_cast<int64_t>(&virtualLobby_fovscale)));
			a.jmp(ret);

			// original
			a.bind(original);
			a.popad64();
			a.mov(rax, qword_ptr(0x1413A8580));
			a.jmp(ret);

			a.bind(ret);
			a.mov(rcx, 0x142935000);
			a.jmp(0x1400B556A);
		});
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

			virtualLobby_fovscale = dvars::register_float("virtualLobby_fovScale", 0.7f, 0.0f, 2.0f, 
				game::DVAR_FLAG_SAVED, "Field of view scaled for the virtual lobby");

			utils::hook::nop(0x1400B555C, 14);
			utils::hook::jump(0x1400B555C, get_fovscale_stub, true);
		}
	};
}

//REGISTER_COMPONENT(virtuallobby::component)
