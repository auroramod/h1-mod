#include <std_include.hpp>
#include "html_frame.hpp"

doc_host_ui_handler::doc_host_ui_handler(html_frame* frame): frame_(frame)
{
}

HRESULT doc_host_ui_handler::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	auto client_site = this->frame_->get_client_site();
	if (client_site)
	{
		return client_site->QueryInterface(riid, ppvObj);
	}

	return E_NOINTERFACE;
}

ULONG doc_host_ui_handler::AddRef()
{
	return 1;
}

ULONG doc_host_ui_handler::Release()
{
	return 1;
}

HRESULT doc_host_ui_handler::ShowContextMenu(DWORD /*dwID*/, POINT* /*ppt*/, IUnknown* /*pcmdtReserved*/,
                                             IDispatch* /*pdispReserved*/)
{
	return S_OK;
}

HRESULT doc_host_ui_handler::ShowUI(DWORD /*dwID*/, IOleInPlaceActiveObject* /*pActiveObject*/,
                                    IOleCommandTarget* /*pCommandTarget*/,
                                    IOleInPlaceFrame* /*pFrame*/, IOleInPlaceUIWindow* /*pDoc*/)
{
	return S_OK;
}

HRESULT doc_host_ui_handler::HideUI()
{
	return S_OK;
}

HRESULT doc_host_ui_handler::UpdateUI()
{
	return S_OK;
}

HRESULT doc_host_ui_handler::EnableModeless(BOOL /*fEnable*/)
{
	return S_OK;
}

HRESULT doc_host_ui_handler::OnDocWindowActivate(BOOL /*fActivate*/)
{
	return S_OK;
}

HRESULT doc_host_ui_handler::OnFrameWindowActivate(BOOL /*fActivate*/)
{
	return S_OK;
}

HRESULT doc_host_ui_handler::ResizeBorder(LPCRECT /*prcBorder*/, IOleInPlaceUIWindow* /*pUIWindow*/,
                                          BOOL /*fRameWindow*/)
{
	return S_OK;
}

HRESULT doc_host_ui_handler::TranslateAcceleratorA(LPMSG /*lpMsg*/, const GUID* pguidCmdGroup, DWORD /*nCmdID*/)
{
	pguidCmdGroup = nullptr;
	return S_FALSE;
}

HRESULT doc_host_ui_handler::GetOptionKeyPath(LPOLESTR* /*pchKey*/, DWORD /*dw*/)
{
	return S_FALSE;
}

HRESULT doc_host_ui_handler::GetDropTarget(IDropTarget* /*pDropTarget*/, IDropTarget** /*ppDropTarget*/)
{
	return S_FALSE;
}

HRESULT doc_host_ui_handler::GetExternal(IDispatch** ppDispatch)
{
	*ppDispatch = this->frame_->get_html_dispatch();
	return (*ppDispatch) ? S_OK : S_FALSE;
}

HRESULT doc_host_ui_handler::FilterDataObject(IDataObject* /*pDO*/, IDataObject** ppDORet)
{
	*ppDORet = nullptr;
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE doc_host_ui_handler::TranslateUrl(DWORD /*dwTranslate*/, OLECHAR __RPC_FAR* /*pchURLIn*/,
                                                            OLECHAR __RPC_FAR* __RPC_FAR* ppchURLOut)
{
	*ppchURLOut = nullptr;
	return S_FALSE;
}

HRESULT doc_host_ui_handler::GetHostInfo(DOCHOSTUIINFO __RPC_FAR * pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DPI_AWARE /*| DOCHOSTUIFLAG_SCROLL_NO*/;
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

	return S_OK;
}
