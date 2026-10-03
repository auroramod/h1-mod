#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "images.hpp"
#include "console.hpp"
#include "filesystem.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/image.hpp>
#include <utils/io.hpp>
#include <utils/concurrency.hpp>

static utils::hook::detour load_texture_hook;
static utils::hook::detour setup_texture_hook;
static utils::concurrency::container<std::unordered_map<std::string, std::string>> overriden_textures;

void images::post_unpack()
{
	if (game::environment::is_dedi())
	{
		return;
	}

	setup_texture_hook.create(SELECT_VALUE(0x140083300, 0x14008C320), setup_texture_stub); // Setup_Texture
	load_texture_hook.create(SELECT_VALUE(0x140082050, 0x14008B190), load_texture_stub);
	//load_texture_hook.create(SELECT_VALUE(0x14055F870, 0x1405DC050), load_texture_stub);
}

void images::override_texture(std::string name, std::string data)
{
	overriden_textures.access([&](std::unordered_map<std::string, std::string>& textures)
	{
		textures[std::move(name)] = std::move(data);
	});
}

std::optional<std::string> images::load_image(game::GfxImage* image)
{
	std::string data{};
	overriden_textures.access([&](const std::unordered_map<std::string, std::string>& textures)
	{
		if (const auto i = textures.find(image->name); i != textures.end())
		{
			data = i->second;
		}
	});

	if (data.empty() && !filesystem::read_file(utils::string::va("images/%s.png", image->name), &data))
	{
		return {};
	}

	return {std::move(data)};
}

std::optional<utils::image> images::load_raw_image_from_file(game::GfxImage* image)
{
	const auto image_file = load_image(image);
	if (!image_file)
	{
		return {};
	}

	return utils::image(*image_file);
}

bool images::load_custom_texture(game::GfxImage* image)
{
	auto raw_image = load_raw_image_from_file(image);
	if (!raw_image)
	{
		return false;
	}

	image->mapType = game::MAPTYPE_2D;
	image->semantic = 2;
	image->category = 3;
	image->flags = 0;

	D3D11_SUBRESOURCE_DATA data{};
	data.SysMemPitch = raw_image->get_width() * 4;
	data.SysMemSlicePitch = data.SysMemPitch * raw_image->get_height();
	data.pSysMem = raw_image->get_buffer();

	game::Image_Setup(image, raw_image->get_width(), raw_image->get_height(), image->depth, image->numElements,
		image->mapType, DXGI_FORMAT_R8G8B8A8_UNORM, image->name, &data);

	return true;
}

void images::load_texture_stub(game::GfxImage* image, void* a2, int* a3)
//void load_texture_stub(void* a1, game::GfxImage* image)
{
	try
	{
		if (load_custom_texture(image))
		{
			return;
		}
	}
	catch (std::exception& e)
	{
		console::error("Failed to load image %s: %s\n", image->name, e.what());
	}

	load_texture_hook.invoke<void>(image, a2, a3);
	//load_texture_hook.invoke<void>(a1, image);
}

int images::setup_texture_stub(game::GfxImage* image, void* a2, void* a3)
{
	if (*(int*)&image->picmip == -1) // resourceSize
	{
		return 0;
	}

	return setup_texture_hook.invoke<bool>(image, a2, a3);
}

REGISTER_COMPONENT(images)
