#pragma once
#include "loader/component_loader.hpp"

#include "game/structs.hpp"
#include "game/game.hpp"

class mods final : public component_interface
{
public:
	void post_unpack() override;

	static void set_mod(const std::string& path, bool change_fs_game = true);
	static std::optional<std::string> get_mod();
	static std::vector<std::string> get_mod_list();
	static bool mod_exists(const std::string& folder);
	static std::optional<nlohmann::json> get_mod_info(const std::string& mod);
	static void load(const std::string& path);
	static void unload();
	static void read_stats();
	static void execute_restart(const std::optional<game::netadr_s>& server = {});

private:
	static void db_release_xassets_stub();
	static void restart();
	static void reload_omnvars();
	static void reset_fonts();
	static bool mod_requires_restart(const std::string& path);
	static void set_filesystem_data(const std::string& path, bool change_fs_game);
	static bool can_use_vid_restart();
	static void do_vid_restart(const std::optional<game::netadr_s>& server);
	static void do_full_restart(const std::optional<game::netadr_s>& server);
};
