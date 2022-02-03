#pragma once

class html_frame;

class ole_in_place_frame final : public IOleInPlaceFrame
{
public:
	ole_in_place_frame(html_frame* frame);
	virtual ~ole_in_place_frame() = default;

private:
	html_frame* frame_;

public: // IOleInPlaceFrame interface
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;
	HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd) override;
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) override;
	HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder) override;
	HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) override;
	HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths) override;
	HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR pszObjName) override;
	HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) override;
	HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) override;
	HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared) override;
	HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText) override;
	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable) override;
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID) override;
};
