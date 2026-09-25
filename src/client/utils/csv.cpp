#include <std_include.hpp>
#include "csv.hpp"

#include "utils/string.hpp"

#include <stdexcept>

namespace utils::csv
{
	int parser_raw::get_num_rows()
	{
		return this->raw_info.num_rows;
	}

	row** parser_raw::get_rows()
	{
		return this->raw_info.rows;
	}

	int parser_raw::get_max_columns()
	{
		int max_columns = 0;
		if (this->raw_info.rows)
		{
			for (int i = 0; i < this->raw_info.num_rows; i++)
			{
				if (this->raw_info.rows[i])
				{
					if (max_columns < this->raw_info.rows[i]->num_fields)
					{
						max_columns = this->raw_info.rows[i]->num_fields;
					}
				}
			}
		}
		return max_columns;
	}

	std::vector<std::string> parser_raw::split_rows()
	{
		std::vector<std::string> res;
		std::string buffer;
		std::string src = this->raw_info.buffer;

		for (std::size_t i = 0; i < src.size(); i++)
		{
			const auto c = src[i];
			if (c == '\\' && i + 1 < src.size())
			{
				const auto cc = src[i + 1];
				if (cc == 'n')
				{
					buffer.push_back('\n');
					i++;
					continue;
				}
				else if (cc == 't')
				{
					buffer.push_back('\t');
					i++;
					continue;
				}
			}

			if (c == '\r')
			{
				continue;
			}
			else if (c == '\n')
			{
				res.push_back(buffer);
				buffer.clear();
				continue;
			}

			buffer.push_back(c);
		}

		if (!buffer.empty())
		{
			res.push_back(buffer);
		}

		return res;
	}

	std::vector<std::string> parser_raw::split_fields(const std::string& src)
	{
		std::vector<std::string> res;
		std::string buffer;
		auto in_quote = false;

		for (std::size_t i = 0; i < src.size(); i++)
		{
			const auto c = src[i];
			if (c == '"')
			{
				in_quote = !in_quote;
				continue;
			}

			if (c != ',' || in_quote)
			{
				buffer.push_back(c);
			}
			else if (c == ',' && !in_quote)
			{
				res.push_back(buffer);
				buffer.clear();
			}
		}

		if (!buffer.empty())
		{
			res.push_back(buffer);
		}

		return res;
	}

	void parser_raw::parse()
	{
		auto split_rows = this->split_rows();

		this->raw_info.num_rows = static_cast<int>(split_rows.size());
		this->raw_info.rows = nullptr;
		if (this->raw_info.num_rows)
		{
			this->raw_info.rows = new csv::row * [this->raw_info.num_rows];
			if (this->raw_info.rows)
			{
				for (int i = 0; i < this->raw_info.num_rows; i++)
				{
					this->raw_info.rows[i] = new csv::row;
					if (this->raw_info.rows[i])
					{
						auto split_fields = this->split_fields(split_rows[i]);

						this->raw_info.rows[i]->num_fields = static_cast<int>(split_fields.size());
						this->raw_info.rows[i]->fields = nullptr;
						if (this->raw_info.rows[i]->num_fields)
						{
							this->raw_info.rows[i]->fields = new char* [this->raw_info.rows[i]->num_fields];
							if (this->raw_info.rows[i]->fields)
							{
								for (int j = 0; j < this->raw_info.rows[i]->num_fields; j++)
								{
									auto field_len = split_fields[j].size() + 1;
									this->raw_info.rows[i]->fields[j] = new char[field_len];
									memset(this->raw_info.rows[i]->fields[j], 0, field_len);
									if (this->raw_info.rows[i]->fields[j])
									{
										strcpy_s(this->raw_info.rows[i]->fields[j], field_len, split_fields[j].data());
										this->raw_info.rows[i]->fields[j][field_len - 1] = '\0';
									}
								}
							}
						}
					}
				}
			}
		}
	}

	parser_raw::parser_raw(const char* data, int data_len, char delimeter)
	{
		this->raw_info = { 0 };

		if (data)
		{
			this->raw_info.buffer = const_cast<char*>(data);
			this->raw_info.buffer_len = data_len;
		}
		else
		{
			throw std::runtime_error("CSV: Data is invalid!");
		}

		this->raw_info.delimeter = delimeter;

		this->parse();
	}

	parser_raw::parser_raw()
	{

	}

	parser_raw::~parser_raw()
	{
		for (int i = 0; i < this->raw_info.num_rows; i++)
		{
			if (this->raw_info.rows[i])
			{
				if (this->raw_info.rows[i]->fields)
				{
					for (int j = 0; j < this->raw_info.rows[i]->num_fields; j++)
					{
						if (this->raw_info.rows[i]->fields[j])
						{
							delete[] this->raw_info.rows[i]->fields[j];
						}
					}
					delete[] this->raw_info.rows[i]->fields;
				}
				delete this->raw_info.rows[i];
			}
		}
		delete[] this->raw_info.rows;
	}

	bool parser::valid()
	{
		return this->info.fp != nullptr;
	}

	int parser::get_num_rows()
	{
		return this->raw->get_num_rows();
	}

	row** parser::get_rows()
	{
		return this->raw->get_rows();
	}

	int parser::get_max_columns()
	{
		return this->raw->get_max_columns();
	}

	parser::parser(const std::string& buffer, char delimeter)
	{
		this->info.fp = nullptr;
		this->info.buffer = nullptr;

		if (buffer.empty())
		{
			throw std::runtime_error("CSV: Provided buffer is empty!");
		}

		this->info.buffer = reinterpret_cast<char*>(malloc(buffer.size() + 1));
		if (!this->info.buffer)
		{
			throw std::runtime_error("CSV: Couldn't allocate memory for buffer!");
		}

		memset(this->info.buffer, 0, buffer.size() + 1);
		memcpy(this->info.buffer, buffer.data(), buffer.size());

		this->raw = new parser_raw(this->info.buffer, static_cast<int>(buffer.size()), delimeter);
	}

	parser::~parser()
	{
		if (this->info.buffer)
		{
			free(this->info.buffer);
			this->info.buffer = nullptr;
		}

		delete this->raw;
		this->raw = nullptr;
	}
}
