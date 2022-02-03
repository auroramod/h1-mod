#include <std_include.hpp>
#include "html_frame.hpp"

html_dispatch::html_dispatch(html_frame* frame) : frame_(frame)
{
}

HRESULT html_dispatch::QueryInterface(const IID& riid, LPVOID* ppvObj)
{
	if (!memcmp(&riid, &IID_IUnknown, sizeof(GUID)) ||
		!memcmp(&riid, &IID_IDispatch, sizeof(GUID)))
	{
		*ppvObj = this;
		this->AddRef();
		return S_OK;
	}

	*ppvObj = nullptr;
	return E_NOINTERFACE;
}

ULONG html_dispatch::AddRef()
{
	return 1;
}

ULONG html_dispatch::Release()
{
	return 1;
}

HRESULT html_dispatch::GetTypeInfoCount(UINT* pctinfo)
{
	return S_FALSE;
}

HRESULT html_dispatch::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	return S_FALSE;
}

HRESULT html_dispatch::GetIDsOfNames(const IID& riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	for (unsigned int i = 0; i < cNames; ++i)
	{
		std::wstring wide_name(rgszNames[i]);
		std::string name(wide_name.begin(), wide_name.end());

		rgDispId[i] = this->frame_->get_callback_id(name);
	}

	return S_OK;
}

HRESULT html_dispatch::Invoke(DISPID dispIdMember, const IID& riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
                              VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	html_frame::callback_params params(pDispParams, pVarResult);
	this->frame_->invoke_callback(dispIdMember, &params);
	return S_OK;
}
