#pragma once

class html_frame;

class html_dispatch final : public IDispatch
{
public:
	html_dispatch(html_frame* frame);
	virtual ~html_dispatch() = default;

private:
	html_frame* frame_;

public: // IDispatch interface
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
	override;
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
	                                 VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;
};
