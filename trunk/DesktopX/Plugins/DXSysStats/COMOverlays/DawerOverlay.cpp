// AnimatorOverlay.cpp : Implementation of CAnimatorOverlay
#include "stdafx.h"

#include <direct.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <comdef.h>

#include <Gdiplus.h>
using namespace Gdiplus;
#include "SysStatsUtils.h"
#include "COMOverlays.h"
#include "AnimatorOverlay.h"
#include "AnimatorOverlayDialog.h"
#include "CompositeOverlayImpl.cpp"

/////////////////////////////////////////////////////////////////////////////
// CAnimatorOverlay

STDMETHODIMP CAnimatorOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IAnimatorOverlay
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
CAnimatorOverlay::CAnimatorOverlay() :
	alpha(255), alphaEnd(255),
	rotationEnd(0), centerYEnd(0), centerXEnd(0), yEnd(0), xEnd(0),
	hScaleEnd(1.0), vScaleEnd(1.0),
	path(""),
	pWeakTarget(0),
	maskWidth(-1),
	maskHeight(-1),
	bmp(0),
	framecount(1),
	currentframe(0)
{
}

HRESULT CAnimatorOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CAnimatorOverlay::FinalRelease()
{
	pWeakTarget->Release();
}

/*
 * Return a string representation of the overlay’s type.
 */
STDMETHODIMP CAnimatorOverlay::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_AnimatorOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

/*
 * From SimpleOverlayImpl2<> - used by 'clone'.
 */
IOverlay *CAnimatorOverlay::createInstance()
{
	CComObject<CAnimatorOverlay> *pRet;
	CComObject<CAnimatorOverlay>::CreateInstance(&pRet);

	return pRet;
}

/*
 * This method is called by SysStats - it should copy all of the state of
 * this object into 'o'.
 */
STDMETHODIMP CAnimatorOverlay::clone(IOverlay * * pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// Clone all the superclass's state
	CompositeOverlayImpl<IAnimatorOverlay>::clone(pRet);

	CAnimatorOverlay *o = (CAnimatorOverlay*) *pRet;
	o->alpha = alpha;
	o->alphaEnd = alphaEnd;
	o->rotationEnd = rotationEnd;
	o->centerYEnd = centerYEnd;
	o->centerXEnd = centerXEnd;
	o->yEnd = yEnd;
	o->xEnd = xEnd;
	o->hScaleEnd = hScaleEnd;
	o->vScaleEnd = vScaleEnd;
	o->path = path;

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CAnimatorOverlay::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	AnimatorOverlayDialog *pDialog = new AnimatorOverlayDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::HitTest(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	if (!bmp)
		LoadMask();

	DWORD pixelColor = 0;

	if (bmp)
	{
		Color c;
		AtlTrace("Point before transform: %d, %d\n", X, Y);
		PointF points[1] = { PointF(X, Y) };
		REAL el[6];
		CumulativeEndTransform(el);
		Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);
		transform.Invert();
		transform.TransformPoints(points, 1);
		AtlTrace("Point after transform: %g, %g\n", points[0].X, points[0].Y);
		RectF bounds;
		Unit unit;
		bmp->GetBounds(&bounds, &unit);
		if (bounds.Contains(points[0]) && (bmp->GetPixel(points[0].X, points[0].Y, &c) == Ok))
		{
			pixelColor = c.GetValue();
			AtlTrace("Pixel color=0x%0.8x\n", *retVal);
		}
	}

	// If point is within mask then pass the call on to children
	if ((pixelColor & 0xFF000000) != 0)
	{
		CompositeOverlayImpl<IAnimatorOverlay>::HitTest(X, Y, retVal);
	}

	return S_OK;
}

/*
 * Called by SysStats when the Overlay should render itself
 */
STDMETHODIMP CAnimatorOverlay::Render(LONG _hdc)
{
	Bitmap *mask = LoadMask();

	// Get state of animation meter
	IMeter *iMeter = 0;
	framecount = 1;
	currentframe = 0;
	get_meter(&iMeter);
	if (iMeter)
	{
		iMeter->GetAsLong(L"framecount", &framecount);
		iMeter->GetAsLong(L"currentframe", &currentframe);
	}

	if (!mask)
		CompositeOverlayImpl<IAnimatorOverlay>::Render(_hdc);
	else
	{
		// Create a private HDC to draw into so that we can mask it.
		long width = 128;
		long height = 128;
		model->get_Width(&width);
		model->get_Height(&height);
		HDCImage hdc((HDC)_hdc, width, height);

		// Render the composite
		CompositeOverlayImpl<IAnimatorOverlay>::Render(hdc);

		// Mask it. Mask always lives at the end coordinates so we have to create yet another
		// private HDC to draw the transformed mask into!
		REAL el[6];
		CumulativeEndTransform(el);
		Matrix matrix(el[0], el[1], el[2], el[3], el[4], el[5]);

		HDCImage hdcMask((HDC)_hdc, width, height);

		Graphics gMask(hdcMask);
		gMask.SetInterpolationMode(InterpolationModeHighQuality);
		gMask.SetSmoothingMode(SmoothingModeAntiAlias);
		gMask.MultiplyTransform(&matrix);

		// Need this version of DrawImage otherwise the image is mysteriously scaled.
		gMask.DrawImage(
		   mask,
		   Rect(0, 0, maskWidth, maskHeight),	// Destination rectangle
		   0,						// Source rectangle X
		   0,						// Source rectangle Y
		   maskWidth,		// Source rectangle width
		   maskHeight,		// Source rectangle height
		   UnitPixel);


		::BitBlt(hdc, 0, 0, width, height, hdcMask, 0, 0, SRCAND);

		// Copy private HDC into the passed HDC
		Graphics g((HDC)_hdc);
		g.SetInterpolationMode(InterpolationModeHighQuality);
		g.SetSmoothingMode(SmoothingModeAntiAlias);
		g.DrawImage(
		   &hdc.GetImage(),
		   Rect(0, 0, width, height),	// Destination rectangle
		   0,						// Source rectangle X
		   0,						// Source rectangle Y
		   width,		// Source rectangle width
		   height,		// Source rectangle height
		   UnitPixel);
	}

	return S_OK;
}

const Matrix& CAnimatorOverlay::getEndTransform()
{
	matrixEnd.Reset();
	matrixEnd.Translate((float)(xEnd), (float)(yEnd));
	matrixEnd.Rotate((float)rotationEnd);
	matrixEnd.Scale((float)hScaleEnd, (float)vScaleEnd);
	matrixEnd.Translate((float)(-centerXEnd), (float)(-centerYEnd));

	return matrixEnd;
}

const Matrix& CAnimatorOverlay::getCurTransform()
{
	matrixCur.Reset();

	float delta = framecount <= 1 ? 0.0 : ((float)currentframe)/(framecount-1.0);
	matrixCur.Translate((float)(x + (xEnd-x)*delta), (float)(y + (yEnd-y)*delta));
	matrixCur.Rotate((float)rotation + (rotationEnd-rotation)*delta);
	float hs = (float)hScale + (hScaleEnd-hScale)*delta;
	if (hs == 0)
		hs = 1e-10;
	float vs = (float)vScale + (vScaleEnd-vScale)*delta;
	if (vs == 0)
		vs = 1e-10;

	matrixCur.Scale(hs, vs);
	matrixCur.Translate(-(float)(centerX + (centerXEnd-centerX)*delta), -(float)(centerY + (centerYEnd-centerY)*delta));

	return matrixCur;
}

void CAnimatorOverlay::CumulativeEndTransform(/*[out, retval]*/ float *elements)
{
	ISimpleOverlay2 *parent;
	get_Parent(&parent);
	Matrix m;
	if (parent)
	{
		float el[6];
		parent->CumulativeTransform(el);
		Matrix cm(el[0], el[1], el[2], el[3], el[4], el[5]);
		m.Multiply(&cm);
	}

	m.Multiply(&getEndTransform());
	m.GetElements(elements);
}

STDMETHODIMP CAnimatorOverlay::CumulativeTransform(/*[out, retval]*/ float *elements)
{
	if (elements == NULL)
		return E_POINTER;

	ISimpleOverlay2 *parent;
	get_Parent(&parent);

	Matrix m;
	if (parent)
	{
		float el[6];
		parent->CumulativeTransform(el);
		Matrix cm(el[0], el[1], el[2], el[3], el[4], el[5]);
		m.Multiply(&cm);
	}

	m.Multiply(&getCurTransform());
	m.GetElements(elements);
	return S_OK;
}

Bitmap *CAnimatorOverlay::LoadMask()
{
	if (bmp)
		delete bmp;

	bmp = SysStatsUtils::LoadImage(path);

	if (bmp)
	{
		maskWidth = bmp->GetWidth();
		maskHeight = bmp->GetHeight();
	}

	return bmp;
}

STDMETHODIMP CAnimatorOverlay::get_AlphaEnd(BYTE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = alphaEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_AlphaEnd(BYTE newVal)
{
	alphaEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_Path(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IClientModel *_model = 0;
	get_Model(&_model);
	IAppConfig *appConfig = 0;
	_model->get_AppConfig(&appConfig);

	appConfig->NormalizePath(path, pVal);

	appConfig->Release();
	_model->Release();

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_Path(BSTR newVal)
{
	IAppConfig *appConfig = 0;
	model->get_AppConfig(&appConfig);

	BSTR tmp;
	appConfig->CanonicalPath(newVal, &tmp);
	path = tmp;
	::SysFreeString(tmp);

	appConfig->Release();

	maskHeight = -1;
	maskWidth = -1;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_XEnd(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = xEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_XEnd(long newVal)
{
	xEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_YEnd(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = yEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_YEnd(long newVal)
{
	yEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_CenterXEnd(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = centerXEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_CenterXEnd(long newVal)
{
	centerXEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_CenterYEnd(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = centerYEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_CenterYEnd(long newVal)
{
	centerYEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_RotationEnd(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = rotationEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_RotationEnd(long newVal)
{
	rotationEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_HScaleEnd(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = hScaleEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_HScaleEnd(double newVal)
{
	hScaleEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_VScaleEnd(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = vScaleEnd;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_VScaleEnd(double newVal)
{
	vScaleEnd = newVal;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::get_Alpha(BYTE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = alpha;

	return S_OK;
}

STDMETHODIMP CAnimatorOverlay::put_Alpha(BYTE newVal)
{
	alpha = newVal;

	return S_OK;
}
