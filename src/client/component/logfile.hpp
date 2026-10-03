#pragma once
#include "loader/component_loader.hpp"

#include "game/scripting/entity.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/lua/value_conversion.hpp"
#include "game/scripting/lua/error.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>

class logfile final : public component_interface
{
public:
	void post_unpack() override;

	static bool hook_enabled;

	static void add_player_damage_callback(const sol::protected_function& callback);
	static void add_player_killed_callback(const sol::protected_function& callback);
	static void clear_callbacks();
	static void enable_vm_execute_hook();
	static void disable_vm_execute_hook();
	static bool client_command_stub(const int client_num);
	static void set_lua_hook(const char* pos, const sol::protected_function&);
	static void set_gsc_hook(const char* source, const char* target);
	static void clear_hook(const char* pos);
	static size_t get_hook_count();

private:
	static sol::lua_value convert_entity(lua_State* state, const game::gentity_s* ent);
	static std::string get_weapon_name(unsigned int weapon, bool isAlternate);
	static sol::lua_value convert_vector(lua_State* state, const float* vec);
	static std::string convert_mod(const int meansOfDeath);
	static void scr_player_killed_stub(game::gentity_s* self, const game::gentity_s* inflictor, game::gentity_s* attacker, int damage, const int meansOfDeath, const unsigned int weapon, const bool isAlternate, const float* vDir, const unsigned int hitLoc, int psTimeOffset, int deathAnimDuration);
	static void scr_player_damage_stub(game::gentity_s* self, const game::gentity_s* inflictor, game::gentity_s* attacker, int damage, int dflags, const int meansOfDeath, const unsigned int weapon, const bool isAlternate, const float* vPoint, const float* vDir, const unsigned int hitLoc, const int timeOffset);
	static unsigned int local_id_to_entity(unsigned int local_id);
	static bool execute_vm_hook(const char* pos);
	static void vm_execute_stub(utils::hook::assembler& a);
	static void g_log_printf_stub(const char* fmt, ...);
};
