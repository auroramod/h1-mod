#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/cryptography.hpp>

class auth final : public component_interface
{
public:
	void post_unpack() override;

	static uint64_t get_guid();

private:
	static std::string get_hdd_serial();
	static std::string get_hw_profile_guid();
	static std::string get_protected_data();
	static std::string get_key_entropy();
	static bool load_key(utils::cryptography::ecc::key& key);
	static utils::cryptography::ecc::key generate_key();
	static utils::cryptography::ecc::key load_or_generate_key();
	static utils::cryptography::ecc::key get_key_internal();
	static const utils::cryptography::ecc::key& get_key();
	static std::string hash_string(const std::string& str);
	static int send_connect_data_stub(game::netsrc_t sock, game::netadr_s* adr, const char* format, const int len);
	static void direct_connect(game::netadr_s* from, game::msg_t* msg);
	static void* get_direct_connect_stub();
};
