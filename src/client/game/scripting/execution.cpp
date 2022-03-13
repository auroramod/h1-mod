#include <std_include.hpp>
#include "execution.hpp"
#include "safe_execution.hpp"
#include "stack_isolation.hpp"

#include "component/scripting.hpp"

namespace scripting
{
	namespace
	{
		game::VariableValue* allocate_argument()
		{
			game::VariableValue* value_ptr = ++game::scr_VmPub->top;
			++game::scr_VmPub->inparamcount;
			return value_ptr;
		}

		void push_value(const script_value& value)
		{
			auto* value_ptr = allocate_argument();
			*value_ptr = value.get_raw();

			game::AddRefToValue(value_ptr->type, value_ptr->u);
		}

		int get_field_id(const int classnum, const std::string& field)
		{
			if (scripting::fields_table[classnum].find(field) != scripting::fields_table[classnum].end())
			{
				return scripting::fields_table[classnum][field];
			}

			return -1;
		}

		script_value get_return_value()
		{
			if (game::scr_VmPub->inparamcount == 0)
			{
				return {};
			}

			game::Scr_ClearOutParams();
			game::scr_VmPub->outparamcount = game::scr_VmPub->inparamcount;
			game::scr_VmPub->inparamcount = 0;

			return script_value(game::scr_VmPub->top[1 - game::scr_VmPub->outparamcount]);
		}
	}

	void notify(const entity& entity, const std::string& event, const std::vector<script_value>& arguments)
	{
		stack_isolation _;
		for (auto i = arguments.rbegin(); i != arguments.rend(); ++i)
		{
			push_value(*i);
		}

		const auto event_id = game::SL_GetString(event.data(), 0);
		game::Scr_NotifyId(entity.get_entity_id(), event_id, game::scr_VmPub->inparamcount);
	}

	script_value call_function(const std::string& name, const entity& entity,
	                           const std::vector<script_value>& arguments)
	{
		const auto entref = entity.get_entity_reference();

		const auto is_method_call = *reinterpret_cast<const int*>(&entref) != -1;
		const auto function = find_function(name, !is_method_call);
		if (function == nullptr)
		{
			throw std::runtime_error("Unknown "s + (is_method_call ? "method" : "function") + " '" + name + "'");
		}

		stack_isolation _;

		for (auto i = arguments.rbegin(); i != arguments.rend(); ++i)
		{
			push_value(*i);
		}

		game::scr_VmPub->outparamcount = game::scr_VmPub->inparamcount;
		game::scr_VmPub->inparamcount = 0;

		if (!safe_execution::call(function, entref))
		{
			throw std::runtime_error(
				"Error executing "s + (is_method_call ? "method" : "function") + " '" + name + "'");
		}

		return get_return_value();
	}

	script_value call_function(const std::string& name, const std::vector<script_value>& arguments)
	{
		return call_function(name, entity(), arguments);
	}

	template <>
	script_value call(const std::string& name, const std::vector<script_value>& arguments)
	{
		return call_function(name, arguments);
	}

	script_value exec_ent_thread(const entity& entity, const char* pos, const std::vector<script_value>& arguments)
	{
		const auto id = entity.get_entity_id();

		stack_isolation _;
		for (auto i = arguments.rbegin(); i != arguments.rend(); ++i)
		{
			scripting::push_value(*i);
		}

		game::AddRefToObject(id);

		const auto local_id = game::AllocThread(id);
		const auto result = game::VM_Execute(local_id, pos, (unsigned int)arguments.size());
		game::RemoveRefToObject(result);

		return get_return_value();
	}

	const char* get_function_pos(const std::string& filename, const std::string& function)
	{
		if (scripting::script_function_table.find(filename) == scripting::script_function_table.end())
		{
			throw std::runtime_error("File '" + filename + "' not found");
		};

		const auto functions = scripting::script_function_table[filename];
		if (functions.find(function) == functions.end())
		{
			throw std::runtime_error("Function '" + function + "' in file '" + filename + "' not found");
		}

		return functions.at(function);
	}

	script_value call_script_function(const entity& entity, const std::string& filename,
		const std::string& function, const std::vector<script_value>& arguments)
	{
		const auto pos = get_function_pos(filename, function);
		return exec_ent_thread(entity, pos, arguments);
	}

	static std::unordered_map<unsigned int, std::unordered_map<std::string, script_value>> custom_fields;

	script_value get_custom_field(const entity& entity, const std::string& field)
	{
		auto& fields = custom_fields[entity.get_entity_id()];
		const auto _field = fields.find(field);
		if (_field != fields.end())
		{
			return _field->second;
		}
		return {};
	}

	void set_custom_field(const entity& entity, const std::string& field, const script_value& value)
	{
		const auto id = entity.get_entity_id();

		if (custom_fields[id].find(field) != custom_fields[id].end())
		{
			custom_fields[id][field] = value;
			return;
		}

		custom_fields[id].insert(std::make_pair(field, value));
	}

	void clear_entity_fields(const entity& entity)
	{
		const auto id = entity.get_entity_id();

		if (custom_fields.find(id) != custom_fields.end())
		{
			custom_fields[id].clear();
		}
	}

	void clear_custom_fields()
	{
		custom_fields.clear();
	}

	void set_entity_field(const entity& entity, const std::string& field, const script_value& value)
	{
		const auto entref = entity.get_entity_reference();
		const int id = get_field_id(entref.classnum, field);

		if (id != -1)
		{
			stack_isolation _;
			push_value(value);

			game::scr_VmPub->outparamcount = game::scr_VmPub->inparamcount;
			game::scr_VmPub->inparamcount = 0;

			if (!safe_execution::set_entity_field(entref, id))
			{
				throw std::runtime_error("Failed to set value for field '" + field + "'");
			}
		}
		else
		{
			// Read custom fields
			set_custom_field(entity, field, value);
		}
	}

	script_value get_entity_field(const entity& entity, const std::string& field)
	{
		const auto entref = entity.get_entity_reference();
		const auto id = get_field_id(entref.classnum, field);

		if (id != -1)
		{
			stack_isolation _;

			game::VariableValue value{};
			if (!safe_execution::get_entity_field(entref, id, &value))
			{
				throw std::runtime_error("Failed to get value for field '" + field + "'");
			}

			const auto __ = gsl::finally([value]()
			{
				game::RemoveRefToValue(value.type, value.u);
			});

			return value;
		}

		// Add custom fields
		return get_custom_field(entity, field);
	}

	unsigned int make_array()
	{
		unsigned int index = 0;
		const auto variable = game::AllocVariable(&index);
		variable->w.type = game::SCRIPT_ARRAY;
		variable->u.f.prev = 0;
		variable->u.f.next = 0;

		return index;
	}
}
