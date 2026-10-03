#include <std_include.hpp>
#include "system_check.hpp"

#include "game/game.hpp"

#include <utils/nt.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

void system_check::post_load()
{
	verify_binary_version();

	std::thread([]
	{
		if (!is_valid())
		{
			MSG_BOX_INFO("Your game files are outdated or unsupported.\n"
				"Please get the latest officially supported Call of Duty: Modern Warfare Remastered files, or you will get random crashes and issues.");
		}
	}).detach();
}

bool system_check::is_valid()
{
	static auto valid = is_system_valid();
	return valid;
}

std::string system_check::read_zone(const std::string& name)
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

std::string system_check::hash_zone(const std::string& name)
{
	const auto data = read_zone(name);
	return utils::cryptography::md5::compute(data, true);
}

bool system_check::verify_hashes(const std::unordered_map<std::string, std::string>& zone_hashes)
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

bool system_check::is_system_valid()
{
	// 1.15 zones (binary is 1.04)
	static std::unordered_map<std::string, std::string> mp_zone_hashes =
	{
		{"patch_ui_mp.ff", "A308EE76F49E7FF6B30B33CB37D77282"},
	};

	static std::unordered_map<std::string, std::string> sp_zone_hashes =
	{
		{"patch_icbm.ff", "05581F47DC7965C0272D50538A30660A"},
	};

	return verify_hashes(mp_zone_hashes) && (game::environment::is_dedi() || verify_hashes(sp_zone_hashes));
}

void system_check::verify_binary_version()
{
	if (utils::nt::is_wine())
	{
		return;
	}

	const auto value = *reinterpret_cast<DWORD*>(0x140001337);
	if (game::environment::is_sp())
	{
		if (value == 0x60202B6A || value == 0xBC0E9FE)
		{
			return;
		}

		throw std::runtime_error("Unsupported Call of Duty: Modern Warfare Remastered singleplayer version (1.15)");
	}

	if (value == 0xFFB80080)
	{
		return;
	}

	throw std::runtime_error("Unsupported Call of Duty: Modern Warfare Remastered multiplayer version (1.04)");
}

REGISTER_COMPONENT(system_check)
