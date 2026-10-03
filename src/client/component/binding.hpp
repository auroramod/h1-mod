#pragma once
#include "loader/component_loader.hpp"

class binding final : public component_interface
{
public:
	void post_unpack() override;

private:
	static int get_num_keys();
	static int key_write_bindings_to_buffer_stub(int local_client_num, char* buffer, const int buffer_size);
	static int get_binding_for_custom_command(const char* command);
	static int key_get_binding_for_cmd_stub(const char* command);
	static std::optional<std::string> get_custom_binding_for_key(int key);
	static void cl_execute_key_stub(const int local_client_num, int key, const int down, const unsigned int time);
	static const char* cmd_get_binding_for_key_stub(unsigned int key);
};
