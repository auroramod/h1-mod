#pragma once
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>

class virtuallobby final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void get_fovscale_stub(utils::hook::assembler& a);
};
