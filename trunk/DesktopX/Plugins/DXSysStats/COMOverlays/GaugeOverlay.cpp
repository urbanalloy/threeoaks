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

// GaugeOverlay.cpp : Implementation of CGaugeOverlay
#include "stdafx.h"
#include "COMOverlays.h"
#include "MeterOverlay.h"
#include "GaugeOverlay.h"
#include "GaugeOverlayDialog.h"
#include "SimpleOverlayImpl2.cpp"
#include "SysStatsUtils.h"

/////////////////////////////////////////////////////////////////////////////
// CGaugeOverlay

STDMETHODIMP CGaugeOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IGaugeOverlay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CGaugeOverlay::CGaugeOverlay() : span(360), pWeakTarget(0)
{
	CComObject<CMeterOverlay> *met;
	CComObject<CMeterOverlay>::CreateInstance(&met);
	met->AddRef();
	proxy = met;

}

CGaugeOverlay::~CGaugeOverlay()
{
	if (proxy)
		proxy->Release();
}

HRESULT CGaugeOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CGaugeOverlay::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CGaugeOverlay::get_Span(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = span;

	return S_OK;
}

STDMETHODIMP CGaugeOverlay::put_Span(long newVal)
{
	span = newVal;

	return S_OK;
}

STDMETHODIMP CGaugeOverlay::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_GaugeOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);
		
	return S_OK;
}

// From SimpleOverlayImpl<> - used by 'clone'.
IOverlay *CGaugeOverlay::createInstance()
{
	CComObject<CGaugeOverlay> *pRet;
	CComObject<CGaugeOverlay>::CreateInstance(&pRet);

	return pRet;
}

STDMETHODIMP CGaugeOverlay::clone(IOverlay **pRet)
{
	if (pRet == NULL)
		return E_POINTER;
		
	SimpleOverlayImpl2<IGaugeOverlay>::clone(pRet);
	CGaugeOverlay *o = (CGaugeOverlay*) *pRet;

	IOverlay *over = o->proxy;
	proxy->clone(&over);
	o->proxy = (IMeterOverlay*)over;

	o->put_Span(span);

	return S_OK;
}

STDMETHODIMP CGaugeOverlay::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	GaugeOverlayDialog *pDialog = new GaugeOverlayDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

STDMETHODIMP CGaugeOverlay::HitTest(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal)
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
		long max, min, length, thickness;
		get_Max(&max);
		get_Min(&min);
		get_Length(&length);
		get_Thickness(&thickness);
		long size = max - min;
		long offset = val - min;

		REAL el[6];
		CumulativeTransform(size, offset, el);
		Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);
		transform.Invert();
		transform.TransformPoints(points, 1);

		RectF bounds(0.0, 0.0, (float)length, (float)thickness);
		*retVal = bounds.Contains(points[0]);
	}

	return S_OK;
}

STDMETHODIMP CGaugeOverlay::Render(LONG hdc)
{
	long val = 0;

	IMeter *iMeter;
	get_meter(&iMeter);
	if (iMeter && SUCCEEDED(iMeter->GetAsLong(selector, &val)))
	{
		GraphicsPath path(FillModeAlternate);

		long max, min, length, thickness;
		get_Max(&max);
		get_Min(&min);
		get_Length(&length);
		get_Thickness(&thickness);
		long size = max - min;
		long offset = val - min;

		path.AddRectangle(RectF(0.0, 0.0, (float)length, (float)thickness));

		REAL el[6];
		CumulativeTransform(size, offset, el);
		Matrix matrix(el[0], el[1], el[2], el[3], el[4], el[5]);

		path.Transform(&matrix);
   
		Graphics graphics((HDC)hdc);
		graphics.SetInterpolationMode(InterpolationModeHighQuality);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

		long rgb;
		BYTE alpha;
		get_Color(&rgb);
		get_Alpha(&alpha);
		Color color;
		color.SetFromCOLORREF(rgb);
		rgb = color.GetValue() & 0x00FFFFFF;
		color.SetValue(rgb | ((long)alpha << 24));
		SolidBrush  fg(color);
 		graphics.FillPath(&fg, &path);
	}

	return S_OK;
}

STDMETHODIMP CGaugeOverlay::CumulativeTransform(/*[out, retval]*/ long size, long offset, REAL *elements)
{
	if (elements == NULL)
		return E_POINTER;

	Matrix m;
	ISimpleOverlay2 *parent = NULL;
	get_Parent(&parent);
	if (parent)
	{
		REAL el[6];
		parent->CumulativeTransform(el);
		Matrix cm(el[0], el[1], el[2], el[3], el[4], el[5]);
		m.Multiply(&cm);
	}

	PointF center((float)centerX, (float)centerY);
	m.Translate((float)(x-centerX), (float)(y-centerY));
	m.RotateAt((float)span * offset / size + rotation, center);
	m.Scale((float)hScale, (float)vScale);
	m.GetElements(elements);
	return S_OK;
}
