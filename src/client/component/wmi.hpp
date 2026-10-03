#pragma once
#include "loader/component_loader.hpp"

class wmi final : public component_interface
{
public:
	void post_load() override;
	void post_unpack() override;
	void* load_import(const std::string& library, const std::string& function) override;

private:
	static HRESULT WINAPI co_initialize_ex_stub(LPVOID pvReserved, DWORD dwCoInit);
};
