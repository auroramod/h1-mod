#pragma once
#include "loader/component_loader.hpp"

class splash final : public component_interface
{
public:
	void post_start() override;
	void post_load() override;
	void post_unpack() override;
	void pre_destroy() override;

private:
	HWND window_{};
	HANDLE image_{};

	static void destroy_stub();

	void destroy() const;
	void show();
};
