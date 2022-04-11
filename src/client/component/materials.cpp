#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "materials.hpp"
#include "console.hpp"
#include "filesystem.hpp"

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
			db_material_streaming_fail_hook.create(SELECT_VALUE(0x1401D3180, 0x1402C6260), db_material_streaming_fail_stub);
			db_get_material_index_hook.create(SELECT_VALUE(0x1401CAD00, 0x1402BBB20), db_get_material_index_stub);
		}
	};
}

REGISTER_COMPONENT(materials::component)
