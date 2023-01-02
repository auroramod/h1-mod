#pragma once

namespace map_rotation
{
	struct parse_rotation_error : public std::exception
	{
		const char* what() const noexcept override { return "Rotation parse error"; }
	};

	class rotation_data
	{
	public:
		using rotation_entry = std::pair<std::string, std::string>;

		rotation_data();

		void randomize();

		// In case a new way to enrich the map rotation is added (other than sv_mapRotation)
		// this method should be called to add a new entry (gamemode/map & value)
		void add_entry(const std::string& key, const std::string& value);

		[[nodiscard]] bool contains(const std::string& key, const std::string& value) const;
		[[nodiscard]] bool empty() const noexcept;
		[[nodiscard]] std::size_t get_entries_size() const noexcept;
		[[nodiscard]] rotation_entry& get_next_entry();

		void parse(const std::string& data);

	private:
		std::vector<rotation_entry> rotation_entries_;
		std::size_t index_;
	};
}
