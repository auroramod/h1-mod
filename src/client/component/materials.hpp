#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

class materials final : public component_interface
{
public:
	void post_unpack() override;

	static bool setup_material_image(game::Material* material, const std::string& data);
	static game::Material* create_material(const std::string& name);
	static void free_material(game::Material* material);

private:
	static int db_material_streaming_fail_stub(game::Material* material);
	static unsigned int db_get_material_index_stub(game::Material* material);

#ifdef _DEBUG
	static char material_compare_stub(unsigned int index_a, unsigned int index_b);
	static void print_material(const game::Material* material);
	static void print_current_material_stub(utils::hook::assembler& a);
	static void set_pixel_texture_stub(void* cmd_buf_state, unsigned int a2, const game::GfxImage* image);
#endif
};
