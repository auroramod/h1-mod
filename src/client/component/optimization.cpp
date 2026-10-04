#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "optimization.hpp"

#include "console.hpp"

#include "game/dvars.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>

#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

static utils::hook::detour db_read_x_file_hook;
static utils::hook::detour pmem_commit_memory_hook;
static utils::hook::detour pmem_decommit_memory_hook;

static game::dvar_t* db_verify_zone_hashes = nullptr;
static game::dvar_t* db_coalesce_sound_reads = nullptr;
static game::dvar_t* db_prefault_zone_memory = nullptr;
static game::dvar_t* db_throttle_streaming_on_load = nullptr;

namespace
{
	using load_clock = std::chrono::steady_clock;
}

char optimization::current_zone[256]{};
load_clock::time_point optimization::zone_load_start{};
std::atomic<DWORD> optimization::loader_thread{0};
std::atomic<int> optimization::zone_loads_in_flight{0};
optimization::zone_prefault_t optimization::zone_prefault;

void optimization::post_unpack()
{
	if (game::environment::is_sp())
	{
		return;
	}

	db_verify_zone_hashes = dvars::register_bool("db_verifyZoneHashes",
		true, game::DVAR_ARCHIVE, "Verify the SHA-256 of every 16 KB chunk of signed fastfiles while loading");

	db_coalesce_sound_reads = dvars::register_bool("db_coalesceSoundReads",
		true, game::DVAR_ARCHIVE, "Read packed loaded sounds in large batched reads instead of one blocking read per sound");

	db_prefault_zone_memory = dvars::register_bool("db_prefaultZoneMemory",
		true, game::DVAR_ARCHIVE, "Touch newly committed zone memory on a second thread so the loader takes fewer page faults");

	db_throttle_streaming_on_load = dvars::register_bool("db_throttleStreamingOnLoad",
		true, game::DVAR_ARCHIVE, "Pause non-urgent image streaming while a fastfile is loading (faster loads, blurry textures meanwhile)");

	// zone body reads
	utils::hook::call(0x1402C03BD, db_load_x_file_stub); // DB_LoadXFile
	db_read_x_file_hook.create(game::mp::DB_ReadXFile, db_read_x_file_stub);

	// signed zone chunk verification
	utils::hook::jump(0x14028C70C, verify_zone_chunk_stub(0x14028C746), true);
	utils::hook::jump(0x14028C7D1, verify_zone_chunk_stub(0x14028C811), true);

	// zone memory prefaulting
	pmem_commit_memory_hook.create(game::mp::PMem_CommitMemory, pmem_commit_memory_stub);
	pmem_decommit_memory_hook.create(game::mp::PMem_DecommitMemory, pmem_decommit_memory_stub);

	// packed loaded sounds
	utils::hook::call(0x1402C0438, db_read_packed_loaded_sounds_stub); // DB_ReadPackedLoadedSounds
}

void optimization::begin_zone_load(const char* zone_name)
{
	strncpy_s(current_zone, zone_name, _TRUNCATE);

	loader_thread = GetCurrentThreadId();
	zone_load_start = load_clock::now();
	++zone_loads_in_flight;
}

void optimization::end_zone_load()
{
	--zone_loads_in_flight;
	loader_thread = 0;

	const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(load_clock::now() - zone_load_start).count();
	console::info("Loaded fastfile %s (took %lli ms)\n", current_zone, ms);
}

bool optimization::throttle_streaming()
{
	return zone_loads_in_flight > 0 && db_throttle_streaming_on_load && db_throttle_streaming_on_load->current.enabled;
}

std::uint64_t optimization::read_sound_window(game::StreamFile& file, const std::uint64_t offset, const std::uint64_t size, char* buffer)
{
	const auto aligned_offset = offset & ~sector_mask;
	const auto skip = offset - aligned_offset;
	const auto aligned_size = (skip + size + sector_mask) & ~sector_mask;

	const auto db_fs = *game::db_fs;
	db_fs->vftbl->Read(db_fs, file.handle, aligned_offset, aligned_size, buffer);
	std::uint64_t bytes_read = 0;
	db_fs->vftbl->Tell(db_fs, file.handle, &bytes_read);

	return bytes_read > skip ? std::min(bytes_read - skip, size) : 0;
}

/*
	rewrite of DB_ReadPackedLoadedSounds that batches neighbouring sounds of the same soundfile into one large read instead of one 
	blocking read per sound. this function is HUGE, and has been tested with positive gains and no negatives as far as I can tell... -mikey
*/
void optimization::read_packed_loaded_sounds(game::PackedLoadedSound* const* sounds, const int count, char* buffer)
{
	game::StreamFile file{};
	auto current_key = -1;

	auto i = 0;
	while (i < count)
	{
		const auto first = sounds[i];
		const auto key = first->isLocalized | (first->fileIndex << 16);
		if (key != current_key)
		{
			if (file.handle)
			{
				game::mp::StreamFileClose(&file);
			}

			std::int16_t file_name[0x24]{};
			file_name[0] = first->isLocalized;
			file_name[1] = static_cast<std::int16_t>(first->fileIndex);

			file = {};
			game::mp::StreamFileOpen(&file, file_name);
			current_key = key;
		}

		if (!file.handle)
		{
			++i;
			continue;
		}

		if (first->length > sound_read_window - 0x1000)
		{
			std::uint64_t done = 0;
			while (done < first->length)
			{
				const auto piece = std::min(first->length - done, sound_read_window - 0x1000);
				const auto got = read_sound_window(file, file.startOffset + first->offset + done, piece, buffer);
				if (!got)
				{
					break;
				}

				std::memcpy(first->dest + done, buffer + ((file.startOffset + first->offset + done) & sector_mask), got);
				done += got;
			}

			first->fileIndex = 0;
			++i;
			continue;
		}

		const auto window_start = first->offset;
		auto window_end = first->offset + first->length;
		auto last = i + 1;
		while (last < count)
		{
			const auto next = sounds[last];
			if ((next->isLocalized | (next->fileIndex << 16)) != key || next->offset < window_start ||
				next->offset > window_end + sound_read_max_gap ||
				std::max(window_end, next->offset + next->length) - window_start > sound_read_window - 0x1000)
			{
				break;
			}

			window_end = std::max(window_end, next->offset + next->length);
			++last;
		}

		const auto absolute_start = file.startOffset + window_start;
		const auto got = read_sound_window(file, absolute_start, window_end - window_start, buffer);
		const auto data = buffer + (absolute_start & sector_mask);

		for (auto j = i; j < last; ++j)
		{
			const auto sound = sounds[j];
			const auto rel = sound->offset - window_start;
			if (rel < got && sound->length)
			{
				std::memcpy(sound->dest, data + rel, std::min(sound->length, got - rel));
			}

			sound->fileIndex = 0;
		}

		i = last;
	}

	if (file.handle)
	{
		game::mp::StreamFileClose(&file);
	}
}

void optimization::db_read_packed_loaded_sounds_stub(const int unused)
{
	if (db_coalesce_sound_reads && !db_coalesce_sound_reads->current.enabled)
	{
		game::mp::DB_ReadPackedLoadedSounds(unused);
		return;
	}

	const auto count = *game::mp::db_packedLoadedSoundCount;
	if (count <= 0)
	{
		return;
	}

	const auto buffer = static_cast<char*>(VirtualAlloc(nullptr, sound_read_window + 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	if (!buffer)
	{
		game::mp::DB_ReadPackedLoadedSounds(unused);
		return;
	}

	read_packed_loaded_sounds(game::mp::db_packedLoadedSounds.get(), count, buffer);
	VirtualFree(buffer, 0, MEM_RELEASE);
}

/*
	zone prefaulting (db_prefaultZoneMemory)
*/
DWORD WINAPI optimization::zone_prefault_worker(LPVOID)
{
	auto& p = zone_prefault;
	while (true)
	{
		std::pair<volatile char*, std::size_t> range{};
		{
			std::unique_lock lock(p.mutex);
			p.busy = false;
			p.cv.notify_all();
			p.cv.wait(lock, [&] { return !p.ranges.empty(); });
			range = p.ranges.front();
			p.ranges.pop_front();
			p.busy = true;
		}

		for (std::size_t offset = 0; offset < range.second && !p.cancel.load(std::memory_order_relaxed); offset += 0x1000)
		{
			_InterlockedOr8(const_cast<char*>(range.first + offset), 0);
		}
	}
}

void optimization::zone_prefault_add(char* base, const std::size_t size)
{
	auto& p = zone_prefault;
	std::lock_guard _(p.mutex);
	if (!p.started)
	{
		const auto thread = CreateThread(nullptr, 0, zone_prefault_worker, nullptr, 0, nullptr);
		if (!thread)
		{
			return;
		}

		CloseHandle(thread);
		p.started = true;
	}

	p.ranges.emplace_back(base, size);
	p.cv.notify_all();
}

void optimization::zone_prefault_stop()
{
	auto& p = zone_prefault;
	p.loader_id = 0;

	std::unique_lock lock(p.mutex);
	p.ranges.clear();
	p.cancel = true;
	p.cv.wait(lock, [&] { return !p.busy; });
	p.cancel = false;
}

std::uint64_t optimization::pmem_commit_memory_stub(game::PMemRange* range)
{
	const auto result = pmem_commit_memory_hook.invoke<std::uint64_t>(range);
	if (result)
	{
		console::error("Failed to commit 0x%llX bytes of zone memory at %p (error %lu)\n",
			static_cast<std::uint64_t>(range->blocks) << 16, range->base, GetLastError());
	}

	if (!result && zone_prefault.loader_id == GetCurrentThreadId())
	{
		zone_prefault_add(range->base, static_cast<std::size_t>(range->blocks) << 16);
	}

	return result;
}

std::uint64_t optimization::pmem_decommit_memory_stub(const std::uint64_t a1, const std::uint64_t a2, const std::uint64_t a3, const std::uint64_t a4)
{
	zone_prefault_stop();
	return pmem_decommit_memory_hook.invoke<std::uint64_t>(a1, a2, a3, a4);
}

void optimization::db_load_x_file_stub(const char* name, void* zone, void* memory, const bool flag)
{
	const auto prefault = db_prefault_zone_memory && db_prefault_zone_memory->current.enabled;
	zone_prefault.armed = prefault;
	zone_prefault.body_left = 0;
	zone_prefault.loader_id = prefault ? GetCurrentThreadId() : 0;

	game::mp::DB_LoadXFile(name, zone, memory, flag);

	zone_prefault.armed = false;
	zone_prefault_stop();
}

unsigned int optimization::db_read_x_file_stub(unsigned char* pos, const std::uint64_t size, const int image_copy)
{
	const auto result = db_read_x_file_hook.invoke<unsigned int>(pos, size, image_copy);
	if (loader_thread != GetCurrentThreadId())
	{
		return result;
	}

	auto& p = zone_prefault;
	if (p.body_left)
	{
		p.body_left -= std::min(size, p.body_left);
		if (!p.body_left)
		{
			zone_prefault_stop();
		}
	}
	else if (p.armed)
	{
		p.armed = false;
		if (size == xfile_header_size && !image_copy)
		{
			p.body_left = *reinterpret_cast<std::uint64_t*>(pos);
		}
	}

	return result;
}

/*
	signed zones hash every 16 KB chunk of the compressed stream with LibTomCrypt's portable sha256 (~250 MB/s). CNG uses the CPU's SHA 
	instructions when available instead
*/
int optimization::verify_zone_chunk(const std::uint8_t* expected, const std::uint8_t* data)
{
	constexpr auto chunk_size = 0x4000;

	if (db_verify_zone_hashes && !db_verify_zone_hashes->current.enabled)
	{
		return 0;
	}

	static const auto algorithm = []() -> BCRYPT_ALG_HANDLE
	{
		BCRYPT_ALG_HANDLE handle{};
		return BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&handle, BCRYPT_SHA256_ALGORITHM, nullptr, 0)) ? handle : nullptr;
	}();

	std::uint8_t digest[32]{};
	if (!algorithm || !BCRYPT_SUCCESS(BCryptHash(algorithm, nullptr, 0, const_cast<PUCHAR>(data), chunk_size, digest, sizeof(digest))))
	{
		alignas(16) std::uint8_t state[0x100]{};
		game::mp::sha256_init(state);
		game::mp::sha256_process(state, data, chunk_size);
		game::mp::sha256_done(state, digest);
	}

	return std::memcmp(expected, digest, sizeof(digest));
}

void* optimization::verify_zone_chunk_stub(const std::size_t resume)
{
	return utils::hook::assemble([resume](utils::hook::assembler& a)
	{
		a.mov(rcx, rbx); // rbx holds the expected hash
		a.mov(rdx, r12); // r12 has the chunk
		a.call_aligned(verify_zone_chunk);
		a.jmp(resume);
	});
}

REGISTER_COMPONENT(optimization)
