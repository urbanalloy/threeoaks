// MaskComposite.cpp : Implementation of CMaskComposite
#include "stdafx.h"
#include "MeterImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#pragma comment(lib, "SysStatsCOM.lib")
#include <GdiplusH.h>
#include "SysStatsUtils.h"
#include "COMOverlays.h"
#include "CompositeOverlayImpl.cpp"
#include "MaskComposite.h"
#include "MaskCompositeDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMaskComposite

STDMETHODIMP CMaskComposite::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IMaskComposite
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CMaskComposite::CMaskComposite() : pWeakTarget(0)
{
}

HRESULT CMaskComposite::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CMaskComposite::FinalRelease()
{
	pWeakTarget->Release();
}

/*
 * Return a string representation of the overlay’s type.
 */
STDMETHODIMP CMaskComposite::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MaskComposite, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

/*
 * From SimpleOverlayImpl2<> - used by 'clone'.
 */
IOverlay *CMaskComposite::createInstance()
{
	CComObject<CMaskComposite> *pRet;
	CComObject<CMaskComposite>::CreateInstance(&pRet);

	return pRet;
}

/*
 * This method is called by SysStats - it should copy all of the state of
 * this object into 'o'.
 */
STDMETHODIMP CMaskComposite::clone(IOverlay * * pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// Clone all the superclass's state
	SimpleOverlayImpl2<IMaskComposite>::clone(pRet);

	CMaskComposite *o = (CMaskComposite*) *pRet;

	// Add statements to copy our state into 'o' here.

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CMaskComposite::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	MaskCompositeDialog *pDialog = new MaskCompositeDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

/*
 * Called by SysStats when the Overlay should render itself
 */
STDMETHODIMP CMaskComposite::Render(LONG hdc)
{
	// Use a GdiPlus Graphics object for the rendering.
	Graphics graphics((HDC)hdc);

	// Apply standard transformations
	Matrix matrix;
	PointF center((float)centerX, (float)centerY);
	matrix.Translate((float)(x-centerX), (float)(y-centerY));
	matrix.RotateAt((float)rotation, center);
	matrix.Scale((float)hScale, (float)vScale);

	graphics.MultiplyTransform(&matrix);

#ifdef use_string
	// Example code to get the value of a meter as a string.
	// The actual type you want will depend on how your overlay
	// intends to render the meter, you might want an integer or
	// a double or something else altogether. If so you would
	// call one of the other IMeter::GetValue() methods.
	BSTR text;

	// If there is no meter, use a static string
	if (!meter)
	{
		text = _bstr_t("An instance of IMaskComposite");
	}
	else
	{
		// Otherwise, assume the meter value is numeric and we want
		// it as a string. The actual meter value could be of any type
		// so if we really wanted to display any meter value as a string
		// we would probably allow the user to enter this format
		// string themselves.
		_bstr_t tmp("");
		BSTR tCopy = tmp.copy();
		text = tCopy;
		meter->GetAsString("Meter value = %0.f", selector, &text);
		if (text != tCopy)
			::SysFreeString(tCopy);
	}
#endif // use_string
#ifdef use_long
	// Example code to get the value of a meter as a long.
	long value = 0;

	if (meter)
		meter->GetAsLong(selector, &value);
#endif

	// Add code here to render the overlay using the meter value
	// you obtained above.

	return S_OK;
}

STDMETHODIMP CMaskComposite::get_MaskOverlayIndex(long *pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CMaskComposite::put_MaskOverlayIndex(long newVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CMaskComposite::get_MaskOverlay(IOverlay **pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CMaskComposite::put_MaskOverlay(IOverlay *newVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}
