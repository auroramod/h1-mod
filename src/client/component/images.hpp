#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/image.hpp>

class images final : public component_interface
{
public:
	void post_unpack() override;

	static void override_texture(std::string name, std::string data);

private:
	static std::optional<std::string> load_image(game::GfxImage* image);
	static std::optional<utils::image> load_raw_image_from_file(game::GfxImage* image);
	static bool load_custom_texture(game::GfxImage* image);
	static void load_texture_stub(game::GfxImage* image, void* a2, int* a3);
	static int setup_texture_stub(game::GfxImage* image, void* a2, void* a3);
};
