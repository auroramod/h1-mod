#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "images.hpp"
#include "console.hpp"
#include "filesystem.hpp"
#include "fastfiles.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>
#include <utils/concurrency.hpp>

#define CUSTOM_IMAGE_FILE_INDEX 96

namespace imagefiles
{
	namespace
	{
		struct image_file_unk
		{
			char __pad0[120];
		};

		std::unordered_map<std::string, image_file_unk*> image_file_unk_map;
		std::unordered_map<std::string, game::DB_IFileSysFile*> image_file_handles;

		std::string get_image_file_name()
		{
			return fastfiles::get_current_fastfile();
		}

		image_file_unk* get_image_file_unk(unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return &reinterpret_cast<image_file_unk*>(
					SELECT_VALUE(0x4802090_b, 0x6306770_b))[index];
			}

			const auto name = get_image_file_name();
			if (image_file_unk_map.find(name) == image_file_unk_map.end())
			{
				const auto unk = utils::memory::get_allocator()->allocate<image_file_unk>();
				image_file_unk_map[name] = unk;
				return unk;
			}

			return image_file_unk_map[name];
		}

		game::DB_IFileSysFile* get_image_file_handle(unsigned int index)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return reinterpret_cast<game::DB_IFileSysFile**>(
					SELECT_VALUE(0x4801D80_b, 0x6306180_b))[index];
			}

			const auto name = get_image_file_name();
			return image_file_handles[name];
		}

		void db_create_gfx_image_stream_stub(utils::hook::assembler& a)
		{
			const auto check_image_file_handle = a.newLabel();
			const auto handle_is_open = a.newLabel();

			a.movzx(eax, cx);
			a.push(rax);
			a.push(rax);
			a.pushad64();
			a.mov(rcx, rax);
			a.call_aligned(get_image_file_unk);
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
			a.jmp(SELECT_VALUE(0x1FAD49_b, 0x3A0CA5_b));

			a.bind(handle_is_open);
			a.jmp(SELECT_VALUE(0x1FAD99_b, 0x3A0CF5_b));
		}

		void* pakfile_open_stub(void* /*handles*/, unsigned int count, int is_imagefile, 
			unsigned int index, int is_localized)
		{
			if (index != CUSTOM_IMAGE_FILE_INDEX)
			{
				return utils::hook::invoke<void*>(
					SELECT_VALUE(0x42BC00_b, 0x5B2030_b),
					SELECT_VALUE(0x4801D80_b, 0x6306180_b), 
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

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::jump(SELECT_VALUE(0x1FAD35_b, 0x3A0C95_b),
				utils::hook::assemble(db_create_gfx_image_stream_stub), true);
			utils::hook::call(SELECT_VALUE(0x1FAD7B_b, 0x3A0CD7_b), pakfile_open_stub);
			utils::hook::call(SELECT_VALUE(0x1FAD5D_b, 0x3A0CB9_b), com_sprintf_stub);
		}
	};
}

REGISTER_COMPONENT(imagefiles::component)
