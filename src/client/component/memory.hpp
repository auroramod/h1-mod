#pragma once
#include "loader/component_loader.hpp"

class memory final : public component_interface
{
public:
	void post_unpack() override;

	static constexpr auto custom_script_mem_size = 0x1000000ui64;

private:
	static void pmem_init();
	static void pmem_init_stub();
	static int out_of_memory_text_stub(char* dest, int size, const char* fmt, ...);
};
