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

// BitmapHTTPSink.h: interface for the BitmapHTTPSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAPHTTPSINK_H__BC01800F_D33B_454D_8AA5_F5EEC1F9500D__INCLUDED_)
#define AFX_BITMAPHTTPSINK_H__BC01800F_D33B_454D_8AA5_F5EEC1F9500D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "WinHttp.dll" raw_interfaces_only no_namespace

#include "SysStatsUtils.h"
#include "RemoteBitmap.h"

struct IWinHttpRequest;

struct CSRef : public SysStatsUtils::CriticalSection
{
	CSRef() : SysStatsUtils::CriticalSection(), ref(0) {}

	void AddRef() { ref += 1; }
	void RemoveRef() { ref -= 1; if (ref == 0) delete this; }

private:
	int ref;
};

class BitmapHTTPSink
{
public:
	BitmapHTTPSink(RemoteBitmap *bmp, BSTR bstrURL, CSRef *cs);
	virtual ~BitmapHTTPSink();

private:
	void ReadResponse();
	HRESULT SendRequest(BSTR bstrUrl);
	IWinHttpRequest *pIWinHttpRequest;

	RemoteBitmap *bmp;

	HANDLE hUpdateThread;
	BSTR bstrURL;
	VARIANT_BOOL loadFailed;
	Bitmap *localBmp;
	CSRef *cs;

	DWORD WINAPI updateThread(/*LPVOID lpInput*/);
	static DWORD WINAPI _updateThread(LPVOID in);

	friend class RemoteBitmap;
};

#endif // !defined(AFX_BITMAPHTTPSINK_H__BC01800F_D33B_454D_8AA5_F5EEC1F9500D__INCLUDED_)
