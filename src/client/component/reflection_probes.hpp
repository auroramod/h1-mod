#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/memory.hpp>

class reflection_probes final : public component_interface
{
public:
	void post_unpack() override;

private:
	static std::string clean_name(const std::string& name);
	static void dump_image_dds(game::GfxImage* image);
	static void disableDvars();
	static void restoreDvars();
	static void cg_calc_cubemap_view_values_stub(game::refdef_t* refdef, int cubemapShot, int cubemapSize, int unk);
	static inline game::GfxImage* R_GenerateReflectionImage(int index, unsigned char* pixels, unsigned int datalen, unsigned int size, utils::memory::allocator* allocator);
	static void R_GenerateReflections(const char* name, game::GfxReflectionProbe* probes, unsigned int count, unsigned int size = 128);
	static void scr_update_frame_stub();
	static int cl_cgame_rendering_stub(int localClientNum, int a2);
	static void cg_load_light_set_stub(const char* name);
};
