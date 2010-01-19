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

// RemoteBitmap.cpp: implementation of the RemoteBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <atlbase.h>

#include "RemoteBitmap.h"
#include "BitmapHTTPSink.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RemoteBitmap::RemoteBitmap(Bitmap *bmp, Notifier &notifier) :
	bmp(bmp),
	loadThread(0),
	notifier(notifier),
	cs(0)
{
}

RemoteBitmap::~RemoteBitmap()
{
	if (bmp)
		delete bmp;

	{
		SysStatsUtils::Lock lock(cs);
		if (loadThread)
			loadThread->bmp = NULL;
	}

	if (cs) {
		cs->RemoveRef();
	}
}

void RemoteBitmap::LoadImage(BSTR fileName)
{
	struct _stat sbuf;

	AtlTrace("image filename=%s\n", (char*)_bstr_t(fileName));
	if (::wcscspn(fileName, L"http://") == 0)
	{
		// Disconnect us from any existing thread
		{
			SysStatsUtils::Lock lock(cs);
			if (loadThread) {
				if (::wcscmp(fileName, loadThread->bstrURL) == 0) {
					AtlTrace("Gawd. Give us a chance guv\n");
					return;
				}

				AtlTrace("Cutting existing thread free\n");
				loadThread->bmp = NULL;
			}
		}

		if (cs)
			cs->RemoveRef();

		cs = new CSRef();
		cs->AddRef();

		SysStatsUtils::Lock lock(cs);
		loadThread = new BitmapHTTPSink(this, fileName, cs);
	}
	else
	{
		if (bmp)
			delete bmp;

		bmp = 0;

		if (_wstat(fileName, &sbuf) == 0)
		{
			if (wcsstr(fileName, L".ico") == NULL)
			{
				bmp = Bitmap::FromFile(fileName);
			}
			else
			{
				HICON icon = (HICON) ::LoadImage(0,_bstr_t(fileName),IMAGE_ICON,128,128,LR_LOADFROMFILE);

				ICONINFO iconInfo = {0};
				iconInfo.fIcon = TRUE;
				GetIconInfo(icon,&iconInfo);
				
				BitmapData data;
				bmp = new Bitmap(iconInfo.xHotspot*2,iconInfo.yHotspot*2);
				Rect r(0,0,bmp->GetWidth(),bmp->GetHeight());
				bmp->LockBits(&r,ImageLockModeWrite,PixelFormat32bppARGB,&data);

				GetBitmapBits(iconInfo.hbmColor,iconInfo.xHotspot*iconInfo.yHotspot*4*4,data.Scan0);

				bmp->UnlockBits(&data);

				DeleteObject(iconInfo.hbmColor);
				DeleteObject(iconInfo.hbmMask);
				DestroyIcon(icon);
			}
		}
	}
}

Bitmap *RemoteBitmap::operator->()
{
	SysStatsUtils::Lock lock(cs);
	return bmp;
}

Bitmap *RemoteBitmap::operator=(Bitmap *bmp)
{
	SysStatsUtils::Lock lock(cs);
	if (this->bmp)
		delete this->bmp;
	this->bmp = bmp;

	return bmp;
}

RemoteBitmap::operator Bitmap*()
{
	SysStatsUtils::Lock lock(cs);
	return bmp;
}
