#include <std_include.hpp>
#include "html_frame.hpp"

ole_client_site::ole_client_site(html_frame* frame): frame_(frame)
{
}

HRESULT ole_client_site::QueryInterface(REFIID riid, LPVOID* ppvObject)
{
	if (!memcmp(&riid, &IID_IUnknown, sizeof(GUID)) ||
		!memcmp(&riid, &IID_IOleClientSite, sizeof(GUID)))
	{
		*ppvObject = this;
		this->AddRef();
		return S_OK;
	}

	if (!memcmp(&riid, &IID_IOleInPlaceSite, sizeof(GUID)))
	{
		auto in_place_site = this->frame_->get_in_place_site();
		in_place_site->AddRef();
		*ppvObject = in_place_site;
		return S_OK;
	}

	if (!memcmp(&riid, &IID_IDocHostUIHandler, sizeof(GUID)))
	{
		auto ui_handler = this->frame_->get_ui_handler();
		ui_handler->AddRef();
		*ppvObject = ui_handler;
		return S_OK;
	}

	*ppvObject = nullptr;
	return E_NOINTERFACE;
}

ULONG ole_client_site::AddRef()
{
	return 1;
}

ULONG ole_client_site::Release()
{
	return 1;
}

HRESULT ole_client_site::SaveObject()
{
	return E_NOTIMPL;
}

HRESULT ole_client_site::GetMoniker(DWORD /*dwAssign*/, DWORD /*dwWhichMoniker*/, IMoniker** /*ppmk*/)
{
	return E_NOTIMPL;
}

HRESULT ole_client_site::GetContainer(LPOLECONTAINER* ppContainer)
{
	*ppContainer = nullptr;
	return E_NOINTERFACE;
}

HRESULT ole_client_site::ShowObject()
{
	return NOERROR;
}

HRESULT ole_client_site::OnShowWindow(BOOL /*fShow*/)
{
	return E_NOTIMPL;
}

HRESULT ole_client_site::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}
