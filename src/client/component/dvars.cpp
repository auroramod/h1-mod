#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "dvars.hpp"

#include "console.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>

static std::unordered_map<uint32_t, std::unordered_map<std::string, dvars_component::dvar_combined>> register_overrides;
static std::unordered_map<uint32_t, std::unordered_map<std::string, dvars_component::dvar_combined>> set_overrides;

static std::unordered_map<int, std::function<void(game::dvar_value*)>> dvar_new_value_callbacks;
static std::unordered_map<int, std::function<void()>> dvar_on_register_function_map;

static utils::hook::detour dvar_register_new_hook;
static utils::hook::detour dvar_re_register_hook;
static utils::hook::detour dvar_set_variant_hook;

void dvars_component::post_start()
{
	try
	{
		const auto list_json = utils::nt::load_resource(DVAR_LIST);
		const auto list = nlohmann::json::parse(list_json);
		for (const auto& [_0, dvar_info] : list.items())
		{
			const auto name = dvar_info[0].get<std::string>();
			const auto description = dvar_info[1].get<std::string>();
			dvars::insert_dvar_info(name, description);
		}
	}
	catch (const std::exception& e)
	{
		console::error("Failed to parse dvar list: %s\n", e.what());
	}
}

void dvars_component::post_unpack()
{
	dvar_register_new_hook.create(SELECT_VALUE(0x14041B1D0, 0x1404FC730), dvar_register_new_stub); // Dvar_RegisterNew
	dvar_re_register_hook.create(SELECT_VALUE(0x14041B420, 0x1404FC9F0), dvar_re_register_stub); // Dvar_Reregister
	dvar_set_variant_hook.create(SELECT_VALUE(0x14041C190, 0x1404FD970), dvar_set_variant_stub); // Dvar_SetVariant
}

void dvars_component::override::register_bool(const std::string& name, const bool value, const unsigned int flags)
{
	dvar_combined values;
	values.value.enabled = value;
	values.flags = flags;
	register_overrides[game::dvar_type::boolean][name] = std::move(values);
}

void dvars_component::override::register_float(const std::string& name, const float value, const float min, const float max,
	const unsigned int flags)
{
	dvar_combined values;
	values.value.value = value;
	values.limits.value.min = min;
	values.limits.value.max = max;
	values.flags = flags;
	register_overrides[game::dvar_type::value][name] = std::move(values);
}

void dvars_component::override::register_int(const std::string& name, const int value, const int min, const int max,
	const unsigned int flags)
{
	dvar_combined values;
	values.value.integer = value;
	values.limits.integer.min = min;
	values.limits.integer.max = max;
	values.flags = flags;
	register_overrides[game::dvar_type::integer][name] = std::move(values);
}

void dvars_component::override::register_string(const std::string& name, const std::string& value,
	const unsigned int flags)
{
	dvar_combined values;
	values.string = value;
	values.flags = flags;
	register_overrides[game::dvar_type::string][name] = std::move(values);
}

void dvars_component::override::register_vec2(const std::string& name, const float x, const float y, const float min,
	const float max, const unsigned int flags)
{
	dvar_combined values;
	values.value.vector[0] = x;
	values.value.vector[1] = y;
	values.limits.vector.min = min;
	values.limits.vector.max = max;
	values.flags = flags;
	register_overrides[game::dvar_type::vec2][name] = std::move(values);
}

void dvars_component::override::register_vec3(const std::string& name, const float x, const float y, const float z,
	const float min, const float max, const unsigned int flags)
{
	dvar_combined values;
	values.value.vector[0] = x;
	values.value.vector[1] = y;
	values.value.vector[2] = z;
	values.limits.vector.min = min;
	values.limits.vector.max = max;
	values.flags = flags;
	register_overrides[game::dvar_type::vec3][name] = std::move(values);
}

void dvars_component::override::set_bool(const std::string& name, const bool boolean)
{
	set_overrides[game::dvar_type::boolean][name].value.enabled = boolean;
}

void dvars_component::override::set_float(const std::string& name, const float fl)
{
	set_overrides[game::dvar_type::value][name].value.value = fl;
}

void dvars_component::override::set_int(const std::string& name, const int integer)
{
	set_overrides[game::dvar_type::integer][name].value.integer = integer;
}

void dvars_component::override::set_string(const std::string& name, const std::string& string)
{
	set_overrides[game::dvar_type::string][name].string = string;
}

void dvars_component::callback::on_new_value(const std::string& name, const std::function<void(game::dvar_value*)>& callback)
{
	dvar_new_value_callbacks[game::generateHashValue(name.data())] = callback;
}

void dvars_component::callback::on_register(const std::string& name, const std::function<void()>& callback)
{
	dvar_on_register_function_map[game::generateHashValue(name.data())] = callback;
}

void dvars_component::dvar_register_bool(const int hash, const char* /*name*/, bool& value, unsigned int& flags)
{
	if (const auto* var = find_dvar(register_overrides[game::dvar_type::boolean], hash))
	{
		value = var->value.enabled;
		flags = var->flags;
	}
}

void dvars_component::dvar_register_float(const int hash, const char* /*name*/, float& value, float& min, float& max, unsigned int& flags)
{
	if (const auto* var = find_dvar(register_overrides[game::dvar_type::value], hash))
	{
		value = var->value.value;
		min = var->limits.value.min;
		max = var->limits.value.max;
		flags = var->flags;
	}
}

void dvars_component::dvar_register_int(const int hash, const char* /*name*/, int& value, int& min, int& max, unsigned int& flags)
{
	if (const auto* var = find_dvar(register_overrides[game::dvar_type::integer], hash))
	{
		value = var->value.integer;
		min = var->limits.integer.min;
		max = var->limits.integer.max;
		flags = var->flags;
	}
}

void dvars_component::dvar_register_string(const int hash, const char* /*name*/, const char*& value, unsigned int& flags)
{
	if (const auto* var = find_dvar(register_overrides[game::dvar_type::string], hash))
	{
		value = var->string.data();
		flags = var->flags;
	}
}

void dvars_component::dvar_register_vector2(const int hash, const char* /*name*/, float& x, float& y, float& min, float& max,
	unsigned int& flags)
{
	if (const auto* var = find_dvar(register_overrides[game::dvar_type::vec2], hash))
	{
		x = var->value.vector[0];
		y = var->value.vector[1];
		min = var->limits.vector.min;
		max = var->limits.vector.max;
		flags = var->flags;
	}
}

void dvars_component::dvar_register_vector3(const int hash, const char* /*name*/, float& x, float& y, float& z, float& min,
	float& max, unsigned int& flags)
{
	if (const auto* var = find_dvar(register_overrides[game::dvar_type::vec3], hash))
	{
		x = var->value.vector[0];
		y = var->value.vector[1];
		z = var->value.vector[2];
		min = var->limits.vector.min;
		max = var->limits.vector.max;
		flags = var->flags;
	}
}

void dvars_component::dvar_set_bool(const game::dvar_t* dvar, bool& boolean)
{
	if (const auto* var = find_dvar(set_overrides[game::dvar_type::boolean], dvar->hash))
	{
		boolean = var->value.enabled;
	}
}

void dvars_component::dvar_set_float(const game::dvar_t* dvar, float& fl)
{
	if (const auto* var = find_dvar(set_overrides[game::dvar_type::value], dvar->hash))
	{
		fl = var->value.value;
	}
}

void dvars_component::dvar_set_int(const game::dvar_t* dvar, int& integer)
{
	if (const auto* var = find_dvar(set_overrides[game::dvar_type::integer], dvar->hash))
	{
		integer = var->value.integer;
	}
}

void dvars_component::dvar_set_string(const game::dvar_t* dvar, const char*& string)
{
	if (const auto* var = find_dvar(set_overrides[game::dvar_type::string], dvar->hash))
	{
		string = var->string.data();
	}
}

void dvars_component::check_dvar_overrides(const int hash, const char* name, const game::dvar_type type, unsigned int& flags,
	game::dvar_value* value, game::dvar_limits* domain)
{
	switch (type)
	{
	case game::dvar_type::boolean:
	case game::dvar_type::boolean_hashed:
		dvar_register_bool(hash, name, value->enabled, flags);
		break;
	case game::dvar_type::value:
	case game::dvar_type::value_hashed:
		dvar_register_float(hash, name, value->value, domain->value.min, domain->value.max, flags);
		break;
	case game::dvar_type::integer:
	case game::dvar_type::integer_hashed:
		dvar_register_int(hash, name, value->integer, domain->integer.min, domain->integer.max, flags);
		break;
	case game::dvar_type::string:
		dvar_register_string(hash, name, value->string, flags);
		break;
	case game::dvar_type::vec2:
		dvar_register_vector2(hash, name, value->vector[0], value->vector[1], domain->vector.min, domain->vector.max, flags);
		break;
	case game::dvar_type::vec3:
		dvar_register_vector3(hash, name, value->vector[0], value->vector[1], value->vector[2], domain->vector.min, domain->vector.max, flags);
		break;
	default:
		break;
	}
}

game::dvar_t* dvars_component::dvar_register_new_stub(const int hash, const char* name, const game::dvar_type type, unsigned int flags,
	game::dvar_value* value, game::dvar_limits* domain)
{
	check_dvar_overrides(hash, name, type, flags, value, domain);

	auto* dvar = dvar_register_new_hook.invoke<game::dvar_t*>(hash, name, type, flags, value, domain);
	if (dvar && dvar_on_register_function_map.contains(hash))
	{
		dvar_on_register_function_map[hash]();
		dvar_on_register_function_map.erase(hash);
	}

	return dvar;
}

void dvars_component::dvar_re_register_stub(game::dvar_t* dvar, const int hash, const char* name, const game::dvar_type type,
	unsigned int flags, game::dvar_value* reset_value, game::dvar_limits* domain)
{
	check_dvar_overrides(dvar->hash, name, type, flags, reset_value, domain);
	dvar_re_register_hook.invoke<void>(dvar, hash, name, type, flags, reset_value, domain);
}

void dvars_component::dvar_set_variant_stub(game::dvar_t* dvar, game::dvar_value* value, const game::DvarSetSource source)
{
	switch (dvar->type)
	{
	case game::dvar_type::boolean:
	case game::dvar_type::boolean_hashed:
		dvar_set_bool(dvar, value->enabled);
		break;
	case game::dvar_type::value:
	case game::dvar_type::value_hashed:
		dvar_set_float(dvar, value->value);
		break;
	case game::dvar_type::integer:
	case game::dvar_type::integer_hashed:
		dvar_set_int(dvar, value->integer);
		break;
	case game::dvar_type::string:
		dvar_set_string(dvar, value->string);
		break;
	default:
		break;
	}

	dvar_set_variant_hook.invoke<void>(dvar, value, source);

	if (dvar_new_value_callbacks.contains(dvar->hash))
	{
		dvar_new_value_callbacks[dvar->hash](value);
	}
}

REGISTER_COMPONENT(dvars_component)
