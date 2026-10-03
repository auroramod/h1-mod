#pragma once
#include "loader/component_loader.hpp"

class dedicated_info final : public component_interface
{
public:
	void post_unpack() override;
};
