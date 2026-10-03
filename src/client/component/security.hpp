#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

class security final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void set_cached_playerdata_stub(const int localclient, const int index1, const int index2);
	static void remap_cached_entities(game::cachedSnapshot_t& snapshot);
	static void remap_cached_entities_stub(utils::hook::assembler& a);
};
