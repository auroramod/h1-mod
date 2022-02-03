#pragma once

class html_frame;

class ole_in_place_site final : public IOleInPlaceSite
{
public:
	ole_in_place_site(html_frame* frame);
	virtual ~ole_in_place_site() = default;

private:
	html_frame* frame_;

public: // IOleInPlaceSite interface
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;
	HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd) override;
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) override;
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate() override;
	HRESULT STDMETHODCALLTYPE OnInPlaceActivate() override;
	HRESULT STDMETHODCALLTYPE OnUIActivate() override;
	HRESULT STDMETHODCALLTYPE GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc,
	                                           LPRECT lprcPosRect, LPRECT lprcClipRect,
	                                           LPOLEINPLACEFRAMEINFO lpFrameInfo) override;
	HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtent) override;
	HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable) override;
	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() override;
	HRESULT STDMETHODCALLTYPE DiscardUndoState() override;
	HRESULT STDMETHODCALLTYPE DeactivateAndUndo() override;
	HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect) override;
};
