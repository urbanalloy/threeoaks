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

// MeterOverlay.cpp : Implementation of CMeterOverlay
#include "stdafx.h"

#include "COMOverlays.h"
#include "MeterOverlay.h"
#include "MeterOverlayDialog.h"
#include "SimpleOverlayImpl2.cpp"

#undef min
#undef max

/////////////////////////////////////////////////////////////////////////////
// CMeterOverlay

STDMETHODIMP CMeterOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMeterOverlay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CMeterOverlay::CMeterOverlay() : min(0), max(0), length(0), thickness(0), color(0xFF000000), pWeakTarget(0)
{
}

HRESULT CMeterOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CMeterOverlay::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CMeterOverlay::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MeterOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);
		
	return S_OK;
}

// From SimpleOverlayImpl<> - used by 'clone'.
IOverlay *CMeterOverlay::createInstance()
{
	CComObject<CMeterOverlay> *pRet;
	CComObject<CMeterOverlay>::CreateInstance(&pRet);

	return pRet;
}

STDMETHODIMP CMeterOverlay::clone(IOverlay **pRet)
{
	if (pRet == NULL)
		return E_POINTER;
		
	SimpleOverlayImpl2<IMeterOverlay>::clone(pRet);

	CMeterOverlay *o = (CMeterOverlay*) *pRet;

	o->put_Min(min);
	o->put_Max(max);
	o->put_Length(length);
	o->put_Thickness(thickness);
	long fColor;
	get_Color(&fColor);
	o->put_Color(fColor);
	BYTE bAlpha;
	get_Alpha(&bAlpha);
	o->put_Alpha(bAlpha);

	return S_OK;
}

STDMETHODIMP CMeterOverlay::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	MeterOverlayDialog *pDialog = new MeterOverlayDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::HitTest(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	*retVal = false;

	long val = 0;

	IMeter *iMeter;
	get_meter(&iMeter);
	if (iMeter && SUCCEEDED(iMeter->GetAsLong(selector, &val)))
	{
		PointF points[1] = { PointF(X, Y) };
		REAL el[6];
		CumulativeTransform(el);
		Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);
		transform.Invert();
		transform.TransformPoints(points, 1);

		long size = max - min;
		long offset = (val > max ? max : val) - min;

		if (size && (offset > 0))
		{
			RectF bounds(0.0, 0.0, (float)length * offset / size, (float)thickness);
			*retVal = bounds.Contains(points[0]);
		}
	}

	return S_OK;
}

STDMETHODIMP CMeterOverlay::Render(LONG hdc)
{
	long val = 0;

	IMeter *iMeter;
	get_meter(&iMeter);
	if (iMeter && SUCCEEDED(iMeter->GetAsLong(selector, &val)))
	{
		long size = max - min;
		long offset = (val > max ? max : val) - min;

		GraphicsPath path(FillModeAlternate);

		if (size && (offset > 0))
			path.AddRectangle(RectF(0.0, 0.0, (float)length * offset / size, (float)thickness));

		REAL el[6];
		CumulativeTransform(el);
		Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);
		path.Transform(&transform);
   
		Graphics graphics((HDC)hdc);
		graphics.SetInterpolationMode(InterpolationModeHighQuality);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
		SolidBrush  fg(color);
 		graphics.FillPath(&fg, &path);
	}

	return S_OK;
}

STDMETHODIMP CMeterOverlay::get_Max(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = max;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::put_Max(long newVal)
{
	max = newVal;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::get_Min(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = min;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::put_Min(long newVal)
{
	min = newVal;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::get_Length(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = length;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::put_Length(long newVal)
{
	length = newVal;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::get_Thickness(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = thickness;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::put_Thickness(long newVal)
{
	thickness = newVal;

	return S_OK;
}

STDMETHODIMP CMeterOverlay::get_Color(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = (long)color.ToCOLORREF();

	return S_OK;
}

STDMETHODIMP CMeterOverlay::put_Color(long newVal)
{
	BYTE alpha = color.GetAlpha();
	color.SetFromCOLORREF((COLORREF)newVal);
	ARGB val = color.GetValue() & 0x00FFFFFF;
	val |= alpha << 24;
	color.SetValue(val);

	return S_OK;
}

STDMETHODIMP CMeterOverlay::get_Alpha(BYTE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = color.GetAlpha();

	return S_OK;
}

STDMETHODIMP CMeterOverlay::put_Alpha(BYTE newVal)
{
	ARGB val = color.GetValue() & 0x00FFFFFF;
	val |= newVal << 24;
	color.SetValue(val);

	return S_OK;
}
