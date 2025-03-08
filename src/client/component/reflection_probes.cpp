#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "console.hpp"
#include "command.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#define DUMP_DDS

namespace reflection_probes
{
	namespace
	{
		std::string clean_name(const std::string& name)
		{
			auto new_name = name;

			for (auto i = 0u; i < name.size(); i++)
			{
				switch (new_name[i])
				{
				case '*':
					new_name[i] = '_';
					break;
				}
			}

			return new_name;
		}

		void dump_image_dds(game::GfxImage* image)
		{
			if (image->streamed)
			{
				return;
			}

			auto* data = image->pixelData;
			std::size_t data_used = 0;

			const auto sides = image->mapType == game::MAPTYPE_CUBE ? 6 : 1;

			std::vector<DirectX::Image> images{};
			for (auto idx = 0; idx < image->numElements; idx++)
			{
				for (auto s = 0; s < sides; s++)
				{
					for (auto d = 0; d < image->depth; d++)
					{
						int divider = 1;
						for (auto i = 0; i < image->levelCount; i++)
						{
							DirectX::Image img{};
							img.pixels = data;
							img.width = image->width / divider;
							img.height = image->height / divider;
							img.format = DXGI_FORMAT(image->imageFormat);

							size_t rowPitch;
							size_t slicePitch;
							DirectX::ComputePitch(img.format, img.width, img.height, rowPitch, slicePitch);

							img.rowPitch = rowPitch;
							img.slicePitch = slicePitch;

							images.push_back(img);

							data += slicePitch;
							data_used += slicePitch;
							divider += divider;
						}
					}
				}
			}

			if (image->dataLen1 != 4 && image->width != 1 && image->height != 1) // default asset, uses wrong values for some reason
			{
				assert(data_used == image->dataLen1);
			}

			DirectX::TexMetadata mdata{};
			mdata.width = image->width;
			mdata.height = image->height;
			mdata.depth = image->depth;
			mdata.arraySize = image->numElements * sides;
			mdata.mipLevels = image->levelCount;
			mdata.format = DXGI_FORMAT(image->imageFormat);
			mdata.dimension = image->mapType > 4 ? DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D : (DirectX::TEX_DIMENSION)image->mapType;

			if (image->mapType == game::MAPTYPE_CUBE)
			{
				mdata.miscFlags |= DirectX::TEX_MISC_FLAG::TEX_MISC_TEXTURECUBE;
			}

			std::string parent_path = "dump\\"s + game::Dvar_FindVar("mapname")->current.string + "\\images\\"s;
			std::string spath = parent_path + clean_name(image->name) + ".dds";
			std::wstring wpath(spath.begin(), spath.end());

			if (!std::filesystem::exists(parent_path))
			{
				std::filesystem::create_directories(parent_path);
			}

			auto result = DirectX::SaveToDDSFile(images.data(), images.size(), mdata, DirectX::DDS_FLAGS_NONE, wpath.data());
			if (FAILED(result))
			{
				console::error("Failed to dump image \"%s\"", spath.data());
			}
		}
	}

	namespace
	{
		// Function to flip an RGBA image diagonally
		void flip_diagonally(uint8_t* image, int width, int height) {
			int channels = 4; // Number of channels for RGBA
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					if (i < j) {
						for (int k = 0; k < channels; ++k) {
							std::swap(image[(i * width + j) * channels + k], image[(j * width + i) * channels + k]);
						}
					}
				}
			}
		}

		// Function to flip an RGBA image horizontally
		void flip_horizontally(uint8_t* image, int width, int height) {
			int channels = 4; // Number of channels for RGBA
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width / 2; ++j) {
					for (int k = 0; k < channels; ++k) {
						std::swap(image[(i * width + j) * channels + k], image[(i * width + (width - 1 - j)) * channels + k]);
					}
				}
			}
		}

		// Function to flip an RGBA image vertically
		void flip_vertically(uint8_t* image, int width, int height) {
			int channels = 4; // Number of channels for RGBA
			for (int i = 0; i < height / 2; ++i) {
				for (int j = 0; j < width; ++j) {
					for (int k = 0; k < channels; ++k) {
						std::swap(image[(i * width + j) * channels + k], image[((height - 1 - i) * width + j) * channels + k]);
					}
				}
			}
		}
	}

	namespace
	{
		const char* cubemap_suffix[] = 
		{
			"_rf",
			"_lf",
			"_bk",
			"_ft",
			"_up",
			"_dn"
		};

		std::vector<std::string> dvar_names =
		{
			"fx_enable",
			"cg_draw2d",
			"cg_drawgun",
			"branding",
		};
		int dvar_values[3];

		void disableDvars()
		{
			for (auto i = 0; i < dvar_names.size(); i++)
			{
				auto* dvar = game::Dvar_FindVar(dvar_names[i].data());
				if (dvar)
				{
					dvar_values[i] = dvar->current.integer;
					dvar->current.integer = 0;
				}
			}
		}

		void restoreDvars()
		{
			for (auto i = 0; i < dvar_names.size(); i++)
			{
				auto* dvar = game::Dvar_FindVar(dvar_names[i].data());
				if (dvar)
				{
					dvar->current.integer = dvar_values[i];
				}
			}
		}

		const char* basename = nullptr;
		float probe_origin[3];
		int do_render;

		utils::hook::detour cg_calc_cubemap_view_values_hook;
		utils::hook::detour scr_update_frame_hook;
		utils::hook::detour cl_cgame_rendering_hook;
		utils::hook::detour cg_load_light_set_hook;

		void cg_calc_cubemap_view_values_stub(game::mp::refdef_t* refdef, int cubemapShot, int cubemapSize, int unk)
		{
			if (dvars::r_reflectionProbeGenerate->current.enabled)
			{
				refdef->view.org[0] = probe_origin[0];
				refdef->view.org[1] = probe_origin[1];
				refdef->view.org[2] = probe_origin[2];

				refdef->view.tanHalfFovX = 1.0078125f;
				refdef->view.tanHalfFovY = 1.0078125f;
				refdef->view.unk[0] = 1.0078125f;
				refdef->view.unk[1] = 1.0078125f;

				unk = 1;
			}
			
			cg_calc_cubemap_view_values_hook.invoke<void>(refdef, cubemapShot, cubemapSize, unk);
		}

		inline game::GfxImage* R_GenerateReflectionImage(int index, unsigned char* pixels, unsigned int datalen, unsigned int size, utils::memory::allocator* allocator)
		{
			auto name = utils::string::va("*reflection_probe%i", index);
			auto image = allocator->allocate<game::GfxImage>();
			memset(image, 0, sizeof(game::GfxImage));
			
			image->name = allocator->duplicate_string(name);
			image->imageFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			image->mapType = game::MapType::MAPTYPE_CUBE;
			image->semantic = game::TS_2D;
			image->category = game::IMG_CATEGORY_AUTO_GENERATED;
			image->flags = game::IMAGE_FLAG_NOMIPMAPS;
			image->dataLen1 = datalen;
			image->dataLen2 = datalen;
			image->width = static_cast<unsigned short>(size);
			image->height = static_cast<unsigned short>(size);
			image->depth = 1;
			image->numElements = 1;
			image->levelCount = 1;
			image->streamed = false;
			image->pixelData = pixels;

			return image;
		}

		void R_GenerateReflections(const char* name, game::GfxReflectionProbe* probes, unsigned int count, unsigned int size = 128)
		{
			if (!count) return;

			disableDvars();

			auto cg = game::CG_GetLocalClientGlobals();
			[[maybe_unused]] auto ent = &game::mp::g_entities[0];
			assert(ent->client);

			for (auto probe_index = 0u; probe_index < count; probe_index++)
			{
				auto probe = &probes[probe_index];

				utils::memory::allocator allocator;

				const auto num_channels = 4; // rgba
				auto pixels_size = num_channels * size * size;
				auto pixel_buffer_size = pixels_size * 6; // cube map has 6 images
				auto pixel_buffer = allocator.allocate_array<unsigned char>(pixel_buffer_size);

				for (int shot = game::mp::CUBEMAPSHOT_RIGHT; shot < game::mp::CUBEMAPSHOT_COUNT; shot++)
				{
					cg->cubemapShot = (game::mp::CubemapShot)shot;
					cg->cubemapSize = size;

					probe_origin[0] = probe->origin[0];
					probe_origin[1] = probe->origin[1];
					probe_origin[2] = probe->origin[2];

					const auto generate = [&]()
					{
						game::R_BeginFrame();

						if (!game::CG_DrawActiveFrame(0, game::mp::client_state[0]->serverTime, 0, shot, size, 1, 3))
						{
							__debugbreak();
						}

						*reinterpret_cast<int*>(0x392E8BC_b) = 3;
						game::R_EndFrame();
						game::R_IssueRenderCommands(-1);
					};

					const auto generate_fixed = [&]()
					{
						// generate this multiple times, since game is broke af :)
						for (auto i = 0; i < 0x10; i++)
						{
							generate();
						}
					};

					generate_fixed();

					game::R_TakeScreenshot(0, 1, 0, 0, size, size, 3, &pixel_buffer[pixels_size * (shot - 1)]);

					auto old_data = &pixel_buffer[pixels_size * (shot - 1)];
					auto new_data = allocator.allocate_array<unsigned char>(pixels_size);
					auto ii = 0;
					for (auto i = 0u; i < 3 * size * size;)
					{
						new_data[ii + 0] = old_data[i + 0];
						new_data[ii + 1] = old_data[i + 1];
						new_data[ii + 2] = old_data[i + 2];
						new_data[ii + 3] = 255;

						ii += 4;
						i += 3;
					}
					memcpy(old_data, new_data, pixels_size);
					allocator.free(new_data);
				}

#ifdef DUMP_DDS
				flip_diagonally(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_RIGHT - 1)], size, size);
				flip_diagonally(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_LEFT - 1)], size, size);
				flip_horizontally(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_LEFT - 1)], size, size);
				flip_vertically(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_LEFT - 1)], size, size);
				flip_vertically(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_BACK - 1)], size, size);
				flip_horizontally(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_FRONT - 1)], size, size);
				flip_diagonally(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_UP - 1)], size, size);
				flip_diagonally(&pixel_buffer[pixels_size * (game::mp::CUBEMAPSHOT_DOWN - 1)], size, size);

				auto* image = R_GenerateReflectionImage(probe_index + 1, pixel_buffer, pixel_buffer_size, size, &allocator);
				dump_image_dds(image);
#endif
			}

			restoreDvars();

			dvars::r_reflectionProbeGenerate->current.enabled = false;
		}

		void scr_update_frame_stub()
		{
			scr_update_frame_hook.invoke<void>();

			if (dvars::r_reflectionProbeGenerate->current.enabled && do_render)
			{
				const auto client_state = game::mp::client_state[0];
				if (!client_state)
				{
					return;
				}
				auto serverTime = game::mp::client_state[0]->serverTime;
				if (game::CL_IsCgameInitialized() && !game::VirtualLobby_Loaded() && serverTime > 1000)
				{
					const auto* gfxworld = *game::s_world;
					const auto count = gfxworld->draw.reflectionProbeCount - 1;
					const auto probes = gfxworld->draw.reflectionProbeOrigins + 1;
					basename = gfxworld->name;
					R_GenerateReflections(gfxworld->name, probes, count);
					return;
				}
			}
		}

		int cl_cgame_rendering_stub(int localClientNum, int a2)
		{
			auto result = cl_cgame_rendering_hook.invoke<int>(localClientNum, a2);
			do_render = result;
			return result;
		}

		void cg_load_light_set_stub(const char* name)
		{
			// don't do lightsets when generating reflection probes
			if (dvars::r_reflectionProbeGenerate->current.enabled)
			{
				return;
			}

			cg_load_light_set_hook.invoke<void>(name);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			dvars::r_reflectionProbeGenerate = dvars::register_bool("r_reflectionProbeGenerate", false, game::DVAR_FLAG_NONE, "Generate cube maps for reflection probes.");

			cl_cgame_rendering_hook.create(0x3433A0_b, cl_cgame_rendering_stub);
			scr_update_frame_hook.create(0x343830_b, scr_update_frame_stub);
			cg_calc_cubemap_view_values_hook.create(0x6A2D80_b, cg_calc_cubemap_view_values_stub);
			cg_load_light_set_hook.create(0x1090B0_b, cg_load_light_set_stub);
		}
	};
}

REGISTER_COMPONENT(reflection_probes::component)
