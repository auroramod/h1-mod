#pragma once
#include "loader/component_loader.hpp"

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

class script_extension final : public component_interface
{
public:
	void post_unpack() override;

	static gsc::builtin_function func_table[0x1000];
	static gsc::builtin_method meth_table[0x1000];

	static const game::dvar_t* developer_script;

	static void add_devmap_entry(std::uint8_t*, std::size_t, const std::string&, xsk::gsc::buffer);
	static void clear_devmap();

	static void scr_error(const char* error, const bool force_print = false);

	static void add_function(const std::string& name, gsc::script_function function);
	static void add_method(const std::string& name, gsc::script_method method);

private:
	static std::optional<gsc::devmap_entry> get_devmap_entry(const std::uint8_t* codepos);
	static std::optional<std::pair<std::uint16_t, std::uint16_t>> get_line_and_col_for_codepos(const std::uint8_t* codepos);
	static gsc::function_args get_arguments();
	static void return_value(const scripting::script_value& value);
	static std::uint16_t get_function_id();
	static void set_function_id(std::uint32_t id);
	static game::scr_entref_t get_entity_id_stub(std::uint32_t ent_id);
	static void execute_custom_function(const std::uint16_t id);
	static void vm_call_builtin_function_stub(gsc::builtin_function func);
	static void vm_call_builtin_function_stub_mp();
	static void execute_custom_method(const std::uint16_t id);
	static void vm_call_builtin_method_stub(gsc::builtin_method meth);
	static void vm_call_builtin_method_stub_mp();
	static void builtin_call_error(const std::string& error);
	static std::optional<std::string> get_opcode_name(const std::uint8_t opcode);
	static void print_callstack();
	static void vm_error_stub(int mark_pos);
	static void print(const gsc::function_args& args);
	static scripting::script_value typeof(const gsc::function_args& args);
	static void* store_func_id_stub();
	static void* store_func_id_pointer_stub();
	static void* store_method_id_stub();
};
