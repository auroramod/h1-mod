#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class script_error final : public component_interface
{
public:
	void post_unpack() override;
	void pre_destroy() override;

	static std::optional<std::pair<std::string, std::string>> find_function(const char* pos);

private:
	static void scr_emit_function_stub(std::uint32_t filename, std::uint32_t thread_name, char* code_pos);
	static std::string get_filename_name();
	static void get_unknown_function_error(const char* code_pos);
	static void get_unknown_function_error(std::uint32_t thread_name);
	static void script_link_error();
	static void compile_error_stub(const char* code_pos, const char* msg);
	static std::uint32_t find_variable_stub(std::uint32_t parent_id, std::uint32_t thread_name);
	static unsigned int scr_get_object(unsigned int index);
	static unsigned int scr_get_const_string(unsigned int index);
	static unsigned int scr_get_const_istring(unsigned int index);
	static void scr_validate_localized_string_ref(int parm_index, const char* token, int token_len);
	static void scr_get_vector(unsigned int index, float* vector_value);
	static int scr_get_int(unsigned int index);
	static float scr_get_float(unsigned int index);
	static int scr_get_pointer_type(unsigned int index);
	static int scr_get_type(unsigned int index);
	static const char* scr_get_type_name(unsigned int index);

	template <size_t address>
	static void safe_func();
};
