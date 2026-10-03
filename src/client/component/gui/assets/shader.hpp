#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_shader final : public component_interface
{
public:
	void post_unpack() override;

private:
	static std::string disassemble_shader(ID3D11DeviceChild* shader);
	template <typename T> static std::string& get_disassembled_shader(T* asset, bool force = false);
	template <typename T> static bool draw_asset(T* asset);
};
#endif
