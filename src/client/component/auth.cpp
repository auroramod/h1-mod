#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "auth.hpp"
#include "command.hpp"
#include "console.hpp"
#include "network.hpp"

#include "game/game.hpp"
#include "steam/steam.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/smbios.hpp>
#include <utils/info_string.hpp>
#include <utils/cryptography.hpp>
#include <utils/properties.hpp>
#include <utils/io.hpp>

namespace auth
{
	namespace
	{
		std::string get_player_suffix()
		{
			static const auto suffix = []() -> std::string
			{
				// other player stuff starts at 2, not 1
				for (auto i = 1; i <= 8; ++i)
				{
					const auto mutex = CreateMutexA(nullptr, FALSE, utils::string::va("h1-mod-player-%d", i));
					if (!mutex)
					{
						break;
					}

					if (GetLastError() != ERROR_ALREADY_EXISTS)
					{
						return i == 1 ? std::string{} : utils::string::va("-%d", i);
					}

					ReleaseMutex(mutex);
					CloseHandle(mutex);
				}

				return {};
			}();

			return suffix;
		}

		std::string get_key_path(const char* name)
		{
			return (utils::properties::get_appdata_path() / utils::string::va("h1-%s%s.key", name, get_player_suffix().data())).generic_string();
		}

		std::string get_hdd_serial()
		{
			DWORD serial{};
			if (!GetVolumeInformationA("C:\\", nullptr, 0, &serial, nullptr, nullptr, nullptr, 0))
			{
				return {};
			}

			return utils::string::va("%08X", serial);
		}

		std::string get_hw_profile_guid()
		{
			auto hw_profile_path = (utils::properties::get_appdata_path() / "h1-guid.dat").generic_string();
			if (utils::io::file_exists(hw_profile_path))
			{
				utils::io::remove_file(hw_profile_path);
			}

			HW_PROFILE_INFO info;
			if (!GetCurrentHwProfileA(&info))
			{
				return {};
			}

			auto hw_profile_info = std::string{ info.szHwProfileGuid, sizeof(info.szHwProfileGuid) };
			return hw_profile_info;
		}

		std::string get_protected_data()
		{
			std::string input = "H1Mod-Auth";

			DATA_BLOB data_in{}, data_out{};
			data_in.pbData = reinterpret_cast<uint8_t*>(input.data());
			data_in.cbData = static_cast<DWORD>(input.size());
			if (CryptProtectData(&data_in, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_LOCAL_MACHINE, &data_out) != TRUE)
			{
				return {};
			}

			const auto size = std::min<DWORD>(data_out.cbData, 52);
			std::string result{ reinterpret_cast<char*>(data_out.pbData), size };
			LocalFree(data_out.pbData);

			return result;
		}

		std::string get_key_entropy()
		{
			std::string entropy{};
			entropy.append(get_player_suffix());
			entropy.append(utils::smbios::get_uuid());
			entropy.append(get_hw_profile_guid());
			entropy.append(get_protected_data());
			entropy.append(get_hdd_serial());

			if (entropy.empty())
			{
				entropy.resize(32);
				utils::cryptography::random::get_data(entropy.data(), entropy.size());
			}

			return entropy;
		}

		bool load_key(utils::cryptography::ecc::key& key)
		{
			std::string data{};

			auto key_path = get_key_path("private");
			if (!utils::io::read_file(key_path, &data))
			{
				return false;
			}

			key.deserialize(data);
			if (!key.is_valid())
			{
				console::error("Loaded key is invalid!\n");
				return false;
			}

			return true;
		}

		utils::cryptography::ecc::key generate_key()
		{
			auto key = utils::cryptography::ecc::generate_key(512, get_key_entropy());
			if (!key.is_valid())
			{
				throw std::runtime_error("Failed to generate cryptographic key!");
			}

			auto key_path = get_key_path("private");
			if (!utils::io::write_file(key_path, key.serialize()))
			{
				console::error("Failed to write cryptographic key!\n");
			}

			console::info("Generated cryptographic key: %llX\n", key.get_hash());
			return key;
		}

		utils::cryptography::ecc::key load_or_generate_key()
		{
			utils::cryptography::ecc::key key{};
			if (load_key(key))
			{
				console::info("Loaded cryptographic key: %llX\n", key.get_hash());
				return key;
			}

			return generate_key();
		}

		utils::cryptography::ecc::key get_key_internal()
		{
			auto key = load_or_generate_key();

			auto key_path = get_key_path("public");
			if (!utils::io::write_file(key_path, key.get_public_key()))
			{
				console::error("Failed to write public key!\n");
			}

			return key;
		}

		const utils::cryptography::ecc::key& get_key()
		{
			static auto key = get_key_internal();
			return key;
		}

		std::string hash_string(const std::string& str)
		{
			const auto value = game::generateHashValue(str.data());
			return utils::string::va("0x%lX", value);
		}

		int send_connect_data_stub(game::netsrc_t sock, game::netadr_s* adr, const char* format, const int len)
		{
			std::string connect_string(format, len);
			game::SV_Cmd_TokenizeString(connect_string.data());
			const auto _ = gsl::finally([]()
			{
				game::SV_Cmd_EndTokenizedString();
			});

			const command::params_sv params;
			if (params.size() < 3)
			{
				return false;
			}

			const utils::info_string info_string{std::string{params[2]}};
			const auto challenge = info_string.get(hash_string("challenge"));

			connect_string.clear();
			connect_string.append(params[0]);
			connect_string.append(" ");
			connect_string.append(params[1]);
			connect_string.append(" ");
			connect_string.append("\"" + info_string.build() + "\"");

			proto::network::connect_info info;
			info.set_publickey(get_key().get_public_key());
			info.set_signature(utils::cryptography::ecc::sign_message(get_key(), challenge));
			info.set_infostring(connect_string);

			network::send(*adr, "connect", info.SerializeAsString());
			return true;
		}

		void direct_connect(game::netadr_s* from, game::msg_t* msg)
		{
			const auto offset = sizeof("connect") + 4;

			proto::network::connect_info info;
			if (msg->cursize < offset || !info.ParseFromArray(msg->data + offset, msg->cursize - offset))
			{
				network::send(*from, "error", "Invalid connect data!", '\n');
				return;
			}

			game::SV_Cmd_EndTokenizedString();
			game::SV_Cmd_TokenizeString(info.infostring().data());

			const command::params_sv params;
			if (params.size() < 3)
			{
				network::send(*from, "error", "Invalid connect string!", '\n');
				return;
			}

			const utils::info_string info_string{std::string{params[2]}};

			const auto steam_id = info_string.get(hash_string("xuid"));
			const auto challenge = info_string.get(hash_string("challenge"));

			if (steam_id.empty() || challenge.empty())
			{
				network::send(*from, "error", "Invalid connect data!", '\n');
				return;
			}

			utils::cryptography::ecc::key key;
			key.set(info.publickey());

			const auto xuid = std::strtoull(steam_id.data(), nullptr, 16);
			if (xuid != key.get_hash())
			{
				network::send(*from, "error",
					utils::string::va("XUID doesn't match the certificate: %llX != %llX", xuid, key.get_hash()), '\n');
				return;
			}

			if (!key.is_valid() || !utils::cryptography::ecc::verify_message(key, challenge, info.signature()))
			{
				network::send(*from, "error", "Challenge signature was invalid!", '\n');
				return;
			}

			game::SV_DirectConnect(from);
		}

		void* get_direct_connect_stub()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.lea(rcx, qword_ptr(rsp, 0x20));
				a.movaps(xmmword_ptr(rsp, 0x20), xmm0);

				a.pushad64();
				a.mov(rdx, rsi);
				a.call_aligned(direct_connect);
				a.popad64();

				a.jmp(0x140488CE2);
			});
		}
	}

	uint64_t get_guid()
	{
		if (game::environment::is_dedi())
		{
			return 0x110000100000000 | (::utils::cryptography::random::get_integer() & ~0x80000000);
		}

		return get_key().get_hash();
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Patch steam id bit check
			if (game::environment::is_sp())
			{
				utils::hook::jump(0x1404FA1B3, 0x1404FA21A, true);
				utils::hook::jump(0x1404FB272, 0x1404FB2B7, true);
				utils::hook::jump(0x1404FB781, 0x1404FB7D3, true);
			}
			else
			{
				utils::hook::jump(0x140571E07, 0x140571E5A); // also kills "disconnected from steam" error
				utils::hook::jump(0x14004B223, 0x14004B4F2);
				utils::hook::jump(0x14004B4AD, 0x14004B4F2);
				utils::hook::jump(0x140572F6F, 0x140572FB0);
				utils::hook::jump(0x140573470, 0x1405734B6);

				utils::hook::jump(0x140488BC1, get_direct_connect_stub(), true);
				utils::hook::call(0x140250ED2, send_connect_data_stub);

				// Skip checks for sending connect packet
				utils::hook::jump(0x1402508FC, 0x140250946);

				// Don't instantly timeout the connecting client ? not sure about this
				utils::hook::set(0x14025136B, 0xC3);
			}

			command::add("guid", []() -> void
			{
				console::info("Your guid: %llX\n", steam::SteamUser()->GetSteamID().bits);
			});
		}
	};
}

REGISTER_COMPONENT(auth::component)
