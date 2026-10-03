#pragma once
#include "loader/component_loader.hpp"

class ranked final : public component_interface
{
public:
	void post_unpack() override;
};
