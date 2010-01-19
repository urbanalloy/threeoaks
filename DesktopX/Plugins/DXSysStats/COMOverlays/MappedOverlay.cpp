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

// MappedOverlay.cpp : Implementation of CMappedOverlay
#include "stdafx.h"

#include <direct.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <comdef.h>
#include <math.h>
#include "COMOverlays.h"
#include "SimpleOverlayImpl2.cpp"
#include "MappedOverlay.h"
#include "MappedOverlayDialog.h"
#include "HDCImage.h"
#include "QColorMatrix.h"

#undef min
#undef max


/////////////////////////////////////////////////////////////////////////////
// CMappedOverlay

STDMETHODIMP CMappedOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IMappedOverlay
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
CMappedOverlay::CMappedOverlay() :
	alpha(255),
	min(0),
	max(0),
	imagePath(""),
	mapPath(""),
	imageHeight(-1),
	imageWidth(-1),
	pWeakTarget(0),
	bmp(0, *this),
	map(0, *this),
	hdcImage(0),
	oldFactor(-3.1415927F),
	saturation(1.0),
	hueShift(0),
	limitWidth(0),
	limitHeight(0),
	alignment(0)
{
}

HRESULT CMappedOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CMappedOverlay::FinalRelease()
{
	pWeakTarget->Release();
	if (hdcImage)
		delete hdcImage;
}

/*
 * Return a string representation of the overlay’s type.
 */
STDMETHODIMP CMappedOverlay::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MappedOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

/*
 * From SimpleOverlayImpl<> - used by 'clone'.
 */
IOverlay *CMappedOverlay::createInstance()
{
	CComObject<CMappedOverlay> *pRet;
	CComObject<CMappedOverlay>::CreateInstance(&pRet);

	return pRet;
}

/*
 * This method is called by SysStats - it should copy all of the state of
 * this object into 'o'.
 */
STDMETHODIMP CMappedOverlay::clone(IOverlay * * pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// Clone all the superclass's state
	SimpleOverlayImpl2<IMappedOverlay>::clone(pRet);

	CMappedOverlay *o = (CMappedOverlay*) *pRet;

	o->put_Min(min);
	o->put_Max(max);
	o->put_Alpha(alpha);
	o->put_ImagePath(imagePath);
	o->put_MapPath(mapPath);
	o->put_HueShift(hueShift);
	o->put_Saturation(saturation);
	o->put_LimitWidth(limitWidth);
	o->put_LimitHeight(limitHeight);
	o->put_Alignment(alignment);

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CMappedOverlay::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	MappedOverlayDialog *pDialog = new MappedOverlayDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::Dispose()
{
	UnloadImage();

	return S_OK;
}

STDMETHODIMP CMappedOverlay::HitTest(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	DWORD pixelColor = 0;
	HRESULT hr = GetPixelValue(X, Y, &pixelColor);

	*retVal = (pixelColor & 0xFF000000) != 0;

	return hr;
}

STDMETHODIMP CMappedOverlay::GetPixelValue(long X, long Y, DWORD *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	*retVal = 0;

	Image *image = LoadCombinedImage();

	if (image)
	{
		Color c;
		AtlTrace("Point before transform: %d, %d\n", X, Y);
		PointF points[1] = { PointF(X, Y) };
		REAL el[6];
		CumulativeTransform(el);
		Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);
		transform.Invert();
		transform.TransformPoints(points, 1);
		AtlTrace("Point after transform: %g, %g\n", points[0].X, points[0].Y);
		RectF bounds;
		Unit unit;
		image->GetBounds(&bounds, &unit);
		if (bounds.Contains(points[0]) && (((Bitmap*)image)->GetPixel(points[0].X, points[0].Y, &c) == Ok))
		{
			*retVal = c.GetValue();
			AtlTrace("Pixel color=0x%0.8x\n", *retVal);
		}
	}

	return S_OK;
}

/*
 * Called by SysStats when the Overlay should render itself
 */
STDMETHODIMP CMappedOverlay::Render(LONG hdc)
{
	Image *image = LoadCombinedImage();

	if (image)
	{
		Graphics g((HDC)hdc);
		g.SetInterpolationMode(InterpolationModeHighQuality);
		g.SetSmoothingMode(SmoothingModeAntiAlias);
		g.SetPixelOffsetMode(PixelOffsetModeHalf);

		QColorMatrix colorMatrix;

		colorMatrix.SetAlpha(((float)alpha)/255.0f);

		if (saturation != 1.0)
			colorMatrix.SetSaturation(saturation);

		if (hueShift != 0)
			colorMatrix.RotateHue(hueShift);

 		// Create an ImageAttributes object and set its color matrix.
		ImageAttributes imageAtt;
		imageAtt.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

		REAL el[6];
		CumulativeTransform(el);
		Matrix matrix(el[0], el[1], el[2], el[3], el[4], el[5]);

		g.MultiplyTransform(&matrix);
		g.DrawImage(
			image, 
			Rect(0, 0, imageWidth, imageHeight),	// Destination rectangle
			0,						// Source rectangle X 
			0,						// Source rectangle Y
			imageWidth,		// Source rectangle width
			imageHeight,		// Source rectangle height
			UnitPixel, 
			&imageAtt);
	}

	return S_OK;
}

/*
 * Load and combine the images.
 */
Image *CMappedOverlay::LoadCombinedImage()
{
	LoadImage(imagePath, bmp);

	IMeter *iMeter;
	get_meter(&iMeter);
	if (iMeter)
	{
		float factor = 0.0;
		double d;
		iMeter->GetAsDouble(selector, &d);
		if (max != min)
		{
			factor = (d - min) / (max-min);
			factor -= floor(factor);	// Just take the fractional part
		}

		if ((factor != oldFactor) && (bmp != NULL))
		{
			oldFactor = factor;

			LoadImage(mapPath, map);
			if (map)
			{
				// Create a private HDC
				if (hdcImage)
					delete hdcImage;
				hdcImage = new HDCImage(NULL, imageWidth, imageHeight);

				// Draw into the private dc
				Graphics graphics(*hdcImage);
				graphics.DrawImage(
				   bmp, 
				   Rect(0, 0, imageWidth, imageHeight),	// Destination rectangle
				   0,						// Source rectangle X 
				   0,						// Source rectangle Y
				   imageWidth,		// Source rectangle width
				   imageHeight,		// Source rectangle height
				   UnitPixel, 
				   NULL);

				HDCImage hdcMask(NULL, imageWidth, imageHeight);

				Graphics mapGraphics(hdcMask);
				mapGraphics.SetInterpolationMode(InterpolationModeHighQuality);
				mapGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
				mapGraphics.SetPixelOffsetMode(PixelOffsetModeHalf);

				Color thresholdColor(factor*255+0.5, factor*255+0.5, factor*255+0.5);
				ImageAttributes	mapAtt; 
				mapAtt.SetThreshold(factor, ColorAdjustTypeBitmap);
				mapAtt.SetColorKey(Color::Black, thresholdColor, ColorAdjustTypeBitmap);

				mapGraphics.DrawImage(
				   map, 
				   Rect(0, 0, imageWidth, imageHeight),	// Destination rectangle
				   0,						// Source rectangle X 
				   0,						// Source rectangle Y
				   imageWidth,		// Source rectangle width
				   imageHeight,		// Source rectangle height
				   UnitPixel,
				   &mapAtt);

				hdcImage->MaskOut(hdcMask);
			}
		}


		if (hdcImage)
			return &hdcImage->GetImage();
		else
			return bmp;
	}

	return bmp;
}

STDMETHODIMP CMappedOverlay::get_Min(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = min;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_Min(long newVal)
{
	min = newVal;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::get_Max(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = max;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_Max(long newVal)
{
	max = newVal;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::get_Alpha(BYTE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = alpha;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_Alpha(BYTE newVal)
{
	alpha = newVal;

	return S_OK;
}

// Get as a relative imagePath
STDMETHODIMP CMappedOverlay::get_ImagePath(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IClientModel *_model = 0;
	get_Model(&_model);
	IAppConfig *appConfig = 0;
	_model->get_AppConfig(&appConfig);

	appConfig->NormalizePath(imagePath, pVal);

	appConfig->Release();
	_model->Release();

	return S_OK;
}

// Store as an absolute imagePath
STDMETHODIMP CMappedOverlay::put_ImagePath(BSTR newVal)
{
	UnloadImage();

	IAppConfig *appConfig = 0;
	model->get_AppConfig(&appConfig);

	BSTR tmp;
	appConfig->CanonicalPath(newVal, &tmp);
	imagePath = tmp;
	::SysFreeString(tmp);

	appConfig->Release();

	imageHeight = -1;
	imageWidth = -1;

	return S_OK;
}

// Get as a relative imagePath
STDMETHODIMP CMappedOverlay::get_MapPath(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IClientModel *_model = 0;
	get_Model(&_model);
	IAppConfig *appConfig = 0;
	_model->get_AppConfig(&appConfig);

	appConfig->NormalizePath(mapPath, pVal);

	appConfig->Release();
	_model->Release();

	return S_OK;
}

// Store as an absolute imagePath
STDMETHODIMP CMappedOverlay::put_MapPath(BSTR newVal)
{
	UnloadImage();

	IAppConfig *appConfig = 0;
	model->get_AppConfig(&appConfig);

	BSTR tmp;
	appConfig->CanonicalPath(newVal, &tmp);
	mapPath = tmp;
	::SysFreeString(tmp);

	appConfig->Release();

	return S_OK;
}

void CMappedOverlay::loaded() {
	if (bmp)
	{
		imageWidth = bmp->GetWidth();
		imageHeight = bmp->GetHeight();
	}

	if (map)
	{
		imageWidth = map->GetWidth();
		imageHeight = map->GetHeight();
	}

	model->put_dirty(1);
}

void CMappedOverlay::LoadImage(_bstr_t path, RemoteBitmap &_bmp)
{
	BSTR text;

	if (_bmp)
		return;

	text = path.copy();

	_bmp.LoadImage(text);
	
	if (_bmp)
	{
		imageWidth = _bmp->GetWidth();
		imageHeight = _bmp->GetHeight();
	}

	::SysFreeString(text);

	return;
}

STDMETHODIMP CMappedOverlay::get_ImageHeight(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	if (imageHeight == -1)
	{
		LoadImage(imagePath, bmp);
	}

	*pVal = imageHeight;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::get_ImageWidth(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	if (imageWidth == -1)
	{
		LoadImage(imagePath, bmp);
	}


	*pVal = imageWidth;

	return S_OK;
}

void CMappedOverlay::UnloadImage()
{
//	Does this work when called from another thread?
	if (bmp)
	{
		bmp = 0;
		imageHeight = -1;
		imageWidth = -1;
	}

	if (map)
	{
		map = 0;
	}

	if (hdcImage)
	{
		delete hdcImage;
		hdcImage = 0;
		oldFactor = -3.1415927;
	}
}

STDMETHODIMP CMappedOverlay::get_HueShift(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = hueShift;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_HueShift(long newVal)
{
	hueShift = newVal;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::get_Saturation(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = saturation;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_Saturation(double newVal)
{
	saturation = newVal;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::get_LimitWidth(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = limitWidth;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_LimitWidth(long newVal)
{
	limitWidth = newVal;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::get_LimitHeight(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = limitHeight;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_LimitHeight(long newVal)
{
	limitHeight = newVal;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::get_Alignment(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = alignment;

	return S_OK;
}

STDMETHODIMP CMappedOverlay::put_Alignment(short newVal)
{
	alignment = newVal;

	return S_OK;
}
