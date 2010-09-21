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

// ScrollerOverlay.cpp : Implementation of CScrollerOverlay
#include "stdafx.h"
#include "MeterImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#pragma comment(lib, "SysStatsCOM.lib")
#include <Gdiplus.h>
using namespace Gdiplus;
#include "SysStatsUtils.h"
#include "COMOverlays.h"
#include "CompositeOverlayImpl.cpp"
#include "ScrollerOverlay.h"
#include "ScrollerOverlayDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CScrollerOverlay

STDMETHODIMP CScrollerOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IScrollerOverlay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CScrollerOverlay::CScrollerOverlay() :
	alpha(255),
	dx(0),
	dy(0),
	duplicateLastFrame(false),
	oldImage(NULL),
	pWeakTarget(0),
	framecount(1),
	currentframe(0),
	smoothframecount(1),
	smoothcurrentframe(0),
	smoothScrollMeter(0),
	smoothMeterPlayState(0),
	delta(1)
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakReference", &clsid);
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, /* IID_IWeakReference */ __uuidof(IWeakReference), (void**)&smoothScrollMeter);
}

CScrollerOverlay::~CScrollerOverlay()
{
	if (oldImage)
		delete oldImage;

	if (smoothScrollMeter)
		smoothScrollMeter->Release();
}

HRESULT CScrollerOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CScrollerOverlay::FinalRelease()
{
	pWeakTarget->Release();
}

/*
 * Return a string representation of the overlay’s type.
 */
STDMETHODIMP CScrollerOverlay::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ScrollerOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

/*
 * From SimpleOverlayImpl2<> - used by 'clone'.
 */
IOverlay *CScrollerOverlay::createInstance()
{
	CComObject<CScrollerOverlay> *pRet;
	CComObject<CScrollerOverlay>::CreateInstance(&pRet);

	return pRet;
}

/*
 * This method is called by SysStats - it should copy all of the state of
 * this object into 'o'.
 */
STDMETHODIMP CScrollerOverlay::clone(IOverlay * * pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// Clone all the superclass's state
	CompositeOverlayImpl<IScrollerOverlay>::clone(pRet);

	CScrollerOverlay *o = (CScrollerOverlay*) *pRet;
	o->alpha = alpha;
	o->dx = dx;
	o->dy = dy;
	o->duplicateLastFrame = duplicateLastFrame;
	IUnknown *iUnknown = 0;
	smoothScrollMeter->DeReference(&iUnknown);
	o->put_SmoothScrollMeter((IMeter*)iUnknown);

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CScrollerOverlay::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	ScrollerOverlayDialog *pDialog = new ScrollerOverlayDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

void CScrollerOverlay::controlMeter(IMeter *iMeter, BSTR selector, BSTR value)
{
	LPOLESTR name = L"PutValue";
	DISPID id;
	HRESULT hr = iMeter->GetIDsOfNames(IID_NULL,
                           &name,
                           1,
                           LOCALE_USER_DEFAULT,
                           &id);

	if (SUCCEEDED(hr))
	{
		CComVariant varResult;
		CComVariant* pvars = new CComVariant[2];

		VariantClear(&varResult);
		pvars[0] = (BSTR)value;
		pvars[1] = (BSTR)selector;
		DISPPARAMS disp = { pvars, NULL, 2, 0 };
		hr = iMeter->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
		delete[] pvars;

		if (SUCCEEDED(hr))
		{
			AtlTrace("ScrollerOverlay: Set Value via PutValue\n");
		}
	}
}

/*
 * Called by SysStats when the Overlay should render itself
 */
STDMETHODIMP CScrollerOverlay::Render(LONG _hdc)
{
	// Get state of animation meter
	IMeter *iMeter = 0;

	long oldcurrentframe = currentframe;
	delta = 1;
	get_meter(&iMeter);
	if (iMeter)
	{
		iMeter->GetAsLong(L"framecount", &framecount);
		iMeter->GetAsLong(L"currentframe", &currentframe);
		iMeter->GetAsLong(L"framedelta", &delta);
	}
	else
	{
		framecount = 1;
		currentframe = 0;
	}

	// If we have a smooth scroll meter, kick it off.
	smoothframecount = 1;
	smoothcurrentframe = 0;
	smoothMeterPlayState = 0;
	get_SmoothScrollMeter(&iMeter);
	if (iMeter)
	{
		if (currentframe != oldcurrentframe)
		{
			if (delta == -1)
			{
				controlMeter(iMeter, L"framedelta", L"-1");
				controlMeter(iMeter, L"position", L"2");	// == END
			}
			else
			{
				controlMeter(iMeter, L"framedelta", L"1");
				controlMeter(iMeter, L"position", L"1");	// == START
			}
			controlMeter(iMeter, L"playstate", L"1");
		}
		iMeter->GetAsLong(L"framecount", &smoothframecount);
		iMeter->GetAsLong(L"currentframe", &smoothcurrentframe);
		iMeter->GetAsLong(L"playstate", &smoothMeterPlayState);
		if (smoothframecount == 0)
			smoothframecount = 1;
	}

	if (!duplicateLastFrame)
		CompositeOverlayImpl<IScrollerOverlay>::Render(_hdc);
	else
	{
		// Create a private HDC to draw into so that we can mask it.
		long width = 128;
		long height = 128;
		model->get_Width(&width);
		model->get_Height(&height);
		HDCImage hdc((HDC)_hdc, width, height);

		// Render the composite
		CompositeOverlayImpl<IScrollerOverlay>::Render(hdc);

		// Nerd knob for smooth scrolling that loops.
		if (smoothMeterPlayState == 1)
		{
			if (duplicateLastFrame && oldImage)
			{
				Rect clip;
				clip.X = x - centerX;
				clip.Y = y - centerY;
				int drawX = 0, drawY = 0;
				if (delta >= 0)	// The main scroller is scrolling 'forwards'
				{
					clip.Width = (smoothframecount-smoothcurrentframe-1)*dx/smoothframecount;
					clip.Height = (smoothframecount-smoothcurrentframe-1)*dy/smoothframecount;

					drawX = clip.Width - dx;
					drawY = clip.Height - dy;

					if (clip.Width < 0)
					{
						clip.Width = -clip.Width;
						clip.X += drawX;
					}

					if (clip.Height < 0)
					{
						clip.Height = -clip.Height;
						clip.Y += drawY;
					}
				}
				else
				{
					clip.Width = smoothcurrentframe*dx/smoothframecount;
					clip.Height = smoothcurrentframe*dy/smoothframecount;

					drawX = dx - clip.Width;
					drawY = dy - clip.Height;

					if (clip.Width < 0)
						clip.Width = -clip.Width;
					else
						clip.X += drawX;

					if (clip.Height < 0)
						clip.Height = -clip.Height;
					else
						clip.Y += drawY;
				}

				if (clip.Width != 0 || clip.Height != 0)
				{
					if (clip.Width == 0)
						clip.Width = width;

					if (clip.Height == 0)
						clip.Height = height;

					Graphics gss(hdc);
					gss.SetInterpolationMode(InterpolationModeHighQuality);
					gss.SetSmoothingMode(SmoothingModeAntiAlias);
					gss.SetPixelOffsetMode(PixelOffsetModeHalf);
					gss.SetClip(clip);
					Color nothing(0, 0, 0, 0);
					gss.Clear(nothing);	// clear clipping rectangle
					gss.DrawImage(
					   oldImage,
					   Rect(drawX, drawY, width, height),			// Destination rectangle
					   0,				// Source rectangle X
					   0,				// Source rectangle Y
					   width,			// Source rectangle width
					   height,			// Source rectangle height
					   UnitPixel);
				}
			}
		}
		else
		{
			if (oldImage)
				delete oldImage;

			oldImage = (Bitmap*)hdc.GetImage().Clone();
		}

		// Copy private HDC into the passed HDC
		Graphics g((HDC)_hdc);
		g.SetInterpolationMode(InterpolationModeHighQuality);
		g.SetSmoothingMode(SmoothingModeAntiAlias);
		g.SetPixelOffsetMode(PixelOffsetModeHalf);
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

void CScrollerOverlay::getOffset(int *x, int *y) const
{
	int xoffset = currentframe * dx;
	int yoffset = currentframe * dy;

	if (smoothMeterPlayState == 1)	// Playing
	{
		if (delta >= 0)	// The main scroller is scrolling 'forwards'
		{
			xoffset -= (smoothframecount-smoothcurrentframe-1)*dx/smoothframecount;
			yoffset -= (smoothframecount-smoothcurrentframe-1)*dy/smoothframecount;
		}
		else
		{
			xoffset += smoothcurrentframe*dx/smoothframecount;
			yoffset += smoothcurrentframe*dy/smoothframecount;
		}
	}

	*x = xoffset;
	*y = yoffset;
}

const Matrix& CScrollerOverlay::getCurTransform()
{
	matrixCur.Reset();

	int xoffset = 0;
	int yoffset = 0;

	getOffset(&xoffset, &yoffset);

	matrixCur.Translate((float)(x), (float)(y));
	matrixCur.Rotate((float)rotation);
	matrixCur.Scale((float)hScale, (float)vScale);
	matrixCur.Translate((float)(-centerX-xoffset), (float)(-centerY-yoffset));

	return matrixCur;
}

STDMETHODIMP CScrollerOverlay::CumulativeTransform(/*[out, retval]*/ float *elements)
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

STDMETHODIMP CScrollerOverlay::get_Alpha(BYTE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = alpha;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::put_Alpha(BYTE newVal)
{
	alpha = newVal;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::get_dx(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = dx;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::put_dx(long newVal)
{
	dx = newVal;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::get_dy(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = dy;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::put_dy(long newVal)
{
	dy = newVal;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::get_SmoothScrollMeterIndex(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IMeterHome *pMeters;
	model->get_Meters(&pMeters);
	short index = -1;
	IUnknown *iUnknown = 0;
	smoothScrollMeter->DeReference(&iUnknown);
	pMeters->GetIndex((IMeter*)iUnknown, &index);
	*pVal = index;
	pMeters->Release();

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::put_SmoothScrollMeterIndex(long newVal)
{
	IMeterHome *pMeters;
	model->get_Meters(&pMeters);
	IMeter *pMeter;
	pMeters->Item(CComVariant(newVal), &pMeter);

	put_SmoothScrollMeter(pMeter);
	if (pMeter)
		pMeter->Release();

	pMeters->Release();

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::get_SmoothScrollMeter(IMeter **pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IUnknown *iUnknown = 0;
	smoothScrollMeter->DeReference(&iUnknown);
	*pVal = (IMeter*)iUnknown;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::put_SmoothScrollMeter(IMeter *newVal)
{
	smoothScrollMeter->AssignReference(newVal);

	return S_OK;
}


STDMETHODIMP CScrollerOverlay::get_DuplicateLastFrame(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = duplicateLastFrame;

	return S_OK;
}

STDMETHODIMP CScrollerOverlay::put_DuplicateLastFrame(VARIANT_BOOL newVal)
{
	duplicateLastFrame = newVal;

	return S_OK;
}
