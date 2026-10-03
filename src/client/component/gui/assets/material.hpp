#ifdef _DEBUG
#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class asset_material final : public component_interface
{
public:
	void post_unpack() override;

private:
	static std::string get_image_type_name(unsigned char type);
	static void copy_constant_table_to_cbt(game::Material* mat);
	static bool draw_material_window(game::Material* asset);
};
#endif
