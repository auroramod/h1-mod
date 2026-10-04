#pragma once
#include "game/game.hpp"
#include "game/scripting/array.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/function.hpp"
#include <xsk/gsc/engine/h1.hpp>
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
#pragma pack(push, 1)
	struct dev_map_instruction
	{
		std::uint32_t codepos;
		std::uint16_t line;
		std::uint16_t col;
	};
	struct dev_map
	{
		std::uint32_t num_instructions;
		dev_map_instruction instructions[1];
	};
#pragma pack(pop)
	struct devmap_entry
	{
		const std::uint8_t* bytecode;
		std::size_t size;
		std::string script_name;
		std::vector<dev_map_instruction> devmap;
	};
}

namespace script_extension
{
	extern gsc::builtin_function func_table[0x1000];
	extern gsc::builtin_method meth_table[0x1000];

	extern const game::dvar_t* developer_script;

	void add_devmap_entry(std::uint8_t*, std::size_t, const std::string&, xsk::gsc::buffer);
	void clear_devmap();

	void scr_error(const char* error, const bool force_print = false);

	void add_function(const std::string& name, gsc::script_function function);
	void add_method(const std::string& name, gsc::script_method method);
}
