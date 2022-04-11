#include <std_include.hpp>
#include "types.hpp"
#include "execution.hpp"

namespace ui_scripting
{
	/***************************************************************
	 * Lightuserdata
	 **************************************************************/

	lightuserdata::lightuserdata(void* ptr_)
		: ptr(ptr_)
	{
	}

	bool lightuserdata::operator==(const lightuserdata& other)
	{
		return this->ptr == other.ptr;
	}

	bool lightuserdata::operator!=(const lightuserdata& other)
	{
		return this->ptr != other.ptr;
	}

	/***************************************************************
	 * Userdata
	 **************************************************************/

	userdata::userdata(void* ptr_)
		: ptr(ptr_)
	{
		this->add();
	}

	userdata::userdata(const userdata& other)
	{
		this->operator=(other);
	}

	userdata::userdata(userdata&& other) noexcept
	{
		this->ptr = other.ptr;
		this->ref = other.ref;
		other.ref = 0;
	}

	userdata::~userdata()
	{
		this->release();
	}

	userdata& userdata::operator=(const userdata& other)
	{
		if (&other != this)
		{
			this->release();
			this->ptr = other.ptr;
			this->ref = other.ref;
			this->add();
		}

		return *this;
	}

	userdata& userdata::operator=(userdata&& other) noexcept
	{
		if (&other != this)
		{
			this->release();
			this->ptr = other.ptr;
			this->ref = other.ref;
			other.ref = 0;
		}

		return *this;
	}

	bool userdata::operator==(const userdata& other)
	{
		return this->ptr == other.ptr;
	}

	bool userdata::operator!=(const userdata& other)
	{
		return this->ptr != other.ptr;
	}

	void userdata::add()
	{
		game::hks::HksObject value{};
		value.v.ptr = this->ptr;
		value.t = game::hks::TUSERDATA;

		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		push_value(value);

		this->ref = game::hks::hksi_luaL_ref(*game::hks::lua_state, -10000);
	}

	void userdata::release()
	{
		if (this->ref)
		{
			game::hks::hksi_luaL_unref(*game::hks::lua_state, -10000, this->ref);
		}
	}

	void userdata::set(const script_value& key, const script_value& value) const
	{
		set_field(*this, key, value);
	}

	script_value userdata::get(const script_value& key) const
	{
		return get_field(*this, key);
	}

	/***************************************************************
	 * Table
	 **************************************************************/

	table::table()
	{
		const auto state = *game::hks::lua_state;
		this->ptr = game::hks::Hashtable_Create(state, 0, 0);
		this->add();
	}

	table::table(game::hks::HashTable* ptr_)
		: ptr(ptr_)
	{
		this->add();
	}

	table::table(const table& other)
	{
		this->operator=(other);
	}

	table::table(table&& other) noexcept
	{
		this->ptr = other.ptr;
		this->ref = other.ref;
		other.ref = 0;
	}

	table::~table()
	{
		this->release();
	}

	table& table::operator=(const table& other)
	{
		if (&other != this)
		{
			this->release();
			this->ptr = other.ptr;
			this->ref = other.ref;
			this->add();
		}

		return *this;
	}

	table& table::operator=(table&& other) noexcept
	{
		if (&other != this)
		{
			this->release();
			this->ptr = other.ptr;
			this->ref = other.ref;
			other.ref = 0;
		}

		return *this;
	}

	bool table::operator==(const table& other)
	{
		return this->ptr == other.ptr;
	}

	bool table::operator!=(const table& other)
	{
		return this->ptr != other.ptr;
	}

	void table::add()
	{
		game::hks::HksObject value{};
		value.v.table = this->ptr;
		value.t = game::hks::TTABLE;

		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		push_value(value);

		this->ref = game::hks::hksi_luaL_ref(*game::hks::lua_state, -10000);
	}

	void table::release()
	{
		if (this->ref)
		{
			game::hks::hksi_luaL_unref(*game::hks::lua_state, -10000, this->ref);
		}
	}

	void table::set(const script_value& key, const script_value& value) const
	{
		set_field(*this, key, value);
	}

	script_value table::get(const script_value& key) const
	{
		return get_field(*this, key);
	}

	/***************************************************************
	 * Function
	 **************************************************************/

	function::function(game::hks::cclosure* ptr_, game::hks::HksObjectType type_)
		: ptr(ptr_)
		, type(type_)
	{
		this->add();
	}

	function::function(const function& other)
	{
		this->operator=(other);
	}

	function::function(function&& other) noexcept
	{
		this->ptr = other.ptr;
		this->type = other.type;
		this->ref = other.ref;
		other.ref = 0;
	}

	function::~function()
	{
		this->release();
	}

	function& function::operator=(const function& other)
	{
		if (&other != this)
		{
			this->release();
			this->ptr = other.ptr;
			this->type = other.type;
			this->ref = other.ref;
			this->add();
		}

		return *this;
	}

	function& function::operator=(function&& other) noexcept
	{
		if (&other != this)
		{
			this->release();
			this->ptr = other.ptr;
			this->type = other.type;
			this->ref = other.ref;
			other.ref = 0;
		}

		return *this;
	}

	bool function::operator==(const function& other)
	{
		return this->ptr == other.ptr;
	}

	bool function::operator!=(const function& other)
	{
		return this->ptr != other.ptr;
	}

	void function::add()
	{
		game::hks::HksObject value{};
		value.v.cClosure = this->ptr;
		value.t = this->type;

		const auto state = *game::hks::lua_state;
		state->m_apistack.top = state->m_apistack.base;

		push_value(value);

		this->ref = game::hks::hksi_luaL_ref(*game::hks::lua_state, -10000);
	}

	void function::release()
	{
		if (this->ref)
		{
			game::hks::hksi_luaL_unref(*game::hks::lua_state, -10000, this->ref);
		}
	}

	arguments function::call(const arguments& arguments) const
	{
		return call_script_function(*this, arguments);
	}

	/***************************************************************
     * Stack
     **************************************************************/

    stack::stack()
    {
        this->state = *game::hks::lua_state;
        this->state->m_apistack.top = this->state->m_apistack.base;
    }

    void stack::save(int num_args)
    {
        this->num_args_ = num_args;
        this->num_calls_ = state->m_numberOfCCalls;
        this->base_bottom_ = state->m_apistack.base - state->m_apistack.bottom;
        this->top_bottom_ = state->m_apistack.top - state->m_apistack.bottom;
        this->callstack_ = state->m_callStack.m_current - state->m_callStack.m_records;
    }

    void stack::fix()
    {
        this->state->m_numberOfCCalls = this->num_calls_;

        game::hks::closePendingUpvalues(this->state, &this->state->m_apistack.bottom[this->top_bottom_ - this->num_args_]);
        this->state->m_callStack.m_current = &this->state->m_callStack.m_records[this->callstack_];

        this->state->m_apistack.base = &this->state->m_apistack.bottom[this->base_bottom_];
        this->state->m_apistack.top = &this->state->m_apistack.bottom[this->top_bottom_ - static_cast<uint64_t>(this->num_args_ + 1)];

        this->state->m_apistack.bottom[this->top_bottom_].t = this->state->m_apistack.top[-1].t;
        this->state->m_apistack.bottom[this->top_bottom_].v.ptr = this->state->m_apistack.top[-1].v.ptr;
        this->state->m_apistack.top = &this->state->m_apistack.bottom[this->top_bottom_ + 1];
    }
}
