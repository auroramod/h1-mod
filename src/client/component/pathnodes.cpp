#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "gsc/script_extension.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/csv.hpp>
#include <utils/hook.hpp>

#include "console.hpp"
#include "filesystem.hpp"

namespace pathnodes
{
	namespace
	{
		game::dvar_t* scr_enable_jump_nodes = nullptr;
		
		utils::memory::allocator path_allocator;

		static const std::unordered_map<std::string, unsigned short> waypoint_types =
		{
			{"stand", 13},
			{"crouch", 14},
			{"prone", 15},
		};

		scripting::script_value mark_dangerous_nodes(const gsc::function_args& args)
		{
			if (args.size() < 3)
			{
				throw std::runtime_error("invalid args");
			}

			const auto origin = args[0].as<scripting::vector>();
			const auto radius = args[1].as<float>();

			if (radius <= 0.f)
			{
				throw std::runtime_error("radius must be > 0");
			}

			const auto dangerous = args[2].as<int>();
			game::SV_BotMarkNodesAsDangerous(origin, radius, dangerous);
			return {};
		}

		scripting::script_value mark_dangerous_nodes_in_trigger(const gsc::function_args& args)
		{
			if (args.size() < 2)
			{
				throw std::runtime_error("invalid args");
			}

			const auto entity = args[0].as<scripting::entity>();
			const auto dangerous = args[1].as<int>();

			const auto entref = entity.get_entity_reference();
			if (entref.classnum != 0)
			{
				throw std::runtime_error("invalid entity");
			}

			const auto trigger = &game::mp::g_entities[entref.entnum];

			const auto x1 = std::abs(trigger->box.midPoint[0]);
			const auto y1 = std::abs(trigger->box.midPoint[1]);

			const auto max_height = ((trigger->box.halfSize[2] + trigger->box.halfSize[2]) + 256.f) + 1.f;

			const auto distance = std::sqrtf(
				(y1 + trigger->box.halfSize[1]) * (y1 + trigger->box.halfSize[1]) +
				(x1 + trigger->box.halfSize[0]) * (x1 + trigger->box.halfSize[0])
			);

			game::pathsort_s nodes[256]{};
			const auto count = game::Path_NodesInCylinder(trigger->origin, nullptr, (distance + 256.f) + 1.f, max_height, nodes, 256, -1);

			float midpoint[3]{};
			float halfsize[3]{};

			midpoint[0] = trigger->absBox.midPoint[0];
			midpoint[1] = trigger->absBox.midPoint[1];
			midpoint[2] = trigger->absBox.midPoint[2];

			halfsize[0] = trigger->absBox.halfSize[0] + 128.f;
			halfsize[1] = trigger->absBox.halfSize[1] + 128.f;
			halfsize[2] = trigger->absBox.halfSize[2] + 128.f;

			for (auto i = 0; i < count; i++)
			{
				const auto node = nodes[i].node;
				float pos[3]{};
				pos[0] = node->constant.vLocalOrigin[0];
				pos[1] = node->constant.vLocalOrigin[1];
				pos[2] = node->constant.vLocalOrigin[2];
			
				game::WorldifyPosFromParent(node, pos);

				const auto delta_x = std::abs(midpoint[0] - pos[0]);
				const auto delta_y = std::abs(midpoint[1] - pos[1]);
				const auto delta_z = std::abs(midpoint[2] - pos[2]);
				const auto is_in = (delta_x <= halfsize[0]) && (delta_y <= halfsize[1]) && (delta_z <= halfsize[2]);

				if (!is_in)
				{
					continue;
				}

				if (dangerous)
				{
					++node->dynamic.dangerousCount;
					node->dynamic.flags |= 1u;
				}
				else
				{
					if (node->dynamic.dangerousCount > 0)
					{
						node->dynamic.dangerousCount--;
					}

					if (node->dynamic.dangerousCount == 0)
					{
						node->dynamic.flags &= 0xFFFEu;
					}
				}
			}

			return {};
		}

		constexpr const auto node_type_jump = 32;
		constexpr const auto node_type_jump_attack = 33;
		constexpr const auto node_type_end = 34;

		bool is_jump_node(const std::uint16_t type)
		{
			return type == node_type_jump || type == node_type_jump_attack;
		}

		bool is_traverse_begin_node(const std::uint16_t type)
		{
			return type == game::NODE_NEGOTIATION_BEGIN || type == game::NODE_NEGOTIATION_BEGIN_3D;
		}

		bool is_traverse_end_node(const std::uint16_t type)
		{
			return type == game::NODE_NEGOTIATION_END || type == game::NODE_NEGOTIATION_END_3D;
		}

		bool is_traverse_begin_or_jump_node(const std::uint16_t type)
		{
			return is_traverse_begin_node(type) || is_jump_node(type);
		}

		bool is_traverse_end_or_jump_node(const std::uint16_t type)
		{
			return is_traverse_end_node(type) || is_jump_node(type);
		}

		const char* node_types[] =
		{
			"Error",
			"Path",
			"Cover Stand",
			"Cover Crouch",
			"Cover Crouch Window",
			"Cover Prone",
			"Cover Right",
			"Cover Left",
			"Cover Wide Right",
			"Cover Wide Left",
			"Cover Multi",
			"Ambush",
			"Exposed",
			"Conceal Stand",
			"Conceal Crouch",
			"Conceal Prone",
			"Door",
			"Door Interior",
			"Scripted",
			"Begin",
			"End",
			"Turret",
			"Guard",
			"Path 3D",
			"Cover Up 3D",
			"Cover Right 3D",
			"Cover Left 3D",
			"Exposed 3D",
			"Scripted 3D",
			"Begin 3D",
			"End 3D",
			"",
			"Jump",
			"Jump Attack",
		};

		bool check_traverse_node(const game::pathnode_t* a1, const game::pathnode_t* a2)
		{
			if (!scr_enable_jump_nodes->current.enabled)
			{
				return is_traverse_begin_node(a1->constant.type) && is_traverse_end_node(a2->constant.type) &&
					(a1->constant.targetname == a2->constant.targetname);
			}
			else
			{
				return is_traverse_begin_or_jump_node(a1->constant.type) && is_traverse_end_or_jump_node(a2->constant.type) &&
					(a1->constant.targetname == a2->constant.targetname || (is_jump_node(a1->constant.type) && is_jump_node(a2->constant.type)));
			}
		}

		void path_generate_path_stub(utils::hook::assembler& a)
		{
			const auto do_traverse = a.newLabel();

			a.push(rax);
			a.pushad64();
			a.mov(rcx, rdi);
			a.mov(rdx, rsi);
			a.call_aligned(check_traverse_node);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.test(eax, eax);
			a.jnz(do_traverse);

			a.jmp(0x3EAD2A_b);

			a.bind(do_traverse);
			a.jmp(0x3EAD0A_b);
		}
		
		float distance(float* a, float* b)
		{
			return std::sqrtf((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
		}

		game::pathnode_tree_t* allocate_tree(game::PathData* asset)
		{
			++asset->nodeTreeCount;
			return reinterpret_cast<game::pathnode_tree_t*>(
				game::mp::Hunk_AllocAlignInternal(sizeof(game::pathnode_tree_t), 4));
		}

		game::pathnode_tree_t* build_node_tree(game::PathData* asset, unsigned short* node_indexes, const int num_nodes)
		{
			if (num_nodes < 4)
			{
				const auto result = allocate_tree(asset);
				result->axis = -1;
				result->u.s.nodeCount = num_nodes;
				result->u.s.nodes = node_indexes;
				return result;
			}

			game::vec2_t maxs{};
			game::vec2_t mins{};

			const auto start_node = &asset->nodes[*node_indexes];
			maxs[0] = start_node->constant.vLocalOrigin[0];
			mins[0] = maxs[0];
			maxs[1] = start_node->constant.vLocalOrigin[1];
			mins[1] = maxs[1];

			for (auto i = 1; i < num_nodes; i++)
			{	
				for (auto axis = 0; axis < 2; axis++)
				{
					const auto node = &asset->nodes[node_indexes[i]];
					const auto value = node->constant.vLocalOrigin[axis];
					if (mins[axis] <= value)
					{
						if (value > maxs[axis])
						{
							maxs[axis] = value;
						}
					}
					else
					{
						mins[axis] = value;
					}
				}
			}

			const auto axis = (maxs[1] - mins[1]) > (maxs[0] - mins[0]);
			if ((maxs[axis] - mins[axis]) > 192.f)
			{
				const auto dist = (maxs[axis] + mins[axis]) * 0.5f;
				auto left = 0;

				for (auto right = num_nodes - 1; ; --right)
				{
					while (dist > asset->nodes[node_indexes[left]].constant.vLocalOrigin[axis])
					{
						++left;
					}

					while (asset->nodes[node_indexes[right]].constant.vLocalOrigin[axis] > dist)
					{
						--right;
					}

					if (left >= right)
					{
						break;
					}

					const auto swap_node = node_indexes[left];
					node_indexes[left] = node_indexes[right];
					node_indexes[right] = swap_node;
					++left;
				}

				while (2 * left < num_nodes &&
					asset->nodes[node_indexes[left]].constant.vLocalOrigin[axis] == dist)
				{
					++left;
				}

				while (2 * left < num_nodes &&
					asset->nodes[node_indexes[left - 1]].constant.vLocalOrigin[axis] == dist)
				{
					--left;
				}

				game::pathnode_tree_t* child[2]{};
				child[0] = build_node_tree(asset, node_indexes, left);
				child[1] = build_node_tree(asset, &node_indexes[left], num_nodes - left);
				const auto result = allocate_tree(asset);
				result->axis = axis;
				result->dist = dist;
				result->u.child[0] = child[0];
				result->u.child[1] = child[1];
				
				return result;
			}
			
			const auto result = allocate_tree(asset);
			result->axis = -1;
			result->u.s.nodeCount = num_nodes;
			result->u.s.nodes = node_indexes;
			return result;
		}
		
		void path_init_stub()
		{
			// Path_Init
			utils::hook::invoke<void>(0x3F8370_b);

			if (game::VirtualLobby_Loaded())
			{
				return;
			}

			// load new paths with zt util to parse bot warfare from disk
			std::string buffer;

			auto mapname = game::Dvar_FindVar("mapname");
			
			if (const auto file_path = std::format("maps/mp/{}_wp.csv", mapname->current.string); filesystem::read_file(file_path, &buffer))
			{
				console::debug("Loading paths '%s' from disk", file_path.data());
				
				auto table = utils::csv::parser(buffer);

				auto* asset = path_allocator.allocate<game::PathData>(); // stringtable allocator lul

				asset->name = path_allocator.duplicate_string(std::string(mapname->current.string));
				
				if (table.get_num_rows() <= 0)
				{
					return;
				}
				
				const auto rows = table.get_rows();
				asset->nodeCount = std::atoi(rows[0]->fields[0]);
				asset->nodes = path_allocator.allocate_array<game::pathnode_t>(asset->nodeCount);

				if (table.get_num_rows() < static_cast<int>(asset->nodeCount))
				{
					console::error("Less than asset->nodeCount + 1 (%i) rows", asset->nodeCount);
					return;
				}

				for (auto i = 0u; i < asset->nodeCount; i++)
				{
					const auto row = rows[i + 1];
					const auto node = &asset->nodes[i];
					node->constant.type = 1;

					if (row->num_fields < 4)
					{
						console::error("Not enough fields for node num %i (must be origin,links,type,angles,...)", i);
						return;
					}

					const auto origin_str = utils::string::split(row->fields[0], ' ');
					if (origin_str.size() == 3)
					{
						node->constant.vLocalOrigin[0] = static_cast<float>(std::atof(origin_str[0].data()));
						node->constant.vLocalOrigin[1] = static_cast<float>(std::atof(origin_str[1].data()));
						node->constant.vLocalOrigin[2] = static_cast<float>(std::atof(origin_str[2].data()));
					}

					auto field = row->fields[2];
					auto field_str = path_allocator.duplicate_string(field);
					if (waypoint_types.contains(field_str))
					{
						node->constant.type = waypoint_types.at(field_str);
					}

					const auto angles_str = utils::string::split(row->fields[3], ' ');
					if (angles_str.size() == 3)
					{
						node->constant.___u9.angles[0] = static_cast<float>(std::atof(angles_str[1].data()));
						node->constant.___u9.angles[1] = static_cast<float>(std::atof(angles_str[0].data()));
						node->constant.___u9.angles[2] = static_cast<float>(std::atof(angles_str[2].data()));
					}

					const auto links_str = utils::string::split(row->fields[1], ' ');
					node->constant.totalLinkCount = static_cast<unsigned short>(links_str.size());
					node->constant.Links = path_allocator.allocate_array<game::pathlink_s>(links_str.size());
					for (auto o = 0; o < node->constant.totalLinkCount; o++)
					{
						const auto num = std::atoi(links_str[o].data());
						node->constant.Links[o].nodeNum = static_cast<unsigned short>(num);
					}
				}

				for (auto i = 0u; i < asset->nodeCount; i++)
				{
					const auto node = &asset->nodes[i];
					for (auto o = 0; o < node->constant.totalLinkCount; o++)
					{
						const auto linked_num = node->constant.Links[o].nodeNum;
						if (linked_num >= asset->nodeCount)
						{
							console::error("Node link num out of bounds");
							return;
						}

						const auto linked = &asset->nodes[linked_num];
						node->constant.Links[o].negotiationLink = 1;
						node->constant.Links[o].fDist = 
							distance(node->constant.vLocalOrigin, linked->constant.vLocalOrigin);
					}
				}

				const auto node_indexes = path_allocator.allocate_array<unsigned short>(asset->nodeCount);
				for (auto i = 0u; i < asset->nodeCount; i++)
				{
					node_indexes[i] = static_cast<unsigned short>(i);
				}

				asset->nodeTree = build_node_tree(asset, node_indexes, asset->nodeCount);

				console::debug("new node count is %d\n", asset->nodeCount);

				*game::pathdata = *asset;
			}
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

			scr_enable_jump_nodes = dvars::register_bool("scr_enableJumpNodes", false, game::DVAR_FLAG_REPLICATED, "enable jump nodes");

			// implement jump nodes from iw6
			utils::hook::inject(0x3F5F03_b + 3, node_types);
			utils::hook::inject(0x3F66A0_b + 3, node_types);
			utils::hook::set<std::uint8_t>(0x3F66E7_b + 2, node_type_end);

			utils::hook::jump(0x3EACE0_b, utils::hook::assemble(path_generate_path_stub), true);

			gsc::function::add("markdangerousnodes", mark_dangerous_nodes);
			gsc::function::add("markdangerousnodesintrigger", mark_dangerous_nodes_in_trigger);
			
			// add bot warfare CSV loading on map loading
			utils::hook::call(0x420911_b, path_init_stub);
		}
	};
}

REGISTER_COMPONENT(pathnodes::component)
