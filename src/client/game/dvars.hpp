#pragma once

#include "game.hpp"
#include "structs.hpp"
#include <string>

namespace dvars
{
	struct dvar_info
	{
		std::string name;
		std::string description;
		std::int32_t hash;
	};

	extern std::unordered_map<std::int32_t, dvar_info> dvar_map;

	extern game::dvar_t* aimassist_enabled;

	extern game::dvar_t* con_inputBoxColor;
	extern game::dvar_t* con_inputHintBoxColor;
	extern game::dvar_t* con_outputBarColor;
	extern game::dvar_t* con_outputSliderColor;
	extern game::dvar_t* con_outputWindowColor;
	extern game::dvar_t* con_inputDvarMatchColor;
	extern game::dvar_t* con_inputDvarValueColor;
	extern game::dvar_t* con_inputDvarInactiveValueColor;
	extern game::dvar_t* con_inputCmdMatchColor;
	extern game::dvar_t* g_playerEjection;
	extern game::dvar_t* g_playerCollision;
	extern game::dvar_t* player_sustainAmmo;
	extern game::dvar_t* g_enableElevators;
	
	extern game::dvar_t* g_speed;

	extern game::dvar_t* g_gravity;

	extern game::dvar_t* pm_bouncing;
	extern game::dvar_t* pm_bouncingAllAngles;
	extern game::dvar_t* pm_sprintInAir;

	extern game::dvar_t* jump_ladderPushVel;

	extern game::dvar_t* jump_height;

	extern game::dvar_t* jump_enableFallDamage;

	extern game::dvar_t* r_fullbright;
	extern game::dvar_t* r_chams;

	extern game::dvar_t* cg_legacyCrashHandling;

	constexpr int generate_hash(const char* string)
	{
		const char* v1;
		char v2, v6;
		int v4, v5, v7;
		char* end_ptr;

		v1 = string;
		v2 = *string;

		if (v2 == 48 && v1[1] == 120)
		{
			return strtoul(v1 + 2, &end_ptr, 16);
		}

		v4 = v2;

		if ((v2 - 65) <= 0x19u)
		{
			v4 = v2 + 32;
		}

		v5 = 0xB3CB2E29 * static_cast<unsigned int>(v4 ^ 0x319712C3);

		if (v2)
		{
			do
			{
				v6 = *++v1;
				v7 = v6;
				if ((v6 - 65) <= 0x19u)
				{
					v7 = v6 + 32;
				}

				v5 = 0xB3CB2E29 * static_cast<unsigned int>(v5 ^ v7);
			} while (v6);
		}

		return v5;
	}

	std::int32_t generate_hash(const std::string& string);

	void insert_dvar_info(const std::int32_t hash, const std::string& name, const std::string& description);
	void insert_dvar_info(const std::string& name, const std::string& description);

	std::string dvar_get_vector_domain(const int components, const game::dvar_limits& domain);
	std::string dvar_get_domain(const game::dvar_type type, const game::dvar_limits& domain);
	std::string dvar_get_description(const std::string& name);
	std::optional<dvar_info> get_dvar_info_from_hash(const std::int32_t hash);

	game::dvar_t* register_int(const std::string& name, int value, int min, int max,
		unsigned int flags, const std::string& description);
	game::dvar_t* register_bool(const std::string& name, bool value,
		unsigned int flags, const std::string& description);
	game::dvar_t* register_string(const std::string& name, const char* value, 
		unsigned int flags, const std::string& description);
	game::dvar_t* register_float(const std::string& name, float value, float min, float max,
		unsigned int flags, const std::string& description);
	game::dvar_t* register_float_hashed(const std::string& name, float value, float min, float max,
		unsigned int flags, const std::string& description);
	game::dvar_t* register_vec4(const std::string& name, float x, float y, float z, float w, float min, 
		float max, unsigned int flags, const std::string& description);
}
