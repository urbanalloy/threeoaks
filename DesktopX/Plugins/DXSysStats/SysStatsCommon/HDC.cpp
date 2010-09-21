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

// CHDC.cpp: implementation of the CHDC class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
#define AFX_CDECL __cdecl


// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, const char* lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#if _MSC_VER >= 1200
void AFX_CDECL operator delete(void* p, const char* lpszFileName, int nLine);
#endif

#endif // _DEBUG

#include "HDC.h"
#include "SysStatsUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHDC::CHDC(HDC _hdc) :
	hdc(_hdc)
{
}

CHDC::CHDC(long _hdc) :
	hdc((HDC)_hdc)
{
}

CHDC::~CHDC()
{
}

int CHDC::GetBytesPerPixel(int depth)
{
    return (depth==32 ? 4 : 3);
}

int CHDC::GetBytesPerRow(int width, int depth)
{
    int bytesPerPixel = GetBytesPerPixel(depth);
    int bytesPerRow = ((width * bytesPerPixel + 3) & ~3);

    return bytesPerRow;
}

int CHDC::GetBitmapBytes(int width, int height, int depth)
{
    return height * GetBytesPerRow(width, depth);
}

int CHDC::GetBitmapBytes()
{
	HBITMAP curhBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
	BITMAP bmpInfo = { 0, 0, 0, 0, 0, 0, NULL };
	GetObject(curhBmp, sizeof(BITMAP), &bmpInfo);

    return GetBitmapBytes(bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel);
}

/*
 * Input HDC is used to modify the alpha channel of the current HDC.
 * A black pixel in maskHDC will cause the corresponding pixel in the
 * current HDC to be transparent. A white pixel will leave the alpha
 * channel in the current HDC unchanged. Values in-between will have
 * a proportional effect on the current alpha channel.
 *
 * In actuality only the bottom byte of a pixel in maskHDC is used since
 * it is assumed that all the other bytes in the pixel have the same value.
 */
void CHDC::MaskIn(HDC maskHDC)
{
	HBITMAP curhBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
	BITMAP bmpInfo = { 0, 0, 0, 0, 0, 0, NULL };
	GetObject(curhBmp, sizeof(BITMAP), &bmpInfo);

	HBITMAP maskhBmp = (HBITMAP)::GetCurrentObject(maskHDC, OBJ_BITMAP);
	BITMAP maskInfo = { 0, 0, 0, 0, 0, 0, NULL };
	GetObject(maskhBmp, sizeof(BITMAP), &maskInfo);

	if (bmpInfo.bmWidth == maskInfo.bmWidth &&
		bmpInfo.bmHeight == maskInfo.bmHeight &&
		bmpInfo.bmType == maskInfo.bmType &&
		bmpInfo.bmBitsPixel == maskInfo.bmBitsPixel)
	{
		register unsigned char *pBits = (unsigned char*)bmpInfo.bmBits;
		register unsigned char *pDest = pBits;
		register unsigned char *pMask = (unsigned char*)maskInfo.bmBits;
		int size = GetBitmapBytes(bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel)-1;
		register unsigned char newAlpha;

		for (pDest += size, pMask += size-1; pDest >= pBits; pDest -= 4, pMask -= 4)
		{
			// Destination HDC will be pre-multipled. We need to un-multiply it, mask it then re-multiply it!
			// Source HDC will also be pre-multiplied, so we need to unmultiply that too!
			if (*pDest)
			{
				if (*pMask != 0)
				{
					newAlpha = *pMask;
					if (newAlpha != 255)
					{
						// We actually want to modify the destination alpha.
						// We apply the same scaling to all parts of the pixel
						// (i.e. ARGB)
						*pDest = (unsigned char)(*pDest * newAlpha / 256);
						*(pDest-1) = (unsigned char)(*(pDest-1) * newAlpha / 256);
						*(pDest-2) = (unsigned char)(*(pDest-2) * newAlpha / 256);
						*(pDest-3) = (unsigned char)(*(pDest-3) * newAlpha / 256);
					}
				}
				else
				{
					*(unsigned long*)(pDest - 3) = 0;
				}
			}
		}
	}
	else
	{
		// Fallback to something that approximates a mask.
		::BitBlt(hdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, maskHDC, 0, 0, SRCAND);
	}
}

/*
 * Input HDC is used to modify the alpha channel of the current HDC.
 * A white pixel in maskHDC will cause the corresponding pixel in the
 * current HDC to be transparent. A black pixel will leave the alpha
 * channel in the current HDC unchanged. Values in-between will have
 * a proportional effect on the current alpha channel.
 *
 * In actuality only the bottom byte of a pixel in maskHDC is used since
 * it is assumed that all the other bytes in the pixel have the same value.
 *
 * If either of the mask or destination bitmaps is not pre-multiplied with
 * the alpha channel. The calculation would be different. This is a good
 * source of explanation of pre-multiplying: 
 */
void CHDC::MaskOut(HDC maskHDC)
{
	HBITMAP curhBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
	BITMAP bmpInfo = { 0, 0, 0, 0, 0, 0, NULL };
	GetObject(curhBmp, sizeof(BITMAP), &bmpInfo);

	HBITMAP maskhBmp = (HBITMAP)::GetCurrentObject(maskHDC, OBJ_BITMAP);
	BITMAP maskInfo = { 0, 0, 0, 0, 0, 0, NULL };
	GetObject(maskhBmp, sizeof(BITMAP), &maskInfo);

	if (bmpInfo.bmWidth == maskInfo.bmWidth &&
		bmpInfo.bmHeight == maskInfo.bmHeight &&
		bmpInfo.bmType == maskInfo.bmType &&
		bmpInfo.bmBitsPixel == maskInfo.bmBitsPixel)
	{
		register unsigned char *pBits = (unsigned char*)bmpInfo.bmBits;
		register unsigned char *pDest = pBits;
		register unsigned char *pMask = (unsigned char*)maskInfo.bmBits;
		int size = GetBitmapBytes(bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel)-1;
		register unsigned char newAlpha;

		for (pDest += size, pMask += size-1; pDest >= pBits; pDest -= 4, pMask -= 4)
		{
			// Destination HDC will be pre-multipled. We need to un-multiply it, mask it then re-multiply it!
			// Source HDC will also be pre-multiplied, so we need to unmultiply that too!
			if (*pDest)
			{
				if (*pMask != 255)
				{
					// If you remove the ~ below the mask effect will be
					// reversed, i.e. a black pixel in the mask will cause
					// the destination pixel to become transparent.
					newAlpha = ~(*pMask);
					if (newAlpha != 255)
					{
						// We actually want to modify the destination alpha.
						// We apply the same scaling to all parts of the pixel
						// (i.e. ARGB)
						*pDest = (unsigned char)(*pDest * newAlpha / 256);
						*(pDest-1) = (unsigned char)(*(pDest-1) * newAlpha / 256);
						*(pDest-2) = (unsigned char)(*(pDest-2) * newAlpha / 256);
						*(pDest-3) = (unsigned char)(*(pDest-3) * newAlpha / 256);
					}
				}
				else
				{
					*(unsigned long*)(pDest - 3) = 0;
				}
			}
		}
	}
	else
	{
		// Fallback to something that approximates a mask.
		::BitBlt(hdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, maskHDC, 0, 0, 0x00220326 /* !S and D */);
	}
}
