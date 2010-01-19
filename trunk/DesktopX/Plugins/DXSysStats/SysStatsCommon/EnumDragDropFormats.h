#ifndef EnumDragDropFormats_H_20202019
#define EnumDragDropFormats_H_20202019

#include <windows.h>
#include <oleidl.h>
#include <prsht.h>
#include <vector>

class EnumDragDropFormats : public IEnumFORMATETC
{

private:
	long m_nRefCnt;
	std::vector<FORMATETC> m_DragFormats;
	long m_Current;

public:
	EnumDragDropFormats(std::vector<FORMATETC>& formats);
	void AddFormat(FORMATETC& format);

	// IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

	// IEnumFORMATETC
	virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched);
	virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt);

	virtual HRESULT STDMETHODCALLTYPE Reset();
	virtual HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC ** ppenum);
};

#endif//EnumDeskletDragDropFormats_H_20202019