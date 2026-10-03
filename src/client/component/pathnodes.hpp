#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "gsc/script_extension.hpp"

#include <utils/hook.hpp>

class pathnodes final : public component_interface
{
public:
	void post_unpack() override;

private:
	static scripting::script_value mark_dangerous_nodes(const gsc::function_args& args);
	static scripting::script_value mark_dangerous_nodes_in_trigger(const gsc::function_args& args);
	static bool is_jump_node(const std::uint16_t type);
	static bool is_traverse_begin_node(const std::uint16_t type);
	static bool is_traverse_end_node(const std::uint16_t type);
	static bool is_traverse_begin_or_jump_node(const std::uint16_t type);
	static bool is_traverse_end_or_jump_node(const std::uint16_t type);
	static bool check_traverse_node(const game::pathnode_t* a1, const game::pathnode_t* a2);
	static void path_generate_path_stub(utils::hook::assembler& a);
	static float distance(float* a, float* b);
	static game::pathnode_tree_t* allocate_tree(game::PathData* asset);
	static game::pathnode_tree_t* build_node_tree(game::PathData* asset, unsigned short* node_indexes, const int num_nodes);
	static void path_init_stub(const int restart);
};
