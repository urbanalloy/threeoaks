/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// AnimatorOverlay.cpp : Implementation of CAnimatorOverlay
#include "stdafx.h"

#include <direct.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <comdef.h>

#include <GdiplusH.h>
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
	pWeakTarget(0),
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

/*
 * Called by SysStats when the Overlay should render itself
 */
STDMETHODIMP CAnimatorOverlay::Render(LONG _hdc)
{
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

	if (alphaEnd == alpha)
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

		// Copy private HDC into the passed HDC
		Graphics g((HDC)_hdc);
		g.SetInterpolationMode(InterpolationModeHighQuality);
		g.SetSmoothingMode(SmoothingModeAntiAlias);
		float delta = framecount <= 1 ? 0.0 : ((float)currentframe)/(framecount-1.0);
		ColorMatrix colorMatrix = {
						1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, ((float)alpha+(alphaEnd-alpha)*delta)/255.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

 		// Create an ImageAttributes object and set its color matrix.
		ImageAttributes imageAtt;
		imageAtt.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

		g.DrawImage(
		   &hdc.GetImage(), 
		   Rect(0, 0, width, height),	// Destination rectangle
		   0,						// Source rectangle X 
		   0,						// Source rectangle Y
		   width,		// Source rectangle width
		   height,		// Source rectangle height
		   UnitPixel,
		   &imageAtt);
	}

	return S_OK;
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
