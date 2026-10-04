#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "filesystem.hpp"
#include "fastfiles.hpp"
#include "scheduler.hpp"
#include "imagefiles.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>
#include <utils/concurrency.hpp>
#include <utils/memory.hpp>

#define CUSTOM_IMAGE_FILE_INDEX 96

namespace imagefiles
{
	namespace
	{
			struct image_file_unk_mp
			{
				char __pad0[120];
			};

			struct image_file_unk_sp
			{
				char __pad0[96];
			};

		utils::memory::allocator image_file_allocator;
		std::unordered_map<std::string, game::DB_IFileSysFile*> image_file_handles;
		std::unordered_map<std::string, image_file_unk_mp*> image_file_unk_map_mp;
		std::unordered_map<std::string, image_file_unk_sp*> image_file_unk_map_sp;

		std::string get_image_file_name()
		{
			return fastfiles::get_current_fastfile();
		}

		void* get_image_file_unk_mp(unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return &reinterpret_cast<image_file_unk_mp*>(0x145332C90)[index];
			}

			const auto name = get_image_file_name();
			if (image_file_unk_map_mp.find(name) == image_file_unk_map_mp.end())
			{
				const auto unk = image_file_allocator.allocate<image_file_unk_mp>();
				image_file_unk_map_mp[name] = unk;
				return unk;
			}

			return image_file_unk_map_mp[name];
		}

		void* get_image_file_unk_sp(unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return &reinterpret_cast<image_file_unk_sp*>(0x144802090)[index];
			}

			const auto name = get_image_file_name();
			if (image_file_unk_map_sp.find(name) == image_file_unk_map_sp.end())
			{
				const auto unk = image_file_allocator.allocate<image_file_unk_sp>();
				image_file_unk_map_sp[name] = unk;
				return unk;
			}

			return image_file_unk_map_sp[name];
		}

		game::DB_IFileSysFile* get_image_file_handle(unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return reinterpret_cast<game::DB_IFileSysFile**>(
					SELECT_VALUE(0x144801D80, 0x145123B20))[index];
			}

			const auto name = get_image_file_name();
			return image_file_handles[name];
		}

		void db_create_gfx_image_stream_stub(utils::hook::assembler& a)
		{
			const auto handle_is_open = a.newLabel();

			a.movzx(eax, cx);
			a.push(rax);
			a.push(rax);
			a.pushad64();
			a.mov(rcx, rax);
			a.call_aligned(SELECT_VALUE(get_image_file_unk_sp, get_image_file_unk_mp));
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);
			a.mov(rsi, rax);
			a.pop(rax);

			a.push(rax);
			a.push(rax);
			a.pushad64();
			a.mov(rcx, rax);
			a.call_aligned(get_image_file_handle);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);
			a.mov(r12, rax);
			a.pop(rax);

			a.cmp(r12, r13);
			a.jnz(handle_is_open);
			a.jmp(SELECT_VALUE(0x1401FAD49, 0x1402C5A15));

			a.bind(handle_is_open);
			a.jmp(SELECT_VALUE(0x1401FAD99, 0x1402C5A65));
		}

		void* pakfile_open_stub(void* /*handles*/, unsigned int count, int is_imagefile,
			unsigned int index, short is_localized)
		{
			console::debug("Opening %s%d.pak (localized:%d)\n", is_imagefile ? "imagefile" : "soundfile", index, is_localized);

			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return utils::hook::invoke<void*>(
					SELECT_VALUE(0x14042BC00, 0x140506A00),
					SELECT_VALUE(0x144801D80, 0x145123B20),
					count, is_imagefile, index, is_localized
				);
			}

			const auto name = get_image_file_name();
			const auto db_fs = *game::db_fs;
			const auto handle = db_fs->vftbl->OpenFile(db_fs,
				game::SF_PAKFILE, utils::string::va("%s.pak", name.data()));
			if (handle != nullptr)
			{
				image_file_handles[name] = handle;
			}
			return handle;
		}

		int com_sprintf_stub(char* buffer, const int len, const char* fmt, unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return game::Com_sprintf(buffer, len, fmt, index);
			}

			const auto name = get_image_file_name();
			return game::Com_sprintf(buffer, len, "%s.pak", name.data());
		}
	}

	void close_custom_handles()
	{
		const auto db_fs = *game::db_fs;
		for (const auto& handle : image_file_handles)
		{
			if (handle.second != nullptr)
			{
				db_fs->vftbl->Close(db_fs, handle.second);
			}
		}

		image_file_handles.clear();
		image_file_unk_map_sp.clear();
		image_file_unk_map_mp.clear();
		image_file_allocator.clear();
	}

	void close_handle(const std::string& fastfile)
	{
		if (!image_file_handles.contains(fastfile))
		{
			return;
		}

		const auto db_fs = *game::db_fs;
		const auto handle = image_file_handles[fastfile];
		if (handle != nullptr)
		{
			db_fs->vftbl->Close(db_fs, handle);
		}

		image_file_handles.erase(fastfile);
		if (game::environment::is_sp())
		{
			image_file_allocator.free(image_file_unk_map_sp[fastfile]);
			image_file_unk_map_sp.erase(fastfile);
		}
		else
		{
			image_file_allocator.free(image_file_unk_map_mp[fastfile]);
			image_file_unk_map_mp.erase(fastfile);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::jump(SELECT_VALUE(0x1401FAD35, 0x1402C5A05),
				utils::hook::assemble(db_create_gfx_image_stream_stub), true);
			utils::hook::call(SELECT_VALUE(0x1401FAD7B, 0x1402C5A47), pakfile_open_stub);
			utils::hook::call(SELECT_VALUE(0x1401FAD5D, 0x1402C5A29), com_sprintf_stub);
		}
	};
}

REGISTER_COMPONENT(imagefiles::component)
