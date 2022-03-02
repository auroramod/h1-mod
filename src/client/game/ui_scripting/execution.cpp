#include <std_include.hpp>
#include "execution.hpp"
#include "component/ui_scripting.hpp"

#include <utils/string.hpp>

namespace ui_scripting
{
	void push_value(const script_value& value)
	{
		const auto state = *game::hks::lua_state;
		const auto value_ = value.get_raw();
		*state->m_apistack.top = value_;
		state->m_apistack.top++;
	}

	script_value get_return_value(int offset)
	{
		const auto state = *game::hks::lua_state;
		return state->m_apistack.top[-1 - offset];
	}

	arguments get_return_values(int count)
	{
		arguments values;

		for (auto i = count - 1; i >= 0; i--)
		{
			values.push_back(get_return_value(i));
		}

		if (values.size() == 0)
		{
			values.push_back({});
		}

		return values;
	}

	arguments call_script_function(const function& function, const arguments& arguments)
	{
		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		push_value(function);
		for (auto i = arguments.begin(); i != arguments.end(); ++i)
		{
			push_value(*i);
		}

		const auto _1 = gsl::finally(&disable_error_hook);
		enable_error_hook();

		try
		{
			game::hks::vm_call_internal(state, static_cast<int>(arguments.size()), -1, 0);
			const auto count = static_cast<int>(state->m_apistack.top - state->m_apistack.base);
			return get_return_values(count);
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string("Error executing script function: ") + e.what());
		}
	}

	script_value get_field(const userdata& self, const script_value& key)
	{
		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		push_value(key);

		const auto _1 = gsl::finally(&disable_error_hook);
		enable_error_hook();

		game::hks::HksObject value{};
		game::hks::HksObject userdata{};
		userdata.t = game::hks::TUSERDATA;
		userdata.v.ptr = self.ptr;

		try
		{
			game::hks::hks_obj_gettable(&value, state, &userdata, &state->m_apistack.top[-1]);
			return value;
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string("Error getting userdata field: ") + e.what());
		}
	}

	script_value get_field(const table& self, const script_value& key)
	{
		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		push_value(key);

		const auto _1 = gsl::finally(&disable_error_hook);
		enable_error_hook();

		game::hks::HksObject value{};
		game::hks::HksObject userdata{};
		userdata.t = game::hks::TTABLE;
		userdata.v.ptr = self.ptr;

		try
		{
			game::hks::hks_obj_gettable(&value, state, &userdata, &state->m_apistack.top[-1]);
			return value;
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string("Error getting table field: ") + e.what());
		}
	}

	void set_field(const userdata& self, const script_value& key, const script_value& value)
	{
		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		const auto _1 = gsl::finally(&disable_error_hook);
		enable_error_hook();

		game::hks::HksObject userdata{};
		userdata.t = game::hks::TUSERDATA;
		userdata.v.ptr = self.ptr;

		try
		{
			game::hks::hks_obj_settable(state, &userdata, &key.get_raw(), &value.get_raw());
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string("Error setting userdata field: ") + e.what());
		}
	}

	void set_field(const table& self, const script_value& key, const script_value& value)
	{
		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		const auto _1 = gsl::finally(&disable_error_hook);
		enable_error_hook();

		game::hks::HksObject userdata{};
		userdata.t = game::hks::TTABLE;
		userdata.v.ptr = self.ptr;

		try
		{
			game::hks::hks_obj_settable(state, &userdata, &key.get_raw(), &value.get_raw());
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string("Error setting table field: ") + e.what());
		}
	}
}
