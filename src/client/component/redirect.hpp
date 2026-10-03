#pragma once
#include "loader/component_loader.hpp"

class redirect final : public component_interface
{
public:
	void* load_import(const std::string& library, const std::string& function) override;

private:
	static void launch_complementary_game(const bool singleplayer, const std::string& mode = "");
	static HINSTANCE shell_execute_a(const HWND hwnd, const LPCSTR operation, const LPCSTR file, const LPCSTR parameters, const LPCSTR directory, const INT show_cmd);
};
