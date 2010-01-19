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

// TextOverlay.cpp : Implementation of CTextOverlay
#include "stdafx.h"
#include <comdef.h>
#include "COMOverlays.h"
#include "SimpleOverlayImpl2.cpp"
#include "TextOverlay.h"
#include "TextOverlayDialog.h"

const char* CTextOverlay::FORMAT_PROPERTY = "Format";

/////////////////////////////////////////////////////////////////////////////
// CTextOverlay

STDMETHODIMP CTextOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ITextOverlay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

static TextRenderingHint getDefaultRenderingHint(short inHint)
{
	if (inHint != TextRenderingHintSystemDefault) {
		return (TextRenderingHint)inHint;
	}

	TextRenderingHint hint = (TextRenderingHint)inHint;

	HKEY hKey;
	// Create and open key and subkey.
	long lResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
									"Control Panel\\Desktop", 
									0, 
									KEY_READ, 
									&hKey);
	if (lResult == ERROR_SUCCESS)
	{
		// Get the Value.
		DWORD dwData = 0;
		DWORD dwSize = sizeof(dwData) ;
		lResult = RegQueryValueEx(hKey, "FontSmoothingType", NULL, NULL, (BYTE *)&dwData, &dwSize);

		if (lResult == ERROR_SUCCESS)
		{
			switch (dwData)
			{
			case 1:
				hint = TextRenderingHintAntiAliasGridFit;
				break;
			case 2:
				hint = TextRenderingHintClearTypeGridFit;
				break;
			default:
				hint = TextRenderingHintSingleBitPerPixelGridFit;
				break;
			}
		}
		
		RegCloseKey(hKey);
	}

	return hint;
}

CTextOverlay::CTextOverlay() :
	pWeakTarget(0),
	width(-1),
	height(-1),
	alternateRender(false),
	wrap(false),
	vertical(false),
	radius(0),
	textRenderingHint(TextRenderingHintSystemDefault),
	textContrast(4),
	format(""),
	blurColor(-1)
{
    memset(&chooseFont,0,sizeof(CHOOSEFONT));   
    chooseFont.lStructSize=sizeof(CHOOSEFONT);
    chooseFont.lpLogFont = &logFont;
	chooseFont.lpszStyle = fontStyle;
    memset(&logFont,0,sizeof(LOGFONT));
	chooseFont.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_USESTYLE;
	put_FontSize(42.0);
	put_FontStyle(FontStyleBold);
	put_FontColor(0);
	put_HAlign(StringAlignmentCenter);
	put_VAlign(StringAlignmentCenter);
	BSTR b = ::SysAllocString(L"Arial");
	put_FontName(b);
	::SysFreeString(b);
}

HRESULT CTextOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CTextOverlay::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CTextOverlay::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_TextOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);
		
	return S_OK;
}

// From SimpleOverlayImpl<> - used by 'clone'.
IOverlay *CTextOverlay::createInstance()
{
	CComObject<CTextOverlay> *pRet;
	CComObject<CTextOverlay>::CreateInstance(&pRet);

	return pRet;
}

STDMETHODIMP CTextOverlay::clone(IOverlay * * pRet)
{
	if (pRet == NULL)
		return E_POINTER;
		
	SimpleOverlayImpl2<ITextOverlay>::clone(pRet);

	CTextOverlay *o = (CTextOverlay*) *pRet;

	o->put_Radius(radius);
	float fs;
	get_FontSize(&fs);
	o->put_FontSize(fs);
	int fStyle;
	get_FontStyle(&fStyle);
	o->put_FontStyle(fStyle);
	BSTR fName;
	get_FontName(&fName);
	o->put_FontName(fName);
	// We now own fName, so free it:
	::SysFreeString(fName);
	long fColor;
	get_FontColor(&fColor);
	o->put_FontColor(fColor);
	o->put_Format(format);
	o->put_BlurColor(blurColor);

	o->put_AlternateRender(alternateRender);
	o->put_Wrap(wrap);
	o->put_TextRenderingHint(textRenderingHint);
	o->put_TextContrast(textContrast);
	o->put_Width(width);
	o->put_Height(height);

	return S_OK;
}

STDMETHODIMP CTextOverlay::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	TextOverlayDialog *pDialog = new TextOverlayDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

STDMETHODIMP CTextOverlay::HitTest(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	*retVal = false;

	PointF points[1] = { PointF(X, Y) };
	REAL el[6];
	CumulativeTransform(el);
	Matrix transform(el[0], el[1], el[2], el[3], el[4], el[5]);
	transform.Invert();
	transform.TransformPoints(points, 1);
	if (bounds.Contains(points[0]))
		*retVal = true; 

	return S_OK;
}

STDMETHODIMP CTextOverlay::Render(LONG hdc)
{
	PointF zero(0.0, 0.0);

	// Get the text we want to draw!
	BSTR text;

	IMeter *iMeter = 0;
	get_meter(&iMeter);
	if (!iMeter)
	{
		text = format.copy();
	}
	else
	{
		_bstr_t tmp("");
		BSTR tCopy = tmp.copy();
		text = tCopy;
		iMeter->GetAsString(format, selector, &text);
		if (text != tCopy)
			::SysFreeString(tCopy);
	}

	// Create graphics object
	Graphics graphics((HDC)hdc);
	graphics.SetInterpolationMode(InterpolationModeHighQuality);
	graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	
	graphics.SetTextRenderingHint(getDefaultRenderingHint(textRenderingHint));
	graphics.SetCompositingMode(CompositingModeSourceOver);
	graphics.SetTextContrast(textContrast);

	// Calculate world transformation
	REAL el[6];
	CumulativeTransform(el);
	Matrix matrix(el[0], el[1], el[2], el[3], el[4], el[5]);
//	matrix.Invert();
	graphics.SetTransform(&matrix);

	// Create fontFamily for path-based font rendering
	BSTR fName;
	get_FontName(&fName);
	FontFamily  fontFamily(fName);
	::SysFreeString(fName);

	// Get some extra stuff for path-based rendering
	int fStyle;
	get_FontStyle(&fStyle);
	float fSize;
	get_FontSize(&fSize);

	// Set font for alternate rendering
//	Font font((HDC)hdc, &logFont);
	Font font(&fontFamily, fSize, fStyle, UnitPixel);

	// Set font color
	SolidBrush  fg(color);

	// Set up string format
	StringFormat format(vertical ? StringFormatFlagsDirectionVertical : 0, LANG_NEUTRAL);
	if (!wrap)
		format.SetFormatFlags(format.GetFormatFlags() |  StringFormatFlagsNoWrap);
	format.SetAlignment((StringAlignment)hAlign);
	format.SetLineAlignment((StringAlignment)vAlign);

	// Calculate clipping rectangle
	RectF clipRect(0.0, 0.0, (float)width, (float)height);
	if (hAlign == 1)	// Center
		clipRect.X = (float)-width/2;
	else if (hAlign == 2)	// Right
		clipRect.X = (float)-width;

	if (vAlign == 1)	// Center
		clipRect.Y = (float)-height/2;
	else if (vAlign == 2)	// Right
		clipRect.Y = (float)-height;

	// Draw blur
	if (radius > 0)
	{
		GraphicsPath blurPath;
		if (width > -1 && height > -1)
		{
			blurPath.AddString(text, -1, &fontFamily, fStyle, fSize, clipRect, &format);
		}
		else
		{
			blurPath.AddString(text, -1, &fontFamily, fStyle, fSize, zero, &format);
		}
		BYTE alpha;
		get_Alpha(&alpha);
		for (int i=radius; i>0; i--)
		{
			Color _blurColor(alpha/radius, (BYTE)(blurColor >> Color::BlueShift), (BYTE)(blurColor >> Color::GreenShift), (BYTE)(blurColor >> Color::RedShift));
			if (blurColor == -1)
			{
				_blurColor = Color(alpha/radius, color.GetRed(), color.GetGreen(), color.GetBlue());
			}
			Pen blurPen(_blurColor, (float)2*i);
			blurPen.SetEndCap(LineCapRound);
			blurPen.SetMiterLimit(1);

			graphics.DrawPath(&blurPen, &blurPath);
		}

		GraphicsState state = graphics.Save();
		graphics.SetClip(&blurPath);
		Color trans(0, 0, 0, 0);
		SolidBrush brush(trans);
		graphics.FillPath(&brush, &blurPath);
		graphics.Restore(state);
	}

	// Create graphics path for path-based rendering
	GraphicsPath path(FillModeAlternate);
	if (width > -1 && height > -1)
	{
		GraphicsPath clipPath(FillModeAlternate);
		clipPath.AddRectangle(clipRect);
		graphics.SetClip(&clipPath);

		if (alternateRender)
		{
			graphics.DrawString(text, -1, &font, clipRect, &format, &fg);
			graphics.MeasureString(text, -1, &font, clipRect, &format, &bounds);
		}
		else
		{
			path.AddString(text, -1, &fontFamily, fStyle, fSize, clipRect, &format);
			path.GetBounds(&bounds);
		}

		bounds.Intersect(clipRect);
	}
	else
	{
		if (alternateRender)
		{
			graphics.DrawString(text, -1, &font, zero, &format, &fg);
		}
		else
		{
			path.AddString(text, -1, &fontFamily, fStyle, fSize, zero, &format);
			path.GetBounds(&bounds);
		}
	}

	if (alternateRender)
	{
	}
	else
	{
		graphics.FillPath(&fg, &path);
	}

	::SysFreeString(text);

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_FontName(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	HDC screen = GetDC(NULL);
	Font font(screen, &logFont);
	FontFamily family;
    font.GetFamily(&family);
	WCHAR fontName[LF_FACESIZE];
	family.GetFamilyName(fontName);
	ReleaseDC(NULL, screen);

	// The rule is that the caller will own the BSTR - so it had better be on
	// the heap!
	*pVal = ::SysAllocString(fontName);

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_FontName(BSTR newVal)
{
	// Create a Font object.
	float f = ((float)chooseFont.iPointSize) / 10.0F;
//	get_FontSize(&f);
	Font myFont(newVal, f);

    // Get attributes of myFont.
	Bitmap image(128, 128, PixelFormat32bppARGB);

	Graphics graphics(&image);

	myFont.GetLogFontA(&graphics, &logFont);

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_FontStyle(int *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	if (strcmp("Bold", fontStyle) == 0)
		*pVal = FontStyleBold;
	else if (strcmp("Italic", fontStyle) == 0)
		*pVal = FontStyleItalic;
	else if (strcmp("BoldItalic", fontStyle) == 0)
		*pVal = FontStyleBoldItalic;
	else if (strcmp("Underline", fontStyle) == 0)
		*pVal = FontStyleUnderline;
	else if (strcmp("Strikeout", fontStyle) == 0)
		*pVal = FontStyleStrikeout;
	else
		*pVal = FontStyleRegular;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_FontStyle(int newVal)
{
	switch (newVal)
	{
	case FontStyleBold       : strcpy(fontStyle,"Bold"); break;
	case FontStyleItalic     : strcpy(fontStyle,"Italic"); break;
	case FontStyleBoldItalic : strcpy(fontStyle,"BoldItalic"); break;
	case FontStyleUnderline  : strcpy(fontStyle,"Underline"); break;
	case FontStyleStrikeout  : strcpy(fontStyle,"Strikeout"); break;
	default                  : strcpy(fontStyle,"Regular"); break;
	}

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_FontSize(float *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = ((float)chooseFont.iPointSize) / 10.0F;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_FontSize(float newVal)
{
	chooseFont.iPointSize = (int)(newVal * 10);

	HDC screen = GetDC(NULL);
	Font font(screen, &logFont);
	FontFamily family;
    font.GetFamily(&family);
	WCHAR fontName[LF_FACESIZE];
	family.GetFamilyName(fontName);
	ReleaseDC(NULL, screen);

	Font myFont(fontName, newVal);

    // Get attributes of myFont.
	Bitmap image(128, 128, PixelFormat32bppARGB);

	Graphics graphics(&image);

	myFont.GetLogFontA(&graphics, &logFont);

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_FontColor(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = (long)color.ToCOLORREF();

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_FontColor(long newVal)
{
	BYTE alpha = color.GetAlpha();
	color.SetFromCOLORREF((COLORREF)newVal);
	ARGB val = color.GetValue() & 0x00FFFFFF;
	val |= alpha << 24;
	color.SetValue(val);

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_Alpha(BYTE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = color.GetAlpha();

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_Alpha(BYTE newVal)
{
	ARGB val = color.GetValue() & 0x00FFFFFF;
	val |= newVal << 24;
	color.SetValue(val);

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_Radius(int *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = radius;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_Radius(int newVal)
{
	radius = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_Format(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = format.copy();

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_Format(BSTR newVal)
{
	format = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_Width(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = width;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_Width(long newVal)
{
	width = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_Height(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = height;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_Height(long newVal)
{
	height = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_AlternateRender(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = alternateRender;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_AlternateRender(short newVal)
{
	alternateRender = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_Wrap(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = wrap;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_Wrap(short newVal)
{
	wrap = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_Vertical(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = vertical;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_Vertical(short newVal)
{
	vertical = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_TextRenderingHint(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = textRenderingHint;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_TextRenderingHint(short newVal)
{
	textRenderingHint = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_TextContrast(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = textContrast;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_TextContrast(UINT newVal)
{
	textContrast = newVal;

	return S_OK;
}

STDMETHODIMP CTextOverlay::get_BlurColor(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = blurColor;

	return S_OK;
}

STDMETHODIMP CTextOverlay::put_BlurColor(long newVal)
{
	blurColor = newVal;

	return S_OK;
}
