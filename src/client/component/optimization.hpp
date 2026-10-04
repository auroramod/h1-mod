#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

#include <condition_variable>
#include <deque>

class optimization final : public component_interface
{
public:
	void post_unpack() override;

	static void begin_zone_load(const char* zone_name);
	static void end_zone_load();
	static bool throttle_streaming();

private:
	struct zone_prefault_t
	{
		std::mutex mutex;
		std::condition_variable cv;
		std::deque<std::pair<volatile char*, std::size_t>> ranges;
		bool started = false;
		bool busy = false;
		bool armed = false;
		std::uint64_t body_left = 0;
		std::atomic_bool cancel{false};
		std::atomic<DWORD> loader_id{0};
	};

	static constexpr std::uint64_t sound_read_window = 16 * 1024 * 1024;
	static constexpr std::uint64_t sound_read_max_gap = 1024 * 1024;
	static constexpr std::uint64_t sector_mask = 0xFFF;
	static constexpr std::uint64_t xfile_header_size = 0x48;

	static char current_zone[256];
	static std::chrono::steady_clock::time_point zone_load_start;
	static std::atomic<DWORD> loader_thread;
	static std::atomic<int> zone_loads_in_flight;
	static zone_prefault_t zone_prefault;

	static std::uint64_t read_sound_window(game::StreamFile& file, std::uint64_t offset, std::uint64_t size, char* buffer);
	static void read_packed_loaded_sounds(game::PackedLoadedSound* const* sounds, int count, char* buffer);
	static void db_read_packed_loaded_sounds_stub(int unused);

	static DWORD WINAPI zone_prefault_worker(LPVOID);
	static void zone_prefault_add(char* base, std::size_t size);
	static void zone_prefault_stop();
	static std::uint64_t pmem_commit_memory_stub(game::PMemRange* range);
	static std::uint64_t pmem_decommit_memory_stub(std::uint64_t a1, std::uint64_t a2, std::uint64_t a3, std::uint64_t a4);

	static void db_load_x_file_stub(const char* name, void* zone, void* memory, bool flag);
	static unsigned int db_read_x_file_stub(unsigned char* pos, std::uint64_t size, int image_copy);

	static int verify_zone_chunk(const std::uint8_t* expected, const std::uint8_t* data);
	static void* verify_zone_chunk_stub(std::size_t resume);
};
