#pragma once
#include <utils/concurrency.hpp>

namespace scripting
{
	using shared_table_t = std::unordered_map<std::string, std::string>;

	extern std::unordered_map<int, std::unordered_map<std::string, int>> fields_table;
	extern std::unordered_map<std::string, std::unordered_map<std::string, const char*>> script_function_table;
	extern utils::concurrency::container<shared_table_t> shared_table;
}