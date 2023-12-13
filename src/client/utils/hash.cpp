#include <std_include.hpp>

#include "game/game.hpp"

#include "hash.hpp"
#include "component/console.hpp"

#include <zlib.h>
#include <utils/cryptography.hpp>
#include <utils/string.hpp>

namespace utils::hash
{
	namespace
	{
		constexpr auto read_buffer_size = 1ull * 1024ull * 1024ull; // 1MB

		std::string get_file_hash_pakfile(std::ifstream& file_stream, const std::size_t file_size,
			const std::string& filename)
		{
			if (file_size < sizeof(game::XPakHeader))
			{
				return {};
			}

			game::XPakHeader header{};
			file_stream.read(reinterpret_cast<char*>(&header), sizeof(game::XPakHeader));

			constexpr auto hash_size = sizeof(game::DB_AuthHash);

			game::DB_AuthHash empty_hash{};
			if (!std::memcmp(header.hash.bytes, empty_hash.bytes, hash_size))
			{
				console::warn("Computing pakfile hash because its missing, this may take some time...\n");

				hash_state state;
				sha256_init(&state);

				std::string buffer;
				buffer.resize(read_buffer_size);

				auto bytes_to_read = file_size - sizeof(game::XPakHeader);
				while (bytes_to_read > 0)
				{
					const auto read_size = std::min(read_buffer_size, bytes_to_read);
					file_stream.read(buffer.data(), read_size);
					sha256_process(&state, reinterpret_cast<std::uint8_t*>(buffer.data()), static_cast<std::uint32_t>(read_size));
					bytes_to_read -= read_size;
				}

				file_stream.close();

				if (sha256_done(&state, header.hash.bytes) != CRYPT_OK)
				{
					return {};
				}

				std::fstream out_stream;
				out_stream.open(filename, std::ios_base::binary | std::ios_base::out | std::ios_base::in);
				out_stream.write(reinterpret_cast<char*>(&header), sizeof(game::XPakHeader));
			}

			const auto hash_str = std::string{header.hash.bytes, header.hash.bytes + hash_size};

			return utils::string::dump_hex(hash_str, "");
		}

		std::string get_file_hash_generic(std::ifstream& file_stream, const std::size_t file_size)
		{
			auto crc_value = crc32(0L, Z_NULL, 0);
			auto bytes_to_read = file_size;

			std::string buffer;
			buffer.resize(read_buffer_size);

			while (bytes_to_read > 0)
			{
				const auto read_size = std::min(bytes_to_read, read_buffer_size);
				file_stream.read(buffer.data(), read_size);
				crc_value = crc32(crc_value, reinterpret_cast<std::uint8_t*>(buffer.data()), static_cast<std::uint32_t>(read_size));
				bytes_to_read -= read_size;
			}

			std::string hash;
			hash.append(reinterpret_cast<char*>(&crc_value), sizeof(crc_value));
			return utils::string::dump_hex(hash, "");
		}

		std::string get_pakfile_buffer_hash(std::string& buffer)
		{
			if (buffer.size() < sizeof(game::XPakHeader))
			{
				return {};
			}

			constexpr auto hash_size = sizeof(game::DB_AuthHash);
			const auto header = reinterpret_cast<game::XPakHeader*>(buffer.data());

			game::DB_AuthHash empty_hash{};
			if (!std::memcmp(header->hash.bytes, empty_hash.bytes, hash_size))
			{
				console::warn("Computing pakfile hash because its missing, this may take some time...\n");
				const auto hash_start = reinterpret_cast<std::uint8_t*>(buffer.data() + sizeof(game::XPakHeader));
				const auto len = buffer.size() - sizeof(game::XPakHeader);
				const auto hash = utils::cryptography::sha256::compute(hash_start, len, false);
				std::memcpy(header->hash.bytes, hash.data(), sizeof(header->hash));
			}

			std::string hash = {header->hash.bytes, header->hash.bytes + hash_size};
			return utils::string::dump_hex(hash, "");
		}

		std::string get_generic_buffer_hash(const std::string& buffer)
		{
			auto crc_value = crc32(0L, Z_NULL, 0);
			crc_value = crc32(crc_value, reinterpret_cast<const std::uint8_t*>(buffer.data()), 
				static_cast<std::uint32_t>(buffer.size()));
			std::string hash;
			hash.append(reinterpret_cast<char*>(&crc_value), sizeof(crc_value));
			return utils::string::dump_hex(hash, "");
		}
	}

	std::string get_file_hash(const std::string& file)
	{
		std::ifstream file_stream(file, std::ios::binary);
		if (!file_stream.is_open())
		{
			return {};
		}

		file_stream.seekg(0, std::ios::end);
		const auto file_size = static_cast<std::size_t>(file_stream.tellg());
		file_stream.seekg(0, std::ios::beg);

		if (file.ends_with(".pak"))
		{
			return get_file_hash_pakfile(file_stream, file_size, file);
		}
		else
		{
			return get_file_hash_generic(file_stream, file_size);
		}
	}

	std::string get_buffer_hash(std::string& buffer, const std::string& filename)
	{
		if (filename.ends_with(".pak"))
		{
			return get_pakfile_buffer_hash(buffer);
		}
		else
		{
			return get_generic_buffer_hash(buffer);
		}
	}
}
