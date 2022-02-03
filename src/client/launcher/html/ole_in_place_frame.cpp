#include <std_include.hpp>
#include "html_frame.hpp"

ole_in_place_frame::ole_in_place_frame(html_frame* frame): frame_(frame)
{
}

HRESULT ole_in_place_frame::QueryInterface(REFIID /*riid*/, LPVOID* /*ppvObj*/)
{
	return E_NOTIMPL;
}

ULONG ole_in_place_frame::AddRef()
{
	return 1;
}

ULONG ole_in_place_frame::Release()
{
	return 1;
}

HRESULT ole_in_place_frame::GetWindow(HWND* lphwnd)
{
	*lphwnd = this->frame_->get_window();
	return S_OK;
}

HRESULT ole_in_place_frame::ContextSensitiveHelp(BOOL /*fEnterMode*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_frame::GetBorder(LPRECT /*lprectBorder*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_frame::RequestBorderSpace(LPCBORDERWIDTHS /*pborderwidths*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_frame::SetBorderSpace(LPCBORDERWIDTHS /*pborderwidths*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_frame::SetActiveObject(IOleInPlaceActiveObject* /*pActiveObject*/, LPCOLESTR /*pszObjName*/)
{
	return S_OK;
}

HRESULT ole_in_place_frame::InsertMenus(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_frame::SetMenu(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
{
	return S_OK;
}

HRESULT ole_in_place_frame::RemoveMenus(HMENU /*hmenuShared*/)
{
	return E_NOTIMPL;
}

HRESULT ole_in_place_frame::SetStatusText(LPCOLESTR /*pszStatusText*/)
{
	return S_OK;
}

HRESULT ole_in_place_frame::EnableModeless(BOOL /*fEnable*/)
{
	return S_OK;
}

HRESULT ole_in_place_frame::TranslateAcceleratorA(LPMSG /*lpmsg*/, WORD /*wID*/)
{
	return E_NOTIMPL;
}
