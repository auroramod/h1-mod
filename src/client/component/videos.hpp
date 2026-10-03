#pragma once
#include "loader/component_loader.hpp"

class videos final : public component_interface
{
public:
	void post_unpack() override;

private:
	static void playvid(const char* name, const int a2, const int a3);
};
