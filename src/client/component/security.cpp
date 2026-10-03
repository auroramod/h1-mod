#include <std_include.hpp>
#include "security.hpp"

void security::post_unpack()
{
	if (game::environment::is_sp())
	{
		return;
	}

	// Patch vulnerability in PlayerCards_SetCachedPlayerData
	utils::hook::call(0x1402328DD, set_cached_playerdata_stub);

	// Patch entity overflow
	utils::hook::jump(0x140493977, assemble(remap_cached_entities_stub), true);
}

void security::set_cached_playerdata_stub(const int localclient, const int index1, const int index2)
{
	if (index1 >= 0 && index1 < 18 && index2 >= 0 && index2 < 42)
	{
		utils::hook::invoke<void>(0x14056FEA0, localclient, index1, index2);
	}
}

void security::remap_cached_entities(game::cachedSnapshot_t& snapshot)
{
	static bool printed = false;
	if (snapshot.num_clients > 1200 && !printed)
	{
		printed = true;
		printf("Too many entities (%d)... remapping!\n", snapshot.num_clients);
	}

	snapshot.num_clients = std::min(snapshot.num_clients, 1200);
}

void security::remap_cached_entities_stub(utils::hook::assembler& a)
{
	a.pushad64();

	a.mov(rcx, rbx);
	a.call_aligned(remap_cached_entities);

	a.popad64();
	a.jmp(0x140493988);
}

REGISTER_COMPONENT(security)
