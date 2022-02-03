#include <std_include.hpp>
#include "html_frame.hpp"

ole_in_place_site::ole_in_place_site(html_frame* frame) : frame_(frame)
{
}

HRESULT ole_in_place_site::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	auto client_site = this->frame_->get_client_site();
	if (client_site)
	{
		return client_site->QueryInterface(riid, ppvObj);
	}

	return E_NOINTERFACE;
}

ULONG ole_in_place_site::AddRef()
{
	return 1;
}

ULONG ole_in_place_site::Release()
{
	return 1;
}

HRESULT ole_in_place_site::GetWindow(HWND* lphwnd)
{
	*lphwnd = this->frame_->get_window();
	return S_OK;
}

HRESULT ole_in_place_site::ContextSensitiveHelp(BOOL /*fEnterMode*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_site::CanInPlaceActivate()
{
	return S_OK;
}

HRESULT ole_in_place_site::OnInPlaceActivate()
{
	return S_OK;
}

HRESULT ole_in_place_site::OnUIActivate()
{
	return S_OK;
}

HRESULT ole_in_place_site::GetWindowContext(LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc,
                                            LPRECT /*lprcPosRect*/, LPRECT /*lprcClipRect*/,
                                            LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*lplpFrame = this->frame_->get_in_place_frame();
	*lplpDoc = nullptr;

	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = this->frame_->get_window();
	lpFrameInfo->haccel = nullptr;
	lpFrameInfo->cAccelEntries = 0;

	return S_OK;
}

HRESULT ole_in_place_site::Scroll(SIZE /*scrollExtent*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_site::OnUIDeactivate(BOOL /*fUndoable*/)
{
	return S_OK;
}

HRESULT ole_in_place_site::OnInPlaceDeactivate()
{
	return S_OK;
}

HRESULT ole_in_place_site::DiscardUndoState()
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_site::DeactivateAndUndo()
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_site::OnPosRectChange(LPCRECT lprcPosRect)
{
	IOleInPlaceObject* in_place = nullptr;
	if (!this->frame_->get_browser_object()->QueryInterface(IID_IOleInPlaceObject, reinterpret_cast<void**>(&in_place)))
	{
		in_place->SetObjectRects(lprcPosRect, lprcPosRect);
		in_place->Release();
	}

	return S_OK;
}
