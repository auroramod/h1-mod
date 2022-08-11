#pragma once

#include <string>
#include <unordered_map>

#define CHUNK 16384u

namespace utils::compression
{
	namespace zlib
	{
		std::string compress(const std::string& data);
		std::string decompress(const std::string& data);

		auto gsc_compress(const std::vector<std::uint8_t>& data)->std::vector<std::uint8_t>;
	}

	namespace zip
	{
		class archive
		{
		public:
			void add(std::string filename, std::string data);
			bool write(const std::string& filename, const std::string& comment = {});

		private:
			std::unordered_map<std::string, std::string> files_;
		};
	}
};
