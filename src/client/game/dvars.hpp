#pragma once

#include "game.hpp"
#include "structs.hpp"
#include <string>

namespace dvars
{
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

	extern game::dvar_t* jump_enableFallDamage;

	extern game::dvar_t* r_fullbright;
	extern game::dvar_t* r_chams;

	extern game::dvar_t* cg_legacyCrashHandling;

	extern std::vector<std::string> dvar_list;

	std::string dvar_get_vector_domain(const int components, const game::dvar_limits& domain);
	std::string dvar_get_domain(const game::dvar_type type, const game::dvar_limits& domain);

	game::dvar_t* register_int(const std::string& name, int value, int min, int max, game::DvarFlags flags, bool add_to_list = true);
	game::dvar_t* register_bool(const std::string& name, bool value, game::DvarFlags flags, bool add_to_list = true);
	game::dvar_t* register_string(const std::string& name, const char* value, game::DvarFlags flags, bool add_to_list = true);
	game::dvar_t* register_float(const std::string& name, float value, float min, float max, game::DvarFlags flags, bool add_to_list = true);
	game::dvar_t* register_vec4(const std::string& name, float x, float y, float z, float w, float min, float max, game::DvarFlags flags, bool add_to_list = true);
}
