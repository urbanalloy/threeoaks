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

#pragma once

#include "stdafx.h"
#include "DXCanvas.h"
#include "resource.h"  
#include <comsvcs.h>

#include <cairo.h>
#ifdef ENABLE_PANGO
#include <pango/pangocairo.h>
#include <pango/pangowin32.h>
#endif

#include "CanvasGradient.h"
#include "CanvasPattern.h"
#include "CanvasImage.h"
#include "CanvasPixelArray.h"
#include "CanvasImageData.h"
#include "CanvasTextMetrics.h"

#include "CanvasState.h"
#include "CanvasShadow.h"
#include "CSSColorParser.h"

#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////////
// Error Management
#define CHECK_POSITIVE_VALUE(value) \
	if (value < 0) return CCOMError::DispatchError(E_FAIL, CLSID_CanvasRenderingContext2D, _T("Value is invalid"), __FUNCTION__ ": negative values are forbidden (value: " #value ")", 0, NULL);

#define CHECK_POSITIVE_VALUES(value1, value2) \
	CHECK_POSITIVE_VALUE(value1); CHECK_POSITIVE_VALUE(value2);

#define NOT_IMPLEMENTED \
	CCOMError::DispatchError(E_FAIL, CLSID_CanvasRenderingContext2D, _T("Method not implemented"), "This method is not implemented yet: " __FUNCTION__, 0, NULL)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Drawing Mutex
#define USE_DRAWING_MUTEX 1
#ifdef USE_DRAWING_MUTEX
#define ACQUIRE_DRAWING_MUTEX(mutex) \
	DWORD dwWaitMutex = WaitForSingleObject(mutex, INFINITE); \
	switch (dwWaitMutex) \
	{ \
		case WAIT_OBJECT_0: \
		{

#define RELEASE_DRAWING_MUTEX(mutex) \
		ReleaseMutex(mutex); \
		break; \
		} \
	default: \
		break; \
	}
#else
#define ACQUIRE_DRAWING_MUTEX(mutex)
#define RELEASE_DRAWING_MUTEX(mutex)
#endif

//////////////////////////////////////////////////////////////////////////
// Needed for the back reference to the canvas
class CCanvas;

class ATL_NO_VTABLE CCanvasRenderingContext2D :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvasRenderingContext2D, &CLSID_CanvasRenderingContext2D>,
	public IDispatchImpl<ICanvasRenderingContext2D, &IID_ICanvasRenderingContext2D, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
public:
	CCanvasRenderingContext2D() 
		: canvas(NULL),
#ifdef ENABLE_PANGO
		  layout(NULL),
#endif
		  shadow(NULL)
	{}	

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{	
		return S_OK;
	}

	void FinalRelease() 
	{	
		SAFE_DELETE(shadow);
#ifdef ENABLE_PANGO
		/* free the layout object */
		if (layout)
			g_object_unref (layout);
#endif
	}

DECLARE_NOT_AGGREGATABLE(CCanvasRenderingContext2D)

DECLARE_REGISTRY_RESOURCEID(IDR_CANVASRENDERINGCONTEXT2D)

BEGIN_COM_MAP(CCanvasRenderingContext2D)
	COM_INTERFACE_ENTRY(ICanvasRenderingContext2D)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:

	// text operation
	enum TextOperation {
		TEXT_OPERATION_FILL,
		TEXT_OPERATION_STROKE,
		TEXT_OPERATION_MEASURE,
		TEXT_OPERATION_PATH,
		TEXT_OPERATION_PATH_STROKE,
		TEXT_OPERATION_PATH_FILL
	};

	CCanvas* canvas;
#ifdef ENABLE_PANGO
	PangoLayout* layout;
#endif
	CanvasShadow* shadow;
	
	CanvasState::State& currentState();
	
	cairo_pattern_t* createNewPatternFromSurface(cairo_surface_t* surface, int width, int height);

	HRESULT processText(BSTR text, float x, float y, VARIANT maxWidth, TextOperation operation, float* width);

	HRESULT drawRect(float x, float y, float w, float h, Style style);
	void drawPath(Style style);

public:

	void setCanvas(CCanvas* canvas);

	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvasGradient
	//////////////////////////////////////////////////////////////////////////

	////////////////
	// Attributes //
	////////////////

	// compositing
	STDMETHOD(put_globalAlpha)(float alpha);
	STDMETHOD(get_globalAlpha)(float* alpha);
	STDMETHOD(put_globalCompositeOperation)(BSTR operation);
	STDMETHOD(get_globalCompositeOperation)(BSTR* operation);

	// colors and styles
	STDMETHOD(put_strokeStyle)(VARIANT style);
	STDMETHOD(get_strokeStyle)(VARIANT* style);
	STDMETHOD(put_fillStyle)(VARIANT style);
	STDMETHOD(get_fillStyle)(VARIANT* style);

	// line caps/joins
	STDMETHOD(put_lineWidth)(float width);
	STDMETHOD(get_lineWidth)(float* width);
	STDMETHOD(put_lineCap)(BSTR cap);
	STDMETHOD(get_lineCap)(BSTR* cap);
	STDMETHOD(put_lineJoin)(BSTR join);
	STDMETHOD(get_lineJoin)(BSTR* join);	
	STDMETHOD(put_miterLimit)(float limit);
	STDMETHOD(get_miterLimit)(float* limit);

	// shadows
	STDMETHOD(put_shadowOffsetX)(float offset);
	STDMETHOD(get_shadowOffsetX)(float* offset);
	STDMETHOD(put_shadowOffsetY)(float offset);
	STDMETHOD(get_shadowOffsetY)(float* offset);
	STDMETHOD(put_shadowBlur)(float size);
	STDMETHOD(get_shadowBlur)(float* size);
	STDMETHOD(put_shadowColor)(VARIANT color);
	STDMETHOD(get_shadowColor)(VARIANT* color);

	// text
	STDMETHOD(put_font)(BSTR font);
	STDMETHOD(get_font)(BSTR* font);	 
	STDMETHOD(put_textAlign)(BSTR align);
	STDMETHOD(get_textAlign)(BSTR* align);	
	STDMETHOD(put_textBaseline)(BSTR baseline);
	STDMETHOD(get_textBaseline)(BSTR* baseline);	

	STDMETHOD(get_canvas)(ICanvas** canvas);

	/////////////
	// Methods //
	/////////////

	// state
	STDMETHOD(save)();
	STDMETHOD(restore)();

	// transformations (default transform is the identity matrix)
	STDMETHOD(scale)(float x, float y);
	STDMETHOD(rotate)(float angle);
	STDMETHOD(translate)(float x, float y);
	STDMETHOD(transform)(float m11, float m12, float m21, float m22, float dx, float dy);
	STDMETHOD(setTransform)(float m11, float m12, float m21, float m22, float dx, float dy);

	// colors and styles
	STDMETHOD(createLinearGradient)(float x0, float y0, float x1, float y1, ICanvasGradient** gradient);
	STDMETHOD(createRadialGradient)(float x0, float y0, float r0, float x1, float y1, float r1, ICanvasGradient** gradient);
	STDMETHOD(createPattern)(VARIANT image, BSTR repeat, ICanvasPattern** pattern);

	// rects
	STDMETHOD(clearRect)(float x, float y, float w, float h);
	STDMETHOD(fillRect)(float x, float y, float w, float h);
	STDMETHOD(strokeRect)(float x, float y, float w, float h);

	// paths
	STDMETHOD(beginPath)();
	STDMETHOD(closePath)();
	STDMETHOD(moveTo)(float x, float y);
	STDMETHOD(lineTo)(float x, float y);	
	STDMETHOD(quadraticCurveTo)(float cpx, float cpy, float x, float y);
	STDMETHOD(bezierCurveTo)(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);	
	STDMETHOD(arcTo)(float x1, float y1, float x2, float y2, float radius);
	STDMETHOD(rect)(double x, double y, double width, double height);
	STDMETHOD(arc)(float x, float y, float radius, float startAngle, float endAngle, VARIANT_BOOL anticlockwise);	
	STDMETHOD(fill)();
	STDMETHOD(stroke)();
	STDMETHOD(clip)();
	STDMETHOD(isPointInPath)(float x, float y, VARIANT_BOOL* isPresent);

	// text
	STDMETHOD(fillText)(BSTR text, float x, float y, VARIANT maxWidth);	
	STDMETHOD(strokeText)(BSTR text, float x, float y, VARIANT maxWidth);	
	STDMETHOD(textAlongPath)(BSTR text, BOOL stroke);
	STDMETHOD(pathText)(BSTR text);

	STDMETHOD(measureText)(BSTR text, ICanvasTextMetrics** metrics);
	
	// drawing images
	STDMETHOD(loadImage)(BSTR path, ICanvasImage** image);
	STDMETHOD(drawImage)(VARIANT input, float dx, float dy, VARIANT dw, VARIANT dh);
	STDMETHOD(drawImageRegion)(VARIANT input, float sx, float sy, float sw, float sh, float dx, float dy, VARIANT dw, VARIANT dh);

	// pixel manipulation
	STDMETHOD(createImageData)(float sw, float sh, ICanvasImageData** data);
	STDMETHOD(getImageData)(float sx, float sy, float sw, float sh, ICanvasImageData** data);
	STDMETHOD(putImageData)(ICanvasImageData* CanvasImageData, float dx, float dy, VARIANT dirtyX, VARIANT dirtyY, VARIANT dirtyWidth, VARIANT dirtyHeight);

};

OBJECT_ENTRY_AUTO(__uuidof(CanvasRenderingContext2D), CCanvasRenderingContext2D)
