#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "system_check.hpp"
#include "arxan.hpp"

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
				{"patch_common_mp.ff", "E45EF5F29D12A5A47F405F89FBBEE479C0A90D02141ABF852D481689514134A1"},
			};

			static std::unordered_map<std::string, std::string> sp_zone_hashes =
			{
				// Steam doesn't necessarily deliver this file :(
				{"patch_common.ff", "1D32A9770F90ED022AA76F4859B4AB178E194A703383E61AC2CE83B1E828B18F"},
			};


			return verify_hashes(mp_zone_hashes) && (game::environment::is_dedi() || verify_hashes(sp_zone_hashes));
		}

		// need to update these values 
		void verify_binary_version()
		{
			const auto value = *reinterpret_cast<DWORD*>(0x1337_b);

			if (arxan::is_wine())
			{
				if (value != 0xFFB81143)
				{
					throw std::runtime_error("Unsupported Call of Duty: Modern Warfare Remastered version (1.15)");
				}
			}
			else
			{
				if (value != 0x60202B6A && value != 0xBC0E9FE)
				{
					throw std::runtime_error("Unsupported Call of Duty: Modern Warfare Remastered version (1.15)");
				}
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
			// TODO: find correct values for wine
			if (arxan::is_wine())
			{
				return;
			}

			verify_binary_version();

			if (!is_valid())
			{
				MessageBoxA(nullptr, "Your game files are outdated or unsupported.\n"
					"Please get the latest officially supported Call of Duty: Modern Warfare Remastered files, or you will get random crashes and issues.",
					"Invalid game files!", MB_ICONINFORMATION);
			}
		}
	};
}

REGISTER_COMPONENT(system_check::component)
