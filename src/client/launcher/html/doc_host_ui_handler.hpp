#pragma once

class html_frame;

class doc_host_ui_handler final : public IDocHostUIHandler
{
public:
	doc_host_ui_handler(html_frame* frame);
	virtual ~doc_host_ui_handler() = default;

private:
	html_frame* frame_;

public: // IDocHostUIHandler interface
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObj) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;
	HRESULT STDMETHODCALLTYPE ShowContextMenu(
		DWORD dwID,
		POINT __RPC_FAR * ppt,
		IUnknown __RPC_FAR * pcmdtReserved,
		IDispatch __RPC_FAR * pdispReserved) override;
	HRESULT STDMETHODCALLTYPE ShowUI(
		DWORD dwID,
		IOleInPlaceActiveObject __RPC_FAR * pActiveObject,
		IOleCommandTarget __RPC_FAR * pCommandTarget,
		IOleInPlaceFrame __RPC_FAR * pFrame,
		IOleInPlaceUIWindow __RPC_FAR * pDoc) override;
	HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO __RPC_FAR * pInfo) override;
	HRESULT STDMETHODCALLTYPE HideUI() override;
	HRESULT STDMETHODCALLTYPE UpdateUI() override;
	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable) override;
	HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate) override;
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate) override;
	HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow __RPC_FAR * pUIWindow,
	                                       BOOL fRameWindow) override;
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg, const GUID __RPC_FAR * pguidCmdGroup, DWORD nCmdID)
	override;
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR __RPC_FAR * pchKey, DWORD dw) override;
	HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget __RPC_FAR * pDropTarget,
	                                        IDropTarget __RPC_FAR *__RPC_FAR * ppDropTarget) override;
	HRESULT STDMETHODCALLTYPE GetExternal(IDispatch __RPC_FAR *__RPC_FAR * ppDispatch) override;
	HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate, OLECHAR __RPC_FAR * pchURLIn,
	                                       OLECHAR __RPC_FAR *__RPC_FAR * ppchURLOut) override;
	HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject __RPC_FAR * pDO, IDataObject __RPC_FAR *__RPC_FAR * ppDORet)
	override;
};
