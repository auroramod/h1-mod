#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

class rcon final : public component_interface
{
public:
	void post_unpack() override;

	static bool message_redirect(const std::string& message);

private:
	static void setup_redirect(const game::netadr_s& target);
	static void clear_redirect();
	static void send_rcon_command(const std::string& password, const std::string& data);
	static std::string build_status_buffer();
};
