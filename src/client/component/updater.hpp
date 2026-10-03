#pragma once
#include "loader/component_loader.hpp"

#define CLIENT_DATA_FOLDER "cdata"

#include <utils/http.hpp>

class updater final : public component_interface
{
public:
	void post_unpack() override;

	static std::optional<utils::http::result> get_server_file(const std::string& endpoint);
	static void relaunch();
	static void set_has_tried_update(bool tried);
	static bool get_has_tried_update();
	static bool auto_updates_enabled();
	static bool is_update_check_done();
	static bool is_update_download_done();
	static bool get_update_check_status();
	static bool get_update_download_status();
	static bool is_update_available();
	static bool is_restart_required();
	static std::string get_last_error();
	static std::string get_current_file();
	static void cancel_update();
	static void start_update_check();
	static void start_update_download();

private:
	static std::string select(const std::string& main, const std::string& develop);
	static std::string load_binary_name();
	static std::string get_binary_name();
	static void notify(const std::string& name);
	static void set_update_check_status(bool done, bool success, const std::string& error = {});
	static void set_update_download_status(bool done, bool success, const std::string& error = {});
	static bool check_file(const std::string& name, const std::string& sha);
	static std::string get_time_str();
	static std::optional<utils::http::result> download_data_file(const std::string& name);
	static std::optional<utils::http::result> download_file_list();
	static bool has_old_data_files();
	static void delete_old_data_files();
	static bool is_update_cancelled();
	static bool write_file(const std::string& name, const std::string& data);
	static void delete_old_file();
	static void reset_data();
	static std::vector<std::string> find_garbage_files(const std::vector<std::string>& update_files);
	static std::string get_mode_flag();
	static std::string curl_error(CURLcode code);
};
