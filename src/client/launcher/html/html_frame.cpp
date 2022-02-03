#include <std_include.hpp>
#include "html_frame.hpp"

#include <utils/nt.hpp>

std::atomic<int> html_frame::frame_count_ = 0;

html_frame::callback_params::callback_params(DISPPARAMS* params, VARIANT* res) : result(res)
{
	for (auto i = params->cArgs; i > 0; --i)
	{
		auto* param = &params->rgvarg[i - 1];
		this->arguments.emplace_back(param);
	}
}

html_frame::html_frame()
	: in_place_frame_(this)
	  , in_place_site_(this)
	  , ui_handler_(this)
	  , client_site_(this)
	  , html_dispatch_(this)
{
	if (frame_count_++ == 0 && OleInitialize(nullptr) != S_OK)
	{
		throw std::runtime_error("Unable to initialize the OLE library");
	}

	auto needs_restart = false;
	needs_restart |= set_browser_feature("FEATURE_BROWSER_EMULATION", 11000);
	needs_restart |= set_browser_feature("FEATURE_GPU_RENDERING", 1);

	if (needs_restart)
	{
		utils::nt::relaunch_self();
		utils::nt::terminate(0);
	}
}

html_frame::~html_frame()
{
	if (--frame_count_ <= 0)
	{
		frame_count_ = 0;
		OleUninitialize();
	}
}

void html_frame::object_deleter(IUnknown* object)
{
	if (object)
	{
		object->Release();
	}
}

HWND html_frame::get_window() const
{
	return this->window_;
}

std::shared_ptr<IOleObject> html_frame::get_browser_object() const
{
	return this->browser_object_;
}

ole_in_place_frame* html_frame::get_in_place_frame()
{
	return &this->in_place_frame_;
}

ole_in_place_site* html_frame::get_in_place_site()
{
	return &this->in_place_site_;
}

doc_host_ui_handler* html_frame::get_ui_handler()
{
	return &this->ui_handler_;
}

ole_client_site* html_frame::get_client_site()
{
	return &this->client_site_;
}

html_dispatch* html_frame::get_html_dispatch()
{
	return &this->html_dispatch_;
}

std::shared_ptr<IWebBrowser2> html_frame::get_web_browser() const
{
	if (!this->browser_object_) return {};

	IWebBrowser2* web_browser = nullptr;
	if (FAILED(this->browser_object_->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&web_browser)))
		|| !web_browser)
		return {};

	return std::shared_ptr<IWebBrowser2>(web_browser, object_deleter);
}

std::shared_ptr<IDispatch> html_frame::get_dispatch() const
{
	const auto web_browser = this->get_web_browser();
	if (!web_browser) return {};

	IDispatch* dispatch = nullptr;
	if (FAILED(web_browser->get_Document(&dispatch)) || !dispatch) return {};

	return std::shared_ptr<IDispatch>(dispatch, object_deleter);
}

std::shared_ptr<IHTMLDocument2> html_frame::get_document() const
{
	const auto dispatch = this->get_dispatch();
	if (!dispatch) return {};

	IHTMLDocument2* document = nullptr;
	if (FAILED(dispatch->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&document)))
		|| !document)
		return {};

	return std::shared_ptr<IHTMLDocument2>(document, object_deleter);
}

void html_frame::initialize(const HWND window)
{
	if (this->window_) return;
	this->window_ = window;

	this->create_browser();
	this->initialize_browser();
}

void html_frame::create_browser()
{
	LPCLASSFACTORY class_factory = nullptr;
	if (FAILED(
		CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, nullptr, IID_IClassFactory,
			reinterpret_cast<void **>(&class_factory))) || !class_factory)
	{
		throw std::runtime_error("Unable to get the class factory");
	}

	IOleObject* browser_object = nullptr;
	class_factory->CreateInstance(nullptr, IID_IOleObject, reinterpret_cast<void**>(&browser_object));
	class_factory->Release();

	if (!browser_object)
	{
		throw std::runtime_error("Unable to create browser object");
	}

	this->browser_object_ = std::shared_ptr<IOleObject>(browser_object, [](IOleObject* browser_object)
	{
		if (browser_object)
		{
			browser_object->Close(OLECLOSE_NOSAVE);
			object_deleter(browser_object);
		}
	});
}

void html_frame::initialize_browser()
{
	this->browser_object_->SetClientSite(this->get_client_site());
	this->browser_object_->SetHostNames(L"Hostname", nullptr);

	RECT rect;
	GetClientRect(this->get_window(), &rect);
	OleSetContainedObject(this->browser_object_.get(), TRUE);

	this->browser_object_->DoVerb(OLEIVERB_SHOW, nullptr, this->get_client_site(), -1, this->get_window(), &rect);
	this->resize(rect.right, rect.bottom);
}

bool html_frame::set_browser_feature(const std::string& feature, DWORD value)
{
	const auto registry_path = R"(SOFTWARE\Microsoft\Internet Explorer\Main\FeatureControl\)" + feature;

	HKEY key = nullptr;
	if (RegCreateKeyA(HKEY_CURRENT_USER, registry_path.data(), &key) == ERROR_SUCCESS)
	{
		RegCloseKey(key);
	}

	key = nullptr;
	if (RegOpenKeyExA(
		HKEY_CURRENT_USER, registry_path.data(), 0,
		KEY_ALL_ACCESS, &key) != ERROR_SUCCESS)
	{
		return false; // Error :(
	}

	const utils::nt::library self;
	const auto name = self.get_name();

	DWORD type{};
	auto is_new = true;
	if (RegQueryValueExA(key, name.data(), nullptr, &type, nullptr, nullptr) == ERROR_SUCCESS)
	{
		is_new = false;
	}

	RegSetValueExA(key, name.data(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
	RegCloseKey(key);

	return is_new;
}

void html_frame::resize(const DWORD width, const DWORD height) const
{
	auto web_browser = this->get_web_browser();
	if (web_browser)
	{
		web_browser->put_Left(0);
		web_browser->put_Top(0);
		web_browser->put_Width(width);
		web_browser->put_Height(height);
	}
}

bool html_frame::load_url(const std::string& url) const
{
	auto web_browser = this->get_web_browser();
	if (!web_browser) return false;

	std::wstring wide_url(url.begin(), url.end());

	VARIANT my_url;
	VariantInit(&my_url);
	my_url.vt = VT_BSTR;
	my_url.bstrVal = SysAllocString(wide_url.data());

	const auto _ = gsl::finally([&my_url]() { VariantClear(&my_url); });
	if (!my_url.bstrVal) return false;

	return SUCCEEDED(web_browser->Navigate2(&my_url, nullptr, nullptr, nullptr, nullptr));
}

bool html_frame::load_html(const std::string& html) const
{
	if (!this->load_url("about:blank")) return false;

	const auto document = this->get_document();
	if (!document) return false;

	SAFEARRAYBOUND safe_array_bound = {1, 0};
	auto safe_array = SafeArrayCreate(VT_VARIANT, 1, &safe_array_bound);
	if (!safe_array) return false;

	const auto _ = gsl::finally([safe_array]() { SafeArrayDestroy(safe_array); });

	VARIANT* variant = nullptr;
	if (FAILED(SafeArrayAccessData(safe_array, reinterpret_cast<void**>(&variant))) || !variant) return false;

	std::wstring wide_html(html.begin(), html.end());

	variant->vt = VT_BSTR;
	variant->bstrVal = SysAllocString(wide_html.data());
	if (!variant->bstrVal) return false;

	document->write(safe_array);
	document->close();

	return true;
}

int html_frame::get_callback_id(const std::string& name)
{
	for (auto i = 0u; i < this->callbacks_.size(); ++i)
	{
		if (this->callbacks_[i].first == name)
		{
			return i;
		}
	}

	return -1;
}

void html_frame::invoke_callback(const int id, callback_params* params)
{
	if (id >= 0 && static_cast<unsigned int>(id) < this->callbacks_.size())
	{
		this->callbacks_[id].second(params);
	}
}

void html_frame::register_callback(const std::string& name, const std::function<void(callback_params*)>& callback)
{
	this->callbacks_.emplace_back(name, callback);
}
