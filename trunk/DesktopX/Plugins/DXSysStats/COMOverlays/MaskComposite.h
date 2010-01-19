// MaskComposite.h : Declaration of the CMaskComposite

#ifndef __MASKCOMPOSITE_H_
#define __MASKCOMPOSITE_H_

#include "resource.h"       // main symbols
#include "SysStatsOverlayCategory.h"
#include "CompositeOverlayImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CMaskComposite
class ATL_NO_VTABLE CMaskComposite :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMaskComposite, &CLSID_MaskComposite>,
	public ISupportErrorInfo,
	public IDispatchImpl<CompositeOverlayImpl<IMaskComposite>, &IID_IMaskComposite, &LIBID_COMOVERLAYSLib>
{
public:
	CMaskComposite();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_MASKCOMPOSITE)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMaskComposite)
	COM_INTERFACE_ENTRY(IMaskComposite)
	COM_INTERFACE_ENTRY(ICompositeOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CMaskComposite)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMaskComposite
public:
	STDMETHOD(get_MaskOverlay)(/*[out, retval]*/ IOverlay* *pVal);
	STDMETHOD(put_MaskOverlay)(/*[in]*/ IOverlay* newVal);
	STDMETHOD(get_MaskOverlayIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_MaskOverlayIndex)(/*[in]*/ long newVal);

// ISimpleOverlay2
//	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);

// IOverlay
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(clone)(IOverlay * * pRet);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * Meters, LONG hDlg);
	STDMETHOD(Render)(LONG hdc);

protected:
	virtual IOverlay *createInstance();

	IUnknown *pWeakTarget;

private:
	IWeakReference *pMaskOverlay;
};

#endif //__MASKCOMPOSITE_H_
