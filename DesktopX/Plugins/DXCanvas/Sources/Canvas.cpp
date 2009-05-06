///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Canvas.h"

#include "COMError.h"
#include "Config.h"

#include <SDPlugin.h>
extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);

void CCanvas::initCairo()
{
	// Init log file
	if (!logFile) {
		char logName[MAX_PATH];
		sprintf_s(logName, "%s\\DXCanvas-%s.log", config->objectDirectory, config->objectName);
		logFile = new CLogFile(logName, FALSE);

		logFile->Write("=====================================================================================================================");
		logFile->Write("== DXCanvas v%d.%d build %d", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
		logFile->Write("=====================================================================================================================");
	}

	logFile->Write("Creating new surface (%dx%d)", config->width, config->height);
	surface = cairo_win32_surface_create_with_dib(CAIRO_FORMAT_ARGB32, config->width, config->height);	
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		logFile->Write("[ERROR] Error creating cairo surface: %d", cairo_status_to_string(cairo_surface_status(surface)));
		return;
	}

	logFile->Write("Creating new context");
	context = cairo_create(surface);
	if (cairo_status(context) != CAIRO_STATUS_SUCCESS) {
		logFile->Write("[ERROR] Error creating cairo context: %d", cairo_status_to_string(cairo_status(context)));
		return;
	}

	// Clear surface
	cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
	cairo_new_path(context);
	cairo_rectangle(context, 0, 0, config->width, config->height);
	cairo_fill(context);

	// Load defaults values
	cairo_set_line_width(context, DEFAULT_LINE_WIDTH);
	cairo_set_line_cap(context, DEFAULT_LINE_CAP);
	cairo_set_line_join(context, DEFAULT_LINE_JOIN);
	cairo_set_miter_limit(context, DEFAULT_MITER_LIMIT);
	cairo_set_operator(context, DEFAULT_OPERATOR);

	cairo_new_path(context);

#ifdef USE_DRAWING_MUTEX
	// init the drawing mutex
	if (hDrawMutex == NULL) {
		char name[MAX_PATH];
		sprintf_s(name, "CairoDrawingMutex-%d", objID);
		hDrawMutex = CreateMutex(NULL, false, name);
	}
#endif

    state->clear();		
}

void CCanvas::destroyCairo()
{	
	logFile->Write("Destroying surface and context");
	logFile->Write("----------------------------------------------------");
	if (context)
		cairo_destroy(context);

	if (surface)
		cairo_surface_destroy(surface);	

#ifdef USE_DRAWING_MUTEX
	if (hDrawMutex != NULL) {
		CloseHandle(hDrawMutex);
		hDrawMutex = NULL;
	}
#endif
}

#pragma warning(push)
#pragma warning(disable: 4100)
void CCanvas::draw(HDC hdc, HBITMAP hBitmap)
{
	if (surface == NULL)
		return;

	ACQUIRE_DRAWING_MUTEX(hDrawMutex)

	HDC cairoDC = cairo_win32_surface_get_dc(surface);	

	BitBlt(hdc, 0, 0, config->width, config->height, cairoDC, 0, 0, SRCCOPY);

	RELEASE_DRAWING_MUTEX(hDrawMutex)
}
#pragma warning(pop)

void CCanvas::queueDraw()
{
	// nothing to draw...
	if (surface == NULL) 
		return;

	if (hThread == NULL)
		hThread = (HANDLE) _beginthreadex(NULL, 0, &CCanvas::threadQueueDrawStatic, (void *)this, 0, &tid);

	ACQUIRE_DRAWING_MUTEX(hDrawMutex)
	// We want to draw...
	shouldDraw = TRUE;
	RELEASE_DRAWING_MUTEX(hDrawMutex)
}

unsigned CCanvas::threadQueueDrawStatic(void* param)
{
	return ((CCanvas*)param)->threadQueueDraw();
}

#define DRAW_INTERVAL 16L
unsigned CCanvas::threadQueueDraw()
{
	while (repeat) {

		Sleep(DRAW_INTERVAL);

		ACQUIRE_DRAWING_MUTEX(hDrawMutex)
		if (!isDrawingSuspended && shouldDraw) {
			shouldDraw = FALSE;
			SDHostMessage(SD_REDRAW, objID , NULL);	
		}
		RELEASE_DRAWING_MUTEX(hDrawMutex)
	}

	_endthreadex(0);

	return 0;
}

void CCanvas::killthreadQueueDraw()
{
	if (hThread == NULL)
		return;

	repeat = FALSE;
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	hThread = NULL;
};

void CCanvas::resize()
{
	SD_SOP_INFO info;
	info.width = config->width;
	info.height = config->height;
	info.flags = SD_SOP_SIZE;
	SDHostMessage(SD_SET_OBJECT_POS, objID, (DWORD) &info);

	queueDraw();
}

void CCanvas::setObjID(DWORD id)
{
	objID = id;
}

void CCanvas::clearContext()
{
	destroyCairo();
	initCairo();
}

cairo_surface_t* CCanvas::getSurface()
{
	return surface;
}

int CCanvas::getWidth()
{
	return config->width;
}

int CCanvas::getHeight()
{
	return config->height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvas::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICanvas
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ICanvas
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvas::put_width(long width)
{
	// if value is invalid, we use the default value
	if (width < 0)
		config->width = DEFAULT_WIDTH;
	else
		config->width = width;

	logFile->Write("New width: %d", config->width);

	clearContext();
	resize();

	return S_OK;
}

STDMETHODIMP CCanvas::get_width(long *width)
{
	*width = config->width;

	return S_OK;
}

STDMETHODIMP CCanvas::put_height(long height)
{
	// if value is invalid, we use the default value
	if (height < 0)
		config->height = DEFAULT_HEIGHT;
	else
		config->height = height;

	logFile->Write("New height: %d", config->height);

	clearContext();
	resize();

	return S_OK;
}

STDMETHODIMP CCanvas::get_height(long *height)
{
	*height = config->height;

	return S_OK;
}

STDMETHODIMP CCanvas::put_debugMode(VARIANT_BOOL isDebugMode)
{
	if (isDebugMode == VARIANT_FALSE) {
		logFile->Write("Debug mode: OFF");
		debugMode = FALSE;
	} else {
		logFile->Write("Debug mode: ON");
		debugMode = TRUE;
	}

	return S_OK;
}

STDMETHODIMP CCanvas::get_debugMode(VARIANT_BOOL* isDebugMode)
{
	*isDebugMode = (debugMode ? VARIANT_TRUE : VARIANT_FALSE);

	return S_OK;
}

STDMETHODIMP CCanvas::suspendDrawing()
{
	ACQUIRE_DRAWING_MUTEX(hDrawMutex)

	isDrawingSuspended = TRUE;

	RELEASE_DRAWING_MUTEX(hDrawMutex)

	return S_OK;
}

STDMETHODIMP CCanvas::resumeDrawing()
{
	ACQUIRE_DRAWING_MUTEX(hDrawMutex)

	isDrawingSuspended = FALSE;

	SDHostMessage(SD_REDRAW, objID , NULL);	

	RELEASE_DRAWING_MUTEX(hDrawMutex)

	return S_OK;
}

STDMETHODIMP CCanvas::getContext(BSTR type, ICanvasRenderingContext2D **ctx)
{
	logFile->Write("Returning a new context object");

	if (context == NULL) {
		logFile->Write("[ERROR] Cairo context is null, cannot return a valid context object!");
		*ctx = NULL;
		return S_OK;
	}

	// If context not supported, we return null
	if (CComBSTR(type) != CComBSTR("2d")) {
		logFile->Write("[INFO] This type of context is not supported. Only context of type '2d' is supported presently.");
		*ctx = NULL;
		return S_OK;
	}

	// Only one context can be associated to a canvas at the same time
	CComObject<CCanvasRenderingContext2D>* pContext2d;
	CComObject<CCanvasRenderingContext2D>::CreateInstance(&pContext2d);
	pContext2d->setCanvas(this);

	pContext2d->QueryInterface(IID_ICanvasRenderingContext2D, (void**)ctx);
	
	return S_OK;
}

STDMETHODIMP CCanvas::toImage(BSTR path, VARIANT_BOOL* result)
{
	*result = VARIANT_FALSE;

	if (!surface)
		return S_OK;

#ifndef CAIRO_HAS_PNG_FUNCTIONS
	return NOT_IMPLEMENTED;
#endif

	if (CComBSTR(path) == CComBSTR(""))
		return CCOMError::DispatchError(E_FAIL, CLSID_Canvas, _T("Path is not valid"), "You need to provide a valid path!", 0, NULL);

	USES_CONVERSION;
	cairo_status_t status = cairo_surface_write_to_png(surface, OLE2T(path));

	switch (status)
	{
	case CAIRO_STATUS_SUCCESS:
		*result = VARIANT_TRUE;
		break;

	default:
		logFile->Write("[ERROR] Error writing image: %s", path);
	}

	return S_OK;
}