#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace security
{
	namespace
	{
		void set_cached_playerdata_stub(const int localclient, const int index1, const int index2)
		{
			if (index1 >= 0 && index1 < 18 && index2 >= 0 && index2 < 42)
			{
				utils::hook::invoke<void>(0x61A9D0_b, localclient, index1, index2);
			}
		}

		void remap_cached_entities(game::mp::cachedSnapshot_t& snapshot)
		{
			static bool printed = false;
			if (snapshot.num_clients > 1200 && !printed)
			{
				printed = true;
				printf("Too many entities (%d)... remapping!\n", snapshot.num_clients);
			}

			snapshot.num_clients = std::min(snapshot.num_clients, 1200);
		}

		void remap_cached_entities_stub(utils::hook::assembler& a)
		{
			a.pushad64();

			a.mov(rcx, rbx);
			a.call_aligned(remap_cached_entities);

			a.popad64();
			a.jmp(0x55E4D8_b);
		}
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

			// Patch vulnerability in PlayerCards_SetCachedPlayerData
			utils::hook::call(0xF4632_b, set_cached_playerdata_stub);

			// Patch entity overflow
			utils::hook::jump(0x55E4C7_b, assemble(remap_cached_entities_stub), true);
		}
	};
}

REGISTER_COMPONENT(security::component)
