#pragma once
#include "loader/component_loader.hpp"

class resources final : public component_interface
{
public:
	~resources() override;

	void post_start() override;
	void* load_import(const std::string& library, const std::string& function) override;

private:
	static HANDLE WINAPI load_image_a(const HINSTANCE handle, LPCSTR name, const UINT type, const int c_x, const int c_y,
		const UINT load);
	static HICON WINAPI load_icon_a(const HINSTANCE handle, const LPCSTR name);
};
