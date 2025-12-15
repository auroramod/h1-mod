#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "gsc/script_extension.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace pathnodes
{
	namespace
	{
		game::dvar_t* scr_enable_jump_nodes = nullptr;

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
		}
	};
}

REGISTER_COMPONENT(pathnodes::component)
