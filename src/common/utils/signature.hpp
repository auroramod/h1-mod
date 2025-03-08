#pragma once
#include "nt.hpp"
#include <cstdint>

namespace utils::hook
{
	class signature final
	{
	public:
		using signature_result = std::vector<uint8_t*>;

		explicit signature(const std::string& pattern, const nt::library& library = {})
			: signature(pattern, library.get_ptr(), library.get_optional_header()->SizeOfImage)
		{
		}

		signature(const std::string& pattern, void* start, void* end)
			: signature(pattern, start, size_t(end) - size_t(start))
		{
		}

		signature(const std::string& pattern, void* start, const size_t length)
			: start_(static_cast<uint8_t*>(start)), length_(length)
		{
			this->load_pattern(pattern);
		}

		signature_result process() const;

	private:
		std::string mask_;
		std::basic_string<uint8_t> pattern_;

		uint8_t* start_;
		size_t length_;

		void load_pattern(const std::string& pattern);

		signature_result process_parallel() const;
		signature_result process_serial() const;
		signature_result process_range(uint8_t* start, size_t length) const;
		signature_result process_range_linear(uint8_t* start, size_t length) const;
		signature_result process_range_vectorized(uint8_t* start, size_t length) const;

		bool has_sse_support() const;
	};
}

utils::hook::signature::signature_result operator"" _sig(const char* str, size_t len);
