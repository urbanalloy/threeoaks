#include "EnumDragDropFormats.h"

EnumDragDropFormats::EnumDragDropFormats(std::vector<FORMATETC>& formats) :
	m_nRefCnt(1),
	m_Current(0)

{
	// post: An EnumDeskletDragDropFormats object has been created

	m_DragFormats = formats;
}

void EnumDragDropFormats::AddFormat(FORMATETC& format)
{
	m_DragFormats.push_back(format);
}

	// IUnknown
HRESULT STDMETHODCALLTYPE EnumDragDropFormats::QueryInterface(REFIID riid, void **ppvObject)
{
	if (IsEqualIID(riid, IID_IEnumFORMATETC))
	{
		*ppvObject = static_cast<IEnumFORMATETC *>(this);
	}
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG STDMETHODCALLTYPE EnumDragDropFormats::AddRef(void)
{
	return ++m_nRefCnt;
}

ULONG STDMETHODCALLTYPE EnumDragDropFormats::Release(void)
{
	m_nRefCnt--;
	if(m_nRefCnt > 0)
		return m_nRefCnt;

	delete this;

	return 0;
}

	// IEnumFORMATETC
HRESULT STDMETHODCALLTYPE EnumDragDropFormats::Next(ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched)
{
	int i(0);
	while(i < celt && m_Current < m_DragFormats.size() )
	{
		rgelt[i++] = m_DragFormats[m_Current++];
	}
	if(pceltFetched)
		*pceltFetched = i;

	return (i == celt) ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE EnumDragDropFormats::Skip(ULONG celt)
{
	int i(0);
	while(i < celt && m_Current < m_DragFormats.size() )
	{
		m_Current++;
		i++;
	}

	return (i == celt) ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE EnumDragDropFormats::Reset()
{
	m_Current = 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE EnumDragDropFormats::Clone(IEnumFORMATETC ** ppenum)
{
	if(!ppenum)
	{
		return E_INVALIDARG;
	}
	else
	{
		EnumDeskletDragDropFormats* edddf = new EnumDeskletDragDropFormats(m_DragFormats);
		edddf->m_Current = m_Current;
		edddf->QueryInterface(IID_IEnumFORMATETC,reinterpret_cast<void**>(ppenum));
		edddf->Release();
		return S_OK;
	}
}