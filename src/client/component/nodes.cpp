#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "gsc/script_extension.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace nodes
{
	namespace
	{
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

			gsc::function::add("markdangerousnodes", mark_dangerous_nodes);
			gsc::function::add("markdangerousnodesintrigger", mark_dangerous_nodes_in_trigger);
		}
	};
}

REGISTER_COMPONENT(nodes::component)
