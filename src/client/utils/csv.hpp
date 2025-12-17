#pragma once

#include <string>
#include <vector>

namespace utils::csv
{
	struct row
	{
		char** fields;
		int num_fields;
	};

	struct parser_info_raw
	{
		char* buffer;
		int buffer_len;
		char delimeter;
		row** rows;
		int num_rows;
	};

	class parser_raw
	{
	private:
		parser_info_raw raw_info{};

	public:
		parser_raw(const char* data, int data_len, char delimeter = ',');
		parser_raw();
		~parser_raw();

		int get_num_rows();
		row** get_rows();
		int get_max_columns();

	private:
		std::vector<std::string> split_rows();
		std::vector<std::string> split_fields(const std::string& src);

		void parse();
	};

	struct parser_info
	{
		char* buffer;
		FILE* fp;
	};

	class parser
	{
	private:
		parser_info info{};
		parser_raw* raw;

	public:
		parser(const std::string& buffer, char delimeter = ',');
		~parser();

		bool valid();

		int get_num_rows();
		row** get_rows();
		int get_max_columns();
	};
}