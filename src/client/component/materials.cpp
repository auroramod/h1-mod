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
		utils::hook::detour material_register_handle_hook;
		utils::hook::detour db_get_material_index_hook;

#ifdef DEBUG
		utils::hook::detour material_compare_hook;
		utils::hook::detour set_pixel_texture_hook;

		const game::dvar_t* debug_materials = nullptr;
#endif

		struct material_data_t
		{
			std::unordered_map<std::string, game::Material*> materials;
			std::unordered_map<std::string, std::string> images;
		};

		char constant_table[0x20] = {};

		utils::concurrency::container<material_data_t> material_data;

		game::GfxImage* setup_image(game::GfxImage* image, const utils::image& raw_image)
		{
			image->imageFormat = 0x1000003;
			image->resourceSize = -1;

			D3D11_SUBRESOURCE_DATA data{};
			data.SysMemPitch = raw_image.get_width() * 4;
			data.SysMemSlicePitch = data.SysMemPitch * raw_image.get_height();
			data.pSysMem = raw_image.get_buffer();

			game::Image_Setup(image, raw_image.get_width(), raw_image.get_height(), image->depth, image->numElements,
				image->imageFormat, DXGI_FORMAT_R8G8B8A8_UNORM, image->name, &data);

			return image;
		}

		game::Material* create_material(const std::string& name, const std::string& data)
		{
			const auto white = material_register_handle_hook.invoke<game::Material*>("white");
			const auto material = utils::memory::get_allocator()->allocate<game::Material>();
			const auto texture_table = utils::memory::get_allocator()->allocate<game::MaterialTextureDef>();
			const auto image = utils::memory::get_allocator()->allocate<game::GfxImage>();

			std::memcpy(material, white, sizeof(game::Material));
			std::memcpy(texture_table, white->textureTable, sizeof(game::MaterialTextureDef));
			std::memcpy(image, white->textureTable->u.image, sizeof(game::GfxImage));

			material->constantTable = &constant_table;
			material->name = utils::memory::get_allocator()->duplicate_string(name);
			image->name = material->name;

			material->textureTable = texture_table;
			material->textureTable->u.image = setup_image(image, data);

			return material;
		}

		void free_material(game::Material* material)
		{
			material->textureTable->u.image->textures.___u0.map->Release();
			material->textureTable->u.image->textures.shaderView->Release();
			utils::memory::get_allocator()->free(material->textureTable->u.image);
			utils::memory::get_allocator()->free(material->textureTable);
			utils::memory::get_allocator()->free(material->name);
			utils::memory::get_allocator()->free(material);
		}

		game::Material* load_material(const std::string& name)
		{
			return material_data.access<game::Material*>([&](material_data_t& data_) -> game::Material*
			{
				if (const auto i = data_.materials.find(name); i != data_.materials.end())
				{
					return i->second;
				}

				std::string data{};
				if (const auto i = data_.images.find(name); i != data_.images.end())
				{
					data = i->second;
				}

				if (data.empty() && !filesystem::read_file(utils::string::va("materials/%s.png", name.data()), &data))
				{
					data_.materials[name] = nullptr;
					return nullptr;
				}

				const auto material = create_material(name, data);
				data_.materials[name] = material;

				return material;
			});
		}

		game::Material* try_load_material(const std::string& name)
		{
			if (name == "white")
			{
				return nullptr;
			}

			try
			{
				return load_material(name);
			}
			catch (const std::exception& e)
			{
				console::error("Failed to load material %s: %s\n", name.data(), e.what());
			}

			return nullptr;
		}

		game::Material* material_register_handle_stub(const char* name)
		{
			auto result = try_load_material(name);
			if (result == nullptr)
			{
				result = material_register_handle_hook.invoke<game::Material*>(name);
			}
			return result;
		}

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
				console::error("Material_Compare: %s - %s (%d - %d)", 
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
#endif
	}

	void add(const std::string& name, const std::string& data)
	{
		material_data.access([&](material_data_t& data_)
		{
			data_.images[name] = data;
		});
	}

	bool exists(const std::string& name)
	{
		return material_data.access<bool>([&](material_data_t& data_)
		{
			return data_.images.find(name) != data_.images.end();
		});
	}

	void clear()
	{
		material_data.access([&](material_data_t& data_)
		{
			for (auto& material : data_.materials)
			{
				if (material.second == nullptr)
				{
					continue;
				}

				free_material(material.second);
			}

			data_.materials.clear();
		});
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

			material_register_handle_hook.create(game::Material_RegisterHandle, material_register_handle_stub);
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
					debug_materials = dvars::register_bool("debug_materials", 0, 0x0, "Print current material and images");
				}, scheduler::main);
			}
#endif
		}
	};
}

REGISTER_COMPONENT(materials::component)
