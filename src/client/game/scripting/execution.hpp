#pragma once
#include "game/game.hpp"
#include "entity.hpp"
#include "script_value.hpp"

namespace scripting
{
	script_value call_function(const std::string& name, const std::vector<script_value>& arguments);
	script_value call_function(const std::string& name, const entity& entity,
	                           const std::vector<script_value>& arguments);

	template <typename T = script_value>
	T call(const std::string& name, const std::vector<script_value>& arguments = {});

	template <>
	script_value call(const std::string& name, const std::vector<script_value>& arguments);

	template <typename T>
	T call(const std::string& name, const std::vector<script_value>& arguments)
	{
		return call<script_value>(name, arguments).as<T>();
	}

	script_value exec_ent_thread(const entity& entity, const char* pos, const std::vector<script_value>& arguments);
	const char* get_function_pos(const std::string& filename, const std::string& function);
	script_value call_script_function(const entity& entity, const std::string& filename,
		const std::string& function, const std::vector<script_value>& arguments);

	void clear_entity_fields(const entity& entity);
	void clear_custom_fields();

	void set_entity_field(const entity& entity, const std::string& field, const script_value& value);
	script_value get_entity_field(const entity& entity, const std::string& field);

	void notify(const entity& entity, const std::string& event, const std::vector<script_value>& arguments);
}
