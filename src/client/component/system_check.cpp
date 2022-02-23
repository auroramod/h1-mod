#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "system_check.hpp"

#include "game/game.hpp"

#include <utils/io.hpp>
#include <utils/cryptography.hpp>

namespace system_check
{
	namespace
	{
		std::string read_zone(const std::string& name)
		{
			std::string data{};
			if (utils::io::read_file(name, &data))
			{
				return data;
			}

			if (utils::io::read_file("zone/" + name, &data))
			{
				return data;
			}

			return {};
		}

		std::string hash_zone(const std::string& name)
		{
			const auto data = read_zone(name);
			return utils::cryptography::sha256::compute(data, true);
		}

		bool verify_hashes(const std::unordered_map<std::string, std::string>& zone_hashes)
		{
			for (const auto& zone_hash : zone_hashes)
			{
				const auto hash = hash_zone(zone_hash.first);
				if (hash != zone_hash.second)
				{
					return false;
				}
			}

			return true;
		}

		bool is_system_valid()
		{
			static std::unordered_map<std::string, std::string> mp_zone_hashes =
			{
				{"patch_common_mp.ff", "3F44B0CFB0B8E0FBD9687C2942204AB7F11E66E6E15C73B8B4A5EB5920115A31"},
			};

			static std::unordered_map<std::string, std::string> sp_zone_hashes =
			{
				// Steam doesn't necessarily deliver this file :(
				{"patch_common.ff", "BB0617DD94AF2F511571E7184BBEDE76E64D97E5D0DAFDB457F00717F035EBF0"},
			};


			return verify_hashes(mp_zone_hashes) && (game::environment::is_dedi() || verify_hashes(sp_zone_hashes));
		}

		void verify_binary_version()
		{
			const auto value = *reinterpret_cast<DWORD*>(0x140001337);
			if (value != 0xFFB8006D && value != 0xFFB80080)
			{
				throw std::runtime_error("Unsupported Call of Duty: Modern Warfare Remastered version(1.4)");
			}
		}
	}

	bool is_valid()
	{
		static auto valid = is_system_valid();
		return valid;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			verify_binary_version();

			if (!is_valid())
			{
				MessageBoxA(nullptr, "Your game files are outdated or unsupported.\n"
					"Please get the latest officially supported Call of Duty: Modern Warfare Remastered 1.4 files, or you will get random crashes and issues.",
					"Invalid game files!", MB_ICONINFORMATION);
			}
		}
	};
}

REGISTER_COMPONENT(system_check::component)
