#pragma once
#define CLIENT_DATA_FOLDER "cdata"
#include <utils/http.hpp>

namespace updater
{
	void relaunch();
	void set_has_tried_update(bool tried);
	bool get_has_tried_update();
	bool auto_updates_enabled();
	bool is_update_check_done();
	bool is_update_download_done();
	bool get_update_check_status();
	bool get_update_download_status();
	bool is_update_available();
	bool is_restart_required();
	std::string get_last_error();
	std::string get_current_file();
	void cancel_update();
	void start_update_check();
	void start_update_download();
}
