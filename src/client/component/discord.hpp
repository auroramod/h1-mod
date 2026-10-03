#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"

struct DiscordUser;

class discord final : public component_interface
{
public:
	void post_unpack() override;
	void pre_destroy() override;

	static game::Material* get_avatar_material(const std::string& id);
	static void respond(const std::string& id, int reply);

private:
	static void update_discord_frontend();
	static void update_discord_ingame();
	static void update_discord();
	static game::Material* create_avatar_material(const std::string& name, const std::string& data);
	static void download_user_avatar(const std::string& id, const std::string& avatar);
	static void download_default_avatar();
	static void ready(const DiscordUser* request);
	static void errored(const int error_code, const char* message);
	static void join_game(const char* join_secret);
	static std::string get_display_name(const DiscordUser* user);
	static void join_request(const DiscordUser* request);
	static void set_default_bindings();
};
