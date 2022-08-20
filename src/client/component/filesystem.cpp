#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "filesystem.hpp"
#include "game_module.hpp"

#include "game/game.hpp"
#include "dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>

namespace filesystem
{
	file::file(std::string name)
		: name_(std::move(name))
	{
		char* buffer{};
		const auto size = game::FS_ReadFile(this->name_.data(), &buffer);

		if (size >= 0 && buffer)
		{
			this->valid_ = true;
			this->buffer_.append(buffer, size);
			game::FS_FreeFile(buffer);
		}
	}

	bool file::exists() const
	{
		return this->valid_;
	}

	const std::string& file::get_buffer() const
	{
		return this->buffer_;
	}

	const std::string& file::get_name() const
	{
		return this->name_;
	}

	std::unordered_set<std::string>& get_search_paths()
	{
		static std::unordered_set<std::string> search_paths{};
		return search_paths;
	}

	std::string read_file(const std::string& path)
	{
		for (const auto& search_path : get_search_paths())
		{
			const auto path_ = search_path + "/" + path;
			if (utils::io::file_exists(path_))
			{
				return utils::io::read_file(path_);
			}
		}

		return {};
	}

	bool read_file(const std::string& path, std::string* data)
	{
		for (const auto& search_path : get_search_paths())
		{
			const auto path_ = search_path + "/" + path;
			if (utils::io::read_file(path_, data))
			{
				return true;
			}
		}

		return false;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			get_search_paths().insert(".");
			get_search_paths().insert("h1-mod");
			get_search_paths().insert("data");

			// fs_game flags
			utils::hook::set<uint32_t>(SELECT_VALUE(0x40D2A5_b, 0x189275_b), 0);
		}
	};
}

REGISTER_COMPONENT(filesystem::component)