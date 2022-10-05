#pragma once

#include "game/scripting/script_value.hpp"

namespace gsc
{
	class function_args
	{
	public:
		function_args(std::vector<scripting::script_value>);

		unsigned int size() const;
		std::vector<scripting::script_value> get_raw() const;
		scripting::value_wrap get(const int index) const;

		scripting::value_wrap operator[](const int index) const
		{
			return this->get(index);
		}
	private:
		std::vector<scripting::script_value> values_;
	};

	using builtin_function = void(*)();
	using builtin_method = void(*)(game::scr_entref_t);

	using script_function = std::function<scripting::script_value(const function_args&)>;
	using script_method = std::function<scripting::script_value(const game::scr_entref_t, const function_args&)>;

	extern builtin_function func_table[0x1000];
	extern builtin_method meth_table[0x1000];

	game::ScriptFile* find_script(game::XAssetType /*type*/, const char* name, int /*allow_create_default*/);

	void load_main_handles();
	void load_init_handles();

	scripting::script_value get_argument(int index);

	namespace function
	{
		void add(const std::string& name, script_function function);
	}

	namespace method
	{
		void add(const std::string& name, script_method function);
	}
}
