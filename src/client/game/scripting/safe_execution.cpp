#include <std_include.hpp>
#include "safe_execution.hpp"

#pragma warning(push)
#pragma warning(disable: 4611)

namespace scripting::safe_execution
{
	namespace
	{
		bool execute_with_seh(const script_function function, const game::scr_entref_t& entref)
		{
			__try
			{
				function(entref);
				return true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return false;
			}
		}
	}

	bool call(const script_function function, const game::scr_entref_t& entref)
	{
		*game::g_script_error_level += 1;
		if (game::_setjmp(&game::g_script_error[*game::g_script_error_level]))
		{
			*game::g_script_error_level -= 1;
			return false;
		}

		const auto result = execute_with_seh(function, entref);
		*game::g_script_error_level -= 1;
		return result;
	}

	bool set_entity_field(const game::scr_entref_t& entref, const int offset)
	{
		*game::g_script_error_level += 1;
		if (game::_setjmp(&game::g_script_error[*game::g_script_error_level]))
		{
			*game::g_script_error_level -= 1;
			return false;
		}

		game::Scr_SetObjectField(entref.classnum, entref.entnum, offset);

		*game::g_script_error_level -= 1;
		return true;
	}

	bool get_entity_field(const game::scr_entref_t& entref, const int offset, game::VariableValue* value)
	{
		*game::g_script_error_level += 1;
		if (game::_setjmp(&game::g_script_error[*game::g_script_error_level]))
		{
			value->type = game::SCRIPT_NONE;
			value->u.intValue = 0;
			*game::g_script_error_level -= 1;
			return false;
		}

		game::GetEntityFieldValue(value, entref.classnum, entref.entnum, offset);

		*game::g_script_error_level -= 1;
		return true;
	}
}

#pragma warning(pop)
