#pragma once

#include <string>

namespace utils
{
	class image
	{
	public:
		image(const std::string& data);

		int get_width() const;
		int get_height() const;
		const void* get_buffer() const;
		size_t get_size() const;

		const std::string& get_data() const;

	private:
		int width{};
		int height{};
		std::string data{};
	};
}
