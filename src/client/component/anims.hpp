#pragma once
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>

class anims final : public component_interface
{
public:
	void post_unpack() override;

private:
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

	static void root_motion_stub(animScriptItem_t* item, animScriptData_t* data, int index);
	static void bg_parse_commands_stub(utils::hook::assembler& a);
	static void set_anim_rate_stub(utils::hook::assembler& a);
};
