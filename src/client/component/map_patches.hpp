#pragma once
#include "loader/component_loader.hpp"

class map_patches final : public component_interface
{
public:
	void post_unpack() override;
};
