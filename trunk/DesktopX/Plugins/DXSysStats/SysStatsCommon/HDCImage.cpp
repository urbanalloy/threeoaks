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

// HDCImage.cpp: implementation of the HDCImage class.
//
//////////////////////////////////////////////////////////////////////

#include "HDCImage.h"
#include "SysStatsUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDCImage::HDCImage(HDC _hdc, INT width, INT height, PixelFormat pixelFormat) :
	CHDC(::CreateCompatibleDC(_hdc)),
	buffered(new Bitmap(width, height, pixelFormat)),
	returnBitmap(0)
{
	::memset(&hBitmap, 0, sizeof(HBITMAP));
	buffered->GetHBITMAP(Color::Black, &hBitmap);
	oldObj = ::SelectObject(hdc, hBitmap);
	if (!oldObj)
		SysStatsUtils::checkError();
}

HDCImage::~HDCImage()
{
	::SelectObject(hdc, oldObj);
	::DeleteObject(hBitmap);
	::DeleteDC(hdc);
	delete buffered;
	if (returnBitmap)
		delete returnBitmap;
}

Image& HDCImage::GetImage()
{
	if (!returnBitmap)
	{
		returnBitmap = new Bitmap(buffered->GetWidth(), buffered->GetHeight());

		Rect rc(0, 0, returnBitmap->GetWidth(), returnBitmap->GetHeight());
		BitmapData bmpData;
		returnBitmap->LockBits(&rc, ImageLockModeWrite, buffered->GetPixelFormat(), &bmpData);

		HBITMAP curhBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
		::GetBitmapBits(curhBmp, GetBitmapBytes(), bmpData.Scan0);

		returnBitmap->UnlockBits(&bmpData);
	}

	return *returnBitmap;
}