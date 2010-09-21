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

// HDCImage.h: interface for the HDCImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDCIMAGE_H__A80C2CA0_45BE_48D9_8841_F0F322247AEE__INCLUDED_)
#define AFX_HDCIMAGE_H__A80C2CA0_45BE_48D9_8841_F0F322247AEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>
#include <Gdiplus.h>
using namespace Gdiplus;

#include "HDC.h"

class HDCImage : public CHDC
{
public:
	HDCImage(HDC _hdc, INT width, INT height, PixelFormat pixelFormat=PixelFormat32bppPARGB);
	virtual ~HDCImage();

	Image& GetImage();
	operator HBITMAP() { return hBitmap; }

protected:
	HBITMAP hBitmap;
	Bitmap *buffered;
	Bitmap *returnBitmap;
	HGDIOBJ oldObj;
};

#endif // !defined(AFX_HDCIMAGE_H__A80C2CA0_45BE_48D9_8841_F0F322247AEE__INCLUDED_)
