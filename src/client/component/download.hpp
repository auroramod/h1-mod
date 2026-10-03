#pragma once
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include <utils/info_string.hpp>

class download final : public component_interface
{
public:
	void pre_destroy() override;

	struct file_t
	{
		std::string name;
		std::string hash;
	};

	static void start_download(const game::netadr_s& target, const utils::info_string& info, const std::vector<file_t>& files);
	static void stop_download();

private:
	static bool download_aborted();
	static void mark_unactive();
	static void mark_active();
	static bool download_active();
	static int progress_callback(size_t total, size_t progress);
	static void menu_error(const std::string& error);
};
