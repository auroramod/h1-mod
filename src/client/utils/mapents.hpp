#pragma once

namespace mapents
{
	using token_name_callback = std::function<std::string(const std::uint32_t)>;
	using token_id_callback = std::function<std::uint32_t(const std::string&)>;

	struct spawn_var
	{
		std::string key;
		std::string value;
		bool sl_string;
	};

	class mapents_entity
	{
	public:
		void clear();
		void add_var(const spawn_var& var);

		std::string get(const std::string& key) const;
		std::vector<spawn_var> get_var_list() const;

	private:
		std::vector<spawn_var> vars;
	};

	struct mapents_list
	{
		std::vector<mapents_entity> entities;
	};

	mapents_list parse(const std::string& data, const token_name_callback& token_name);
}
