#pragma once
#include "loader/component_loader.hpp"

class callstack final : public component_interface
{
public:
	void pre_destroy() override;

	static void print(int max_frames = 32);
	static std::string get_summary(const CONTEXT* context, int max_frames = 32);

private:
	static void init_symbols();
	static std::string format_frame(uint64_t address);
};
