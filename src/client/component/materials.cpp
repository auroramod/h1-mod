#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "materials.hpp"
#include "console.hpp"
#include "filesystem.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/image.hpp>
#include <utils/concurrency.hpp>

namespace materials
{
	namespace
	{
		utils::hook::detour db_material_streaming_fail_hook;
		utils::hook::detour db_get_material_index_hook;

#ifdef DEBUG
		utils::hook::detour material_compare_hook;
		utils::hook::detour set_pixel_texture_hook;
		utils::hook::detour r_draw_triangles_lit_hook;

		const game::dvar_t* debug_materials = nullptr;
#endif

		game::MaterialConstantDef constant_table{};
		
		int db_material_streaming_fail_stub(game::Material* material)
		{
			if (material->constantTable == &constant_table)
			{
				return 0;
			}

			return db_material_streaming_fail_hook.invoke<int>(material);
		}

		unsigned int db_get_material_index_stub(game::Material* material)
		{
			if (material->constantTable == &constant_table)
			{
				return 0;
			}

			return db_get_material_index_hook.invoke<unsigned int>(material);
		}

#ifdef DEBUG
		char material_compare_stub(unsigned int index_a, unsigned int index_b)
		{
			char result = 0;

			__try
			{
				result = material_compare_hook.invoke<char>(index_a, index_b);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				const auto* material_a = utils::hook::invoke<game::Material*>(0x395FE0_b, index_a);
				const auto* material_b = utils::hook::invoke<game::Material*>(0x395FE0_b, index_b);
				console::error("Material_Compare: %s - %s (%d - %d)\n", 
					material_a->name, material_b->name, material_a->info.sortKey, material_b->info.sortKey);
			}

			return result;
		}

		void print_material(const game::Material* material)
		{
			if (!debug_materials || !debug_materials->current.enabled)
			{
				return;
			}

			console::debug("current material is \"%s\"\n", material->name);
		}

		void print_current_material_stub(utils::hook::assembler& a)
		{
			const auto loc_6AD59B = a.newLabel();

			a.pushad64();
			a.mov(rcx, r15);
			a.call_aligned(print_material);
			a.popad64();

			a.cmp(byte_ptr(rbx), 5);
			a.mov(rax, ptr(r15, 0x130));

			a.jnz(loc_6AD59B);
			a.nop(dword_ptr(rax, rax, 0x00000000));

			a.jmp(0x6AD570_b);

			a.bind(loc_6AD59B);
			a.jmp(0x6AD59B_b);
		}

		void set_pixel_texture_stub(void* cmd_buf_state, unsigned int a2, const game::GfxImage* image)
		{
			if (!debug_materials || !debug_materials->current.enabled)
			{
				set_pixel_texture_hook.invoke<void>(cmd_buf_state, a2, image);
				return;
			}

			if (image && image->name)
			{
				console::debug("set_pixel_texture_stub: \"%s\"\n", image->name);
			}
			else
			{
				console::error("set_pixel_texture_stub: texture has no name or is nullptr\n");
			}

			set_pixel_texture_hook.invoke<void>(cmd_buf_state, a2, image);
		}

		struct GfxBspSurfIter
		{
			const unsigned int* current;
			const unsigned int* end;
			const unsigned int* mark;
		};

		struct GfxTrianglesDrawStream
		{
			void* viewProjectionMatrix;
			void* projectionMatrix;
			float viewOrigin[3];
			int needSubdomain;
			GfxBspSurfIter* bspSurfIter;
			const game::GfxTexture* reflectionProbeTexture;
			const game::GfxTexture* lightmapPrimaryTexture;
			const game::GfxTexture* lightmapSecondaryTexture;
			unsigned int customSamplerFlags;
		};

		void r_draw_triangles_lit_stub(GfxTrianglesDrawStream* draw_stream, void* context)
		{
			__try
			{
				r_draw_triangles_lit_hook.invoke<void>(draw_stream, context);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				auto world = (*game::s_world);
				auto index = *(draw_stream->bspSurfIter->current - 1);
				auto surf = &world->dpvs.surfaces[index];
				auto material = surf->material;
				console::error("R_DrawTrianglesLit: %s\n", material->name);
			}
		}
#endif
	}

	bool setup_material_image(game::Material* material, const std::string& data)
	{
		if (*game::d3d11_device == nullptr)
		{
			console::error("Tried to create texture while d3d11 device isn't initialized\n");
			return false;
		}

		const auto image = material->textureTable->u.image;
		*(int*)&image->mapType = 0x1000003;
		*(int*)&image->picmip = -1;

		auto raw_image = utils::image{data};

		D3D11_SUBRESOURCE_DATA resource_data{};
		resource_data.SysMemPitch = raw_image.get_width() * 4;
		resource_data.SysMemSlicePitch = resource_data.SysMemPitch * raw_image.get_height();
		resource_data.pSysMem = raw_image.get_buffer();

		game::Image_Setup(image, raw_image.get_width(), raw_image.get_height(), image->depth, image->numElements,
			image->mapType, DXGI_FORMAT_R8G8B8A8_UNORM, image->name, &resource_data);
		return true;
	}

	game::Material* create_material(const std::string& name)
	{
		const auto white = game::Material_RegisterHandle("$white");
		const auto material = utils::memory::allocate<game::Material>();
		const auto texture_table = utils::memory::allocate<game::MaterialTextureDef>();
		const auto image = utils::memory::allocate<game::GfxImage>();

		std::memcpy(material, white, sizeof(game::Material));
		std::memcpy(texture_table, white->textureTable, sizeof(game::MaterialTextureDef));
		std::memcpy(image, white->textureTable->u.image, sizeof(game::GfxImage));

		material->constantTable = &constant_table;
		material->name = utils::memory::duplicate_string(name);
		image->name = material->name;

		image->texture.map = nullptr;
		image->texture.shaderView = nullptr;
		image->texture.shaderViewAlternate = nullptr;
		texture_table->u.image = image;

		material->textureTable = texture_table;

		return material;
	}

	void free_material(game::Material* material)
	{
		const auto try_release = []<typename T>(T** resource)
		{
			if (*resource != nullptr)
			{
				(*resource)->Release();
				*resource = nullptr;
			}
		};

		try_release(&material->textureTable->u.image->texture.map);
		try_release(&material->textureTable->u.image->texture.shaderView);
		try_release(&material->textureTable->u.image->texture.shaderViewAlternate);

		utils::memory::free(material->textureTable->u.image);
		utils::memory::free(material->textureTable);
		utils::memory::free(material->name);
		utils::memory::free(material);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			db_material_streaming_fail_hook.create(SELECT_VALUE(0x1FB400_b, 0x3A1600_b), db_material_streaming_fail_stub);
			db_get_material_index_hook.create(SELECT_VALUE(0x1F1D80_b, 0x396000_b), db_get_material_index_stub);

#ifdef DEBUG
			if (!game::environment::is_sp())
			{
				material_compare_hook.create(0x693B90_b, material_compare_stub);
				set_pixel_texture_hook.create(0x6B33E0_b, set_pixel_texture_stub);

				utils::hook::jump(0x6AD55C_b, utils::hook::assemble(print_current_material_stub), true);

				scheduler::once([]
				{
					debug_materials = dvars::register_bool("debug_materials", false, game::DVAR_FLAG_NONE, "Print current material and images");
				}, scheduler::main);

				r_draw_triangles_lit_hook.create(0x666870_b, r_draw_triangles_lit_stub);
			}
#endif
		}
	};
}

REGISTER_COMPONENT(materials::component)
