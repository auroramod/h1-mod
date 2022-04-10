#pragma once

namespace filesystem
{
	class file
	{
	public:
		file(std::string name);

		bool exists() const;
		const std::string& get_buffer() const;
		const std::string& get_name() const;

	private:
		bool valid_ = false;
		std::string name_;
		std::string buffer_;
	};

	std::unordered_set<std::string>& get_search_paths();
	std::string read_file(const std::string& path);
	bool read_file(const std::string& path, std::string* data);
}