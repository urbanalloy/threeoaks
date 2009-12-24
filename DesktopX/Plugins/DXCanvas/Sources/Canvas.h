///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008-2010, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "DXCanvas.h"
#include "resource.h"
#include <comsvcs.h>

#include <cairo.h>
#include <cairo-win32.h>

#include "CanvasRenderingContext2D.h"
#include "CanvasState.h"

#include <gdiplus.h>
using namespace Gdiplus;

#include <process.h>

#define _DEBUG_LOG
#include <LogFile.h>

#define DEFAULT_WIDTH 300
#define DEFAULT_HEIGHT 150

// CCanvas
class ATL_NO_VTABLE CCanvas :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvas, &CLSID_Canvas>,
	public IDispatchImpl<ICanvas, &IID_ICanvas, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
	friend class CCanvasRenderingContext2D;
	friend class CanvasState;

public:
	CCanvas() : surface(NULL),
				context(NULL),
				debugMode(FALSE),
				shouldDraw(FALSE),
				logFile(NULL),
				repeat(TRUE),
				hDrawMutex(NULL),
				isDrawingSuspended(FALSE)

	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		hThread = NULL;
		config = new CONFIG_CANVAS;
		state = new CanvasState(this);

		return S_OK;
	}

	void FinalRelease()
	{
		killthreadQueueDraw();

		SAFE_DELETE(state);

		SAFE_DELETE(config);
		SAFE_DELETE(logFile);

#ifdef USE_DRAWING_MUTEX
		if (hDrawMutex != NULL)
			CloseHandle(hDrawMutex);
#endif

		GdiplusShutdown(m_gdiplusToken);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CANVAS)

DECLARE_NOT_AGGREGATABLE(CCanvas)

BEGIN_COM_MAP(CCanvas)
	COM_INTERFACE_ENTRY(ICanvas)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:

	ULONG_PTR m_gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;

	cairo_surface_t *surface;
	cairo_t *context;
	CanvasState* state;

	BOOL debugMode;
	BOOL isDrawingSuspended;
	CLogFile *logFile;

	HANDLE hDrawMutex;

	// Draw thread
	HANDLE hThread;
	unsigned tid;	// Thread id
	static unsigned threadQueueDrawStatic(void* param);
	unsigned threadQueueDraw();
	BOOL repeat;
	BOOL shouldDraw;

	DWORD objID; // the object id used to send messages

public:

	struct CONFIG_CANVAS
	{
		int width;
		int height;
		bool alphablend; // used only by DX
		char objectDirectory[MAX_PATH];
		char objectName[100];

		CONFIG_CANVAS()
		{
			width = DEFAULT_WIDTH;
			height = DEFAULT_HEIGHT;
			alphablend = true;
			strcpy_s(objectDirectory, "");
			strcpy_s(objectName, "");
		}
	};

	CONFIG_CANVAS* config;

	void initCairo();
	void destroyCairo();
	void clearContext();
	void draw(HDC hdc, HBITMAP hBitmap);
	void queueDraw();
	void killthreadQueueDraw();
	void resize();
	void setObjID(DWORD id);

	cairo_surface_t* getSurface();
	int getWidth();
	int getHeight();

	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvas
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(put_width)(long width);
	STDMETHOD(get_width)(long *width);
	STDMETHOD(put_height)(long height);
	STDMETHOD(get_height)(long *height);
	STDMETHOD(put_debugMode)(VARIANT_BOOL isDebugMode);
	STDMETHOD(get_debugMode)(VARIANT_BOOL* isDebugMode);

	STDMETHOD(suspendDrawing)();
	STDMETHOD(resumeDrawing)();
	STDMETHOD(getContext)(BSTR type, ICanvasRenderingContext2D** context);
	STDMETHOD(toImage)(BSTR path, VARIANT_BOOL* result);

};

OBJECT_ENTRY_AUTO(__uuidof(Canvas), CCanvas)
