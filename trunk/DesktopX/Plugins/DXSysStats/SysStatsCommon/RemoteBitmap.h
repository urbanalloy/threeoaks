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

// RemoteBitmap.h: interface for the RemoteBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTEBITMAP_H__BEEC9368_23C2_4578_85F9_58F51F41E845__INCLUDED_)
#define AFX_REMOTEBITMAP_H__BEEC9368_23C2_4578_85F9_58F51F41E845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>

#include <Gdiplus.h>
using namespace Gdiplus;

class BitmapHTTPSink;
struct CSRef;

class RemoteBitmap
{
public:
	struct Notifier {
		virtual void loaded() = 0;
	};

	RemoteBitmap(Bitmap *bmp, Notifier &notifier);
	virtual ~RemoteBitmap();

	void LoadImage(BSTR name);

	Bitmap *operator->();
	Bitmap *operator=(Bitmap *bmp);
	operator Bitmap*();

private:
	void CheckOutstandingRequest();

	Bitmap *bmp;
	BitmapHTTPSink *loadThread;
	Notifier &notifier;
	CSRef *cs;

	friend class BitmapHTTPSink;
};

#endif // !defined(AFX_REMOTEBITMAP_H__BEEC9368_23C2_4578_85F9_58F51F41E845__INCLUDED_)
