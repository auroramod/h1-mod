#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "memory.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace memory
{
	namespace
	{
		constexpr auto mem_low_size = 0x80000000ui64 * 2; // default: 0x80000000
		constexpr auto mem_high_size = 0x80000000ui64 * 2; // default: 0x80000000

		constexpr auto script_mem_low_size = 0x100000ui64; // default: 0x100000
		constexpr auto script_mem_high_size = 0x100000ui64 + custom_script_mem_size; // default: 0x100000

		constexpr auto phys_mem_low_size = 0x700000000ui64; // default: 0x700000000
		constexpr auto phys_mem_high_size = 0x300000000i64; // default: 0x300000000

		constexpr auto pmem_alloc_size =
			mem_low_size +
			mem_high_size +
			script_mem_low_size +
			script_mem_high_size +
			phys_mem_low_size +
			phys_mem_high_size;

		constexpr auto mem_low_buf = 0;
		constexpr auto mem_high_buf = mem_low_buf + mem_low_size;

		constexpr auto script_mem_low_buf = mem_high_buf + mem_high_size;
		constexpr auto script_mem_high_buf = script_mem_low_buf + script_mem_low_size;

		constexpr auto phys_mem_low_buf = script_mem_high_buf + script_mem_high_size;
		constexpr auto phys_mem_high_buf = phys_mem_low_buf + phys_mem_low_size;

		constexpr auto stream_mem_size = 0x2000000ui64;

		void pmem_init()
		{
			const auto size = pmem_alloc_size;
			const auto allocated_buffer = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
			auto buffer = reinterpret_cast<unsigned char*>(allocated_buffer);
			*game::pmem_size = size;
			*game::pmem_buffer = buffer;

			memset(game::g_mem, 0, sizeof(*game::g_mem));

			game::g_mem->prim[game::PHYS_ALLOC_LOW].buf = buffer + mem_low_buf;
			game::g_mem->prim[game::PHYS_ALLOC_LOW].pos = mem_low_size;
			game::g_mem->prim[game::PHYS_ALLOC_HIGH].buf = buffer + mem_high_buf;
			game::g_mem->prim[game::PHYS_ALLOC_HIGH].pos = mem_high_size;

			game::g_mem->prim[game::PHYS_ALLOC_LOW].unk1 = 0;
			game::g_mem->prim[game::PHYS_ALLOC_HIGH].unk1 = 0;

			memset(game::g_scriptmem, 0, sizeof(*game::g_scriptmem));

			game::g_scriptmem->prim[game::PHYS_ALLOC_LOW].buf = buffer + script_mem_low_buf;
			game::g_scriptmem->prim[game::PHYS_ALLOC_LOW].pos = script_mem_low_size;
			game::g_scriptmem->prim[game::PHYS_ALLOC_HIGH].buf = buffer + script_mem_high_buf;
			game::g_scriptmem->prim[game::PHYS_ALLOC_HIGH].pos = script_mem_high_size;

			game::g_scriptmem->prim[game::PHYS_ALLOC_LOW].unk1 = 0;
			game::g_scriptmem->prim[game::PHYS_ALLOC_HIGH].unk1 = 0;

			memset(game::g_physmem, 0, sizeof(*game::g_physmem));

			game::g_physmem->prim[game::PHYS_ALLOC_LOW].buf = buffer + phys_mem_low_buf;
			game::g_physmem->prim[game::PHYS_ALLOC_LOW].pos = phys_mem_low_size;
			game::g_physmem->prim[game::PHYS_ALLOC_HIGH].buf = buffer + phys_mem_high_buf;
			game::g_physmem->prim[game::PHYS_ALLOC_HIGH].pos = phys_mem_high_size;

			game::g_physmem->prim[game::PHYS_ALLOC_LOW].unk1 = 2;
			game::g_physmem->prim[game::PHYS_ALLOC_HIGH].unk1 = 2;

			*game::stream_size = stream_mem_size;
			*game::stream_buffer = reinterpret_cast<unsigned char*>(VirtualAlloc(NULL, *game::stream_size, MEM_COMMIT, PAGE_READWRITE));
		}

		void pmem_init_stub()
		{
			// call our own init
			pmem_init();

			const auto script_mem_size = script_mem_low_size + script_mem_high_size;
			utils::hook::set<uint32_t>(SELECT_VALUE(0x420252_b, 0x5A5582_b), static_cast<uint32_t>(script_mem_size));
		}
	}

	namespace
	{
		int out_of_memory_text_stub(char* dest, int size, const char* fmt, ...)
		{
			fmt = "%s (%d)\n\n"
				"Disable shader caching, lower graphic settings, free up RAM, or update your GPU drivers.\n\n";

			char buffer[2048];

			{
				va_list ap;
				va_start(ap, fmt);

				vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, ap);

				va_end(ap);
			}

			return utils::hook::invoke<int>(SELECT_VALUE(0x429200_b, 0x5AF0F0_b), dest, size, "%s", buffer);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// patch PMem_Init, so we can use whatever memory size we want
			utils::hook::call(SELECT_VALUE(0x38639C_b, 0x15C4D6_b), pmem_init_stub);

			// Com_sprintf for "Out of memory. You are probably low on disk space."
			utils::hook::call(SELECT_VALUE(0x457BC9_b, 0x1D8E09_b), out_of_memory_text_stub);
		}
	};
}

REGISTER_COMPONENT(memory::component)