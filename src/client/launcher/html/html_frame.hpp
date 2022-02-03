#pragma once
#include "ole_in_place_frame.hpp"
#include "ole_in_place_site.hpp"
#include "doc_host_ui_handler.hpp"
#include "ole_client_site.hpp"
#include "html_dispatch.hpp"
#include "html_argument.hpp"

class html_frame
{
public:
	class callback_params final
	{
	public:
		callback_params(DISPPARAMS* params, VARIANT* res);

		std::vector<html_argument> arguments;
		html_argument result;
	};

	html_frame();
	virtual ~html_frame();

	void initialize(HWND window);

	void resize(DWORD width, DWORD height) const;
	bool load_url(const std::string& url) const;
	bool load_html(const std::string& html) const;

	HWND get_window() const;

	std::shared_ptr<IOleObject> get_browser_object() const;
	std::shared_ptr<IWebBrowser2> get_web_browser() const;
	std::shared_ptr<IDispatch> get_dispatch() const;
	std::shared_ptr<IHTMLDocument2> get_document() const;

	ole_in_place_frame* get_in_place_frame();
	ole_in_place_site* get_in_place_site();
	doc_host_ui_handler* get_ui_handler();
	ole_client_site* get_client_site();
	html_dispatch* get_html_dispatch();

	int get_callback_id(const std::string& name);
	void invoke_callback(int id, callback_params* params);

	void register_callback(const std::string& name, const std::function<void(callback_params*)>& callback);

private:
	HWND window_ = nullptr;
	std::shared_ptr<IOleObject> browser_object_;

	ole_in_place_frame in_place_frame_;
	ole_in_place_site in_place_site_;
	doc_host_ui_handler ui_handler_;
	ole_client_site client_site_;
	html_dispatch html_dispatch_;

	std::vector<std::pair<std::string, std::function<void(callback_params*)>>> callbacks_;

	void create_browser();
	void initialize_browser();

	static bool set_browser_feature(const std::string& feature, DWORD value);
	static void object_deleter(IUnknown* object);

	static std::atomic<int> frame_count_;
};
