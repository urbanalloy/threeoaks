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

// ImageOverlay.cpp : Implementation of CImageOverlay
#include "stdafx.h"

#include <direct.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <comdef.h>
#include <float.h>
#include <math.h>
#include "COMOverlays.h"
#include "ImageOverlay.h"
#include "ImageOverlayDialog.h"
#include "SimpleOverlayImpl2.cpp"
#include "QColorMatrix.h"

// Hack for old config files.
static _bstr_t BACKGROUND104 = "Background_1.0.4";

/////////////////////////////////////////////////////////////////////////////
// CImageOverlay

STDMETHODIMP CImageOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IImageOverlay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CImageOverlay::CImageOverlay() :
	alpha(255),
	path(""),
	imageHeight(-1),
	imageWidth(-1),
	bmp(0, *this),
	pWeakTarget(0),
	animationMeter(0),
	oldText(::SysAllocString(L"")),
	hueShift(0),
	saturation(1.0),
	keepImageOpen(true),
	limitWidth(0),
	limitHeight(0),
	alignment(0)
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakReference", &clsid);
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, /* IID_IWeakReference */ __uuidof(IWeakReference), (void**)&animationMeter);
}

CImageOverlay::~CImageOverlay()
{
	if (animationMeter)
		animationMeter->Release();

	::SysFreeString(oldText);
}

HRESULT CImageOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CImageOverlay::FinalRelease()
{
	pWeakTarget->Release();
}

void CImageOverlay::loaded() {
	if (bmp)
	{
		imageWidth = bmp->GetWidth();
		imageHeight = bmp->GetHeight();
	}

	model->put_dirty(1);
}

STDMETHODIMP CImageOverlay::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ImageOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);
		
	return S_OK;
}

// From SimpleOverlayImpl<> - used by 'clone'.
IOverlay *CImageOverlay::createInstance()
{
	CComObject<CImageOverlay> *pRet;
	CComObject<CImageOverlay>::CreateInstance(&pRet);

	return pRet;
}

STDMETHODIMP CImageOverlay::clone(IOverlay * * pRet)
{
	if (pRet == NULL)
		return E_POINTER;
		
	SimpleOverlayImpl2<IImageOverlay>::clone(pRet);

	CImageOverlay *o = (CImageOverlay*) *pRet;

	o->put_Alpha(alpha);
	o->put_Path(path);
	o->put_HueShift(hueShift);
	o->put_Saturation(saturation);
	o->put_KeepImageOpen(keepImageOpen);
	o->put_LimitWidth(limitWidth);
	o->put_LimitHeight(limitHeight);
	o->put_Alignment(alignment);

	return S_OK;
}

STDMETHODIMP CImageOverlay::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	ImageOverlayDialog *pDialog = new ImageOverlayDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

STDMETHODIMP CImageOverlay::HitTest(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	DWORD pixelColor = 0;
	HRESULT hr = GetPixelValue(X, Y, &pixelColor);

	*retVal = (pixelColor & 0xFF000000) != 0;

	return hr;
}

STDMETHODIMP CImageOverlay::Render(LONG hdc)
{
	LoadImage();

	if (bmp)
	{
		Graphics graphics((HDC)hdc);

		REAL el[6];
		CumulativeTransform(el);
		Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);

		if (name == BACKGROUND104)
			transform.Scale(128.0f / imageWidth, 128.0f / imageHeight);

		graphics.SetInterpolationMode(/*InterpolationModeHighQuality*/InterpolationModeHighQualityBicubic);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetPixelOffsetMode(/*PixelOffsetModeHalf*/PixelOffsetModeHighQuality);

		graphics.MultiplyTransform(&transform);

		QColorMatrix colorMatrix;

		colorMatrix.SetAlpha(((float)alpha)/255.0f);

		if (saturation != 1.0)
			colorMatrix.SetSaturation(saturation);

		if (hueShift != 0)
			colorMatrix.RotateHue(hueShift);

 		// Create an ImageAttributes object and set its color matrix.
		ImageAttributes imageAtt;
		imageAtt.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

		IMeter *iAnimMeter = 0;
		long framecount = 1;
		long currentframe = 0;
		get_AnimationMeter(&iAnimMeter);
		if (iAnimMeter)
		{
			iAnimMeter->GetAsLong(L"framecount", &framecount);
			iAnimMeter->GetAsLong(L"currentframe", &currentframe);
		}
		REAL frameWidth = imageWidth / framecount;
		REAL frameX = currentframe * frameWidth;

		graphics.DrawImage(
		   bmp, 
		   RectF(0.0, 0.0, frameWidth, imageHeight),	// Destination rectangle
		   frameX,					// Source rectangle X 
		   0.0,						// Source rectangle Y
		   frameWidth,		// Source rectangle width
		   (REAL)imageHeight,		// Source rectangle height
		   UnitPixel, 
		   &imageAtt);

		if (!keepImageOpen)
		{
			bmp = 0;
		}
	}
	else
	{
	}

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_Alpha(BYTE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = alpha;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_Alpha(BYTE newVal)
{
	alpha = newVal;

	return S_OK;
}

// Get as a relative path
STDMETHODIMP CImageOverlay::get_Path(BSTR *pVal)
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

// Store as an absolute path
STDMETHODIMP CImageOverlay::put_Path(BSTR newVal)
{
	UnloadImage();

	IAppConfig *appConfig = 0;
	model->get_AppConfig(&appConfig);

	BSTR tmp;
	appConfig->CanonicalPath(newVal, &tmp);
	path = tmp;
	::SysFreeString(tmp);

	appConfig->Release();

	return S_OK;
}

void CImageOverlay::UnloadImage()
{
//	Does this work when called from another thread?
	if (bmp)
	{
		bmp = 0;
		imageHeight = -1;
		imageWidth = -1;
		::SysFreeString(oldText);
		oldText = ::SysAllocString(L"");
	}
}

void CImageOverlay::LoadImage()
{
	BSTR text;

	IMeter *iMeter;
	get_meter(&iMeter);
	if (!iMeter)
	{
		if (bmp)
			return;

		text = path.copy();
	}
	else
	{
		// Need normalized path not canonical path:
		BSTR p;
		get_Path(&p);	//p = path.copy()
		BSTR tCopy = ::SysAllocString(L""); // tCopy = ""
		text = tCopy; // text = tCopy
		iMeter->GetAsString(p, selector, &text); // text = copy of something else
		::SysFreeString(p); // don't need p any more

		// This is an identity test - BSTRs are pointers:
		if (text != tCopy)
			::SysFreeString(tCopy); // tcopy is no more

		// Now we need to canonicalize the path
		IAppConfig *appConfig = 0;
		model->get_AppConfig(&appConfig);

		appConfig->CanonicalPath(text, &p); // reuse p
		::SysFreeString(text); // text is no more
		text = p; // set text to p

		appConfig->Release();

		if ((::wcscmp(text, oldText) == 0) && (bmp != NULL))
		{
			AtlTrace("Image is the same\n");
			::SysFreeString(text);
			return;
		}

		::SysFreeString(oldText);
		oldText = ::SysAllocString(text);
	}

	bmp.LoadImage(text);
	
	if (bmp)
	{
		imageWidth = bmp->GetWidth();
		imageHeight = bmp->GetHeight();
	}

	::SysFreeString(text);
}

STDMETHODIMP CImageOverlay::CumulativeTransform(/*[out, retval]*/ float *elements)
{
	if (elements == NULL)
		return E_POINTER;

	ISimpleOverlay2 *parent = NULL;
	get_Parent(&parent);

	Matrix m;
	if (parent)
	{
		float el[6];
		parent->CumulativeTransform(el);
		Matrix cm(el[0], el[1], el[2], el[3], el[4], el[5]);
		m.Multiply(&cm);
	}

	Matrix matrix;
	matrix.Translate((float)(x), (float)(y));
	matrix.Rotate((float)rotation);
	if (limitWidth == 0)
		matrix.Scale((float)hScale, (float)vScale);
	else if (imageWidth > 0 && imageHeight > 0)
	{
		float h = 1.0;
		float v = 1.0;
		if (imageWidth > limitWidth)
			h = (float)limitWidth/(float)imageWidth;

		if (imageHeight > limitHeight)
			v = (float)limitHeight/(float)imageHeight;

		if (fabs(h) < fabs(v)) {
			v = h;
			v =_copysign(v, limitHeight);
		}

		if (fabs(v) < fabs(h)) {
			h = v;
			h = _copysign(h, limitWidth);
		}

		float xOffset = 0.0;
		float yOffset = 0.0;

		// Horizontal alignment
		switch (alignment & 3)
		{
		case 2:	// Right
			xOffset = (float)limitWidth - (imageWidth * h);
			break;
		case 1:	// Center
			xOffset = ((float)limitWidth - (imageWidth * h))/2.0f;
			break;
		}

		// Vertical alignment
		switch ((alignment & 12) >> 2)
		{
		case 2:	// Bottom
			yOffset = (float)limitHeight - (imageHeight * v);
			break;
		case 1:	// Center
			yOffset = ((float)limitHeight - (imageHeight * v))/2.0f;
			break;
		}

		matrix.Translate(xOffset, yOffset);
		matrix.Scale(h, v);
	}
	matrix.Translate((float)(-centerX), (float)(-centerY));

	m.Multiply(&matrix);
	m.GetElements(elements);
	return S_OK;
}

STDMETHODIMP CImageOverlay::get_ImageHeight(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	if (imageHeight == -1)
	{
		LoadImage();
	}

	*pVal = imageHeight;

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_ImageWidth(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	if (imageWidth == -1)
	{
		LoadImage();
	}


	*pVal = imageWidth;

	return S_OK;
}

STDMETHODIMP CImageOverlay::GetPixelValue(long X, long Y, DWORD *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	*retVal = 0;

	if (!bmp)
		LoadImage();

	if (bmp)
	{
		Color c;
		AtlTrace("Point before transform: %d, %d\n", X, Y);
		PointF points[1] = { PointF(X, Y) };
		REAL el[6];
		CumulativeTransform(el);
		Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);
		if (name == BACKGROUND104)
			transform.Scale(128.0f / imageWidth, 128.0f / imageHeight);
		transform.Invert();
		transform.TransformPoints(points, 1);
		AtlTrace("Point after transform: %g, %g\n", points[0].X, points[0].Y);
		RectF bounds;
		Unit unit;
		bmp->GetBounds(&bounds, &unit);
		if (bounds.Contains(points[0]) && (bmp->GetPixel(points[0].X, points[0].Y, &c) == Ok))
		{
			*retVal = c.GetValue();
			AtlTrace("Pixel color=0x%0.8x\n", *retVal);
		}

		if (!keepImageOpen)
		{
			bmp = 0;
		}
	}

	return S_OK;
}

STDMETHODIMP CImageOverlay::Dispose()
{
	UnloadImage();

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_AnimationMeter(IMeter **pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IUnknown *iUnknown = 0;
	animationMeter->DeReference(&iUnknown);
	*pVal = (IMeter*)iUnknown;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_AnimationMeter(IMeter *newVal)
{
	animationMeter->AssignReference(newVal);

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_AnimationMeterIndex(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IMeterHome *pMeters;
	model->get_Meters(&pMeters);
	short index = -1;
	IUnknown *iUnknown = 0;
	animationMeter->DeReference(&iUnknown);
	pMeters->GetIndex((IMeter*)iUnknown, &index);
	*pVal = index;
	pMeters->Release();
		
	return S_OK;
}

STDMETHODIMP CImageOverlay::put_AnimationMeterIndex(long newVal)
{
	IMeterHome *pMeters;
	model->get_Meters(&pMeters);
	IMeter *pMeter;
	pMeters->Item(CComVariant(newVal), &pMeter);

	put_AnimationMeter(pMeter);
	if (pMeter)
		pMeter->Release();

	pMeters->Release();

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_HueShift(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = hueShift;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_HueShift(long newVal)
{
	hueShift = newVal;

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_Saturation(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = saturation;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_Saturation(double newVal)
{
	saturation = newVal;

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_KeepImageOpen(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = keepImageOpen;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_KeepImageOpen(VARIANT_BOOL newVal)
{
	keepImageOpen = newVal;

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_LimitWidth(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = limitWidth;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_LimitWidth(long newVal)
{
	limitWidth = newVal;

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_LimitHeight(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = limitHeight;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_LimitHeight(long newVal)
{
	limitHeight = newVal;

	return S_OK;
}

STDMETHODIMP CImageOverlay::get_Alignment(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = alignment;

	return S_OK;
}

STDMETHODIMP CImageOverlay::put_Alignment(short newVal)
{
	alignment = newVal;

	return S_OK;
}
