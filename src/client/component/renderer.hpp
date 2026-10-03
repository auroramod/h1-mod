#pragma once
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>

class renderer final : public component_interface
{
public:
	void post_unpack() override;

private:
	static int get_fullbright_technique();
	static void gfxdrawmethod();
	static void r_init_draw_method_stub();
	static bool r_update_front_end_dvar_options_stub();
	static void set_tonemap_highlight_range();
	static int db_ready_inside_loadxassets_stub();
	static int get_red_dot_brightness();
	static void* get_tonemap_highlight_range_stub();
	static void r_preload_shaders_stub_sp(utils::hook::assembler& a);
	static void r_preload_shaders_stub_mp(utils::hook::assembler& a);
	static void r_get_gfx_ent_index_stub(utils::hook::assembler& a);
};
