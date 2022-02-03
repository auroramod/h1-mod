#pragma once

class html_frame;

class ole_client_site final : public IOleClientSite
{
public:
	ole_client_site(html_frame* frame);
	virtual ~ole_client_site() = default;

private:
	html_frame* frame_;

public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObject) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;
	HRESULT STDMETHODCALLTYPE SaveObject() override;
	HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk) override;
	HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER FAR* ppContainer) override;
	HRESULT STDMETHODCALLTYPE ShowObject() override;
	HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow) override;
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout() override;
};
