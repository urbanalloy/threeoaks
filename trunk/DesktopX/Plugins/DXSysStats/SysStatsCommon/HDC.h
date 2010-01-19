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

#if !defined(AFX_CHDC_H__A80C2CA0_45BE_48D9_8841_F0F322247AEE__INCLUDED_)
#define AFX_CHDC_H__A80C2CA0_45BE_48D9_8841_F0F322247AEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>
#include <GdiplusH.h>

class CHDC  
{
public:
	CHDC(HDC _hdc);
	CHDC(long _hdc);
	virtual ~CHDC();

	operator HDC() { return hdc; }
	operator long() { return (long)hdc; }
	void MaskIn(HDC maskHDC);
	void MaskOut(HDC maskHDC);

protected:
	int GetBytesPerPixel(int depth);
	int GetBytesPerRow(int width, int depth);
	int GetBitmapBytes(int width, int height, int depth);
	int GetBitmapBytes();

	HDC hdc;
};

#endif // !defined(AFX_CHDC_H__A80C2CA0_45BE_48D9_8841_F0F322247AEE__INCLUDED_)
