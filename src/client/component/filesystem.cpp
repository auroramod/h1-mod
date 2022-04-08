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
	namespace
	{
		bool custom_path_registered = false;

		std::string get_binary_directory()
		{
			const auto dir = game_module::get_host_module().get_folder();
			return utils::string::replace(dir, "/", "\\");
		}

		void register_custom_path_stub(const char* path, const char* dir)
		{
			if (!custom_path_registered)
			{
				custom_path_registered = true;

				const auto launcher_dir = get_binary_directory();
				game::FS_AddLocalizedGameDirectory(launcher_dir.data(), "data");
			}

			game::FS_AddLocalizedGameDirectory(path, dir);
		}

		void fs_startup_stub(const char* gamename)
		{
			custom_path_registered = false;
			game::FS_Startup(gamename);
		}
	}

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
			// Set fs_basegame
			dvars::override::register_string("fs_basegame", "h1-mod", game::DVAR_FLAG_WRITE);

			utils::hook::call(SELECT_VALUE(0x1403B76E2, 0x1404ED3E2), fs_startup_stub);
			if (game::environment::is_mp())
			{
				utils::hook::call(0x1404ED823, fs_startup_stub);
			}

			utils::hook::call(SELECT_VALUE(0x1403B8D31, 0x1404EE3D0), register_custom_path_stub);
			utils::hook::call(SELECT_VALUE(0x1403B8D51, 0x1404EE3F0), register_custom_path_stub);
			utils::hook::call(SELECT_VALUE(0x1403B8D90, 0x1404EE42F), register_custom_path_stub);

			get_search_paths().insert(".");
			get_search_paths().insert("h1-mod");
			get_search_paths().insert("data");
		}
	};
}

REGISTER_COMPONENT(filesystem::component)