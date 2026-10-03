#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class dvars_component final : public component_interface
{
public:
	void post_start() override;
	void post_unpack() override;

	struct dvar_combined
	{
		std::string string{};
		game::dvar_value value{};
		game::dvar_limits limits{};
		unsigned int flags{};
	};

	struct override
	{
		static void register_bool(const std::string& name, bool value, unsigned int flags);
		static void register_float(const std::string& name, float value, float min, float max, unsigned int flags);
		static void register_int(const std::string& name, int value, int min, int max, unsigned int flags);
		static void register_string(const std::string& name, const std::string& value, unsigned int flags);
		static void register_vec2(const std::string& name, float x, float y, float min, float max, unsigned int flags);
		static void register_vec3(const std::string& name, float x, float y, float z, float min, float max, unsigned int flags);

		static void set_bool(const std::string& name, bool boolean);
		static void set_float(const std::string& name, float fl);
		static void set_int(const std::string& name, int integer);
		static void set_string(const std::string& name, const std::string& string);
	};

	struct callback
	{
		static void on_new_value(const std::string& name, const std::function<void(game::dvar_value* value)>& callback);
		static void on_register(const std::string& name, const std::function<void()>& callback);
	};

private:
	static void dvar_register_bool(int hash, const char* name, bool& value, unsigned int& flags);
	static void dvar_register_float(int hash, const char* name, float& value, float& min, float& max, unsigned int& flags);
	static void dvar_register_int(int hash, const char* name, int& value, int& min, int& max, unsigned int& flags);
	static void dvar_register_string(int hash, const char* name, const char*& value, unsigned int& flags);
	static void dvar_register_vector2(int hash, const char* name, float& x, float& y, float& min, float& max, unsigned int& flags);
	static void dvar_register_vector3(int hash, const char* name, float& x, float& y, float& z, float& min, float& max, unsigned int& flags);

	static void dvar_set_bool(const game::dvar_t* dvar, bool& boolean);
	static void dvar_set_float(const game::dvar_t* dvar, float& fl);
	static void dvar_set_int(const game::dvar_t* dvar, int& integer);
	static void dvar_set_string(const game::dvar_t* dvar, const char*& string);

	static void check_dvar_overrides(int hash, const char* name, game::dvar_type type, unsigned int& flags, game::dvar_value* value, game::dvar_limits* domain);

	static game::dvar_t* dvar_register_new_stub(int hash, const char* name, game::dvar_type type, unsigned int flags, game::dvar_value* value, game::dvar_limits* domain);
	static void dvar_re_register_stub(game::dvar_t* dvar, int hash, const char* name, game::dvar_type type, unsigned int flags, game::dvar_value* reset_value, game::dvar_limits* domain);
	static void dvar_set_variant_stub(game::dvar_t* dvar, game::dvar_value* value, game::DvarSetSource source);

	template <typename T>
	static T* find_dvar(std::unordered_map<std::string, T>& map, const std::string& name)
	{
		auto i = map.find(name);
		return i != map.end() ? &i->second : nullptr;
	}

	template <typename T>
	static T* find_dvar(std::unordered_map<std::string, T>& map, const int hash)
	{
		for (auto& [key, val] : map)
		{
			if (game::generateHashValue(key.data()) == hash)
			{
				return &val;
			}
		}

		return nullptr;
	}
};
