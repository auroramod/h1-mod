#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace anims
{
	namespace
	{
		struct animScriptCondition_t
		{
			int index;
			unsigned int value[2];
		};

		struct animScriptCommand_t
		{
			__int16 bodyPart;
			__int16 animIndex;
			__int16 animDuration;
		};

		struct __declspec(align(4)) animScriptItem_t
		{
			int numConditions;
			animScriptCondition_t conditions[5];
			int numCommands;
			animScriptCommand_t commands[11];
		};

		struct animation_s
		{
			char* name;
			__int64 movetype;
			float moveSpeed;
			int nameHash;
			int flags;
			__int16 initialLerp;
			unsigned __int16 duration;
			unsigned __int16 localMeleeVictimAnimIndex;
			char noteType;
			char aimSet;
			char leanSet;
			char turns;
			char twitches;
			char syncGroup;
		};

		struct animScriptData_t
		{
			animation_s animations[1]; // idk
		};

		void root_motion_stub(animScriptItem_t* item, animScriptData_t* data, int index)
		{
			const auto command = item->commands[index].animIndex;
			data->animations[command].flags |= 0x80000u;
		}

		void bg_parse_commands_stub(utils::hook::assembler& a)
		{
			a.pushad64();
			a.mov(r8, rsi);
			a.mov(rdx, rbx);
			a.mov(rcx, rdi);
			a.call_aligned(root_motion_stub);
			a.popad64();

			a.jmp(0x2B70F0_b);
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

			utils::hook::jump(0x2B7628_b, utils::hook::assemble(bg_parse_commands_stub), true);
		}
	};
}

REGISTER_COMPONENT(anims::component)
