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

#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
using namespace std;

#include "Canvas.h"
#include "CanvasRenderingContext2D.h"
#include "COMError.h"

#include "CanvasPath.h"

void CCanvasRenderingContext2D::setCanvas(CCanvas* canvas)
{
	this->canvas = canvas;

#ifdef ENABLE_PANGO
	g_type_init();

	// Create a new layout
	if (!layout)
		layout = pango_cairo_create_layout(canvas->context);
#endif

	if (!shadow)
		shadow = new CanvasShadow();
}

//////////////////////////////////////////////////////////////////////////
// Styles and state
CanvasState::State& CCanvasRenderingContext2D::currentState()
{
	return canvas->state->currentState();
}


//////////////////////////////////////////////////////////////////////////
// Text
HRESULT CCanvasRenderingContext2D::processText(BSTR txt, float x, float y, VARIANT maxW, TextOperation operation, float* width)
{
#ifdef ENABLE_PANGO
	float maxWidth = -1;

	// Check MAX_WIDTH
	if (maxW.vt != VT_ERROR && maxW.vt == VT_I4)
	{
		if (maxW.lVal <= 0)
			return CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Parameter error"), __FUNCTION__ ": maxWidth should be strictly positive", 0, NULL);

		maxWidth = (float)maxW.lVal;
	}

	USES_CONVERSION;
	string text(OLE2T(txt));

	// Replace all the whitespace characters (with U+0020 SPACE)
	replace(text.begin(), text.end(), '\x09', '\x20');	// character tabulation
	replace(text.begin(), text.end(), '\x0A', '\x20'); // line feed
	replace(text.begin(), text.end(), '\x0C', '\x20'); // form feed
	replace(text.begin(), text.end(), '\x0D', '\x20'); // carriage return

	// Set the font and text
	pango_layout_set_text(layout, text.c_str(), -1);

	PangoFontDescription* desc = pango_font_description_from_string(currentState().fontDescription.c_str());
	pango_layout_set_font_description(layout, desc);

	PangoContext* pangoContext = pango_layout_get_context(layout);
	PangoLanguage* language = pango_context_get_language(pangoContext);

	PangoFontMetrics* metrics = pango_context_get_metrics(pangoContext, desc, language);
	int fontAscent = pango_font_metrics_get_ascent(metrics)/PANGO_SCALE;
	int fontDescent = pango_font_metrics_get_descent(metrics)/PANGO_SCALE;

	// cleanup
	pango_font_metrics_unref(metrics);
	pango_font_description_free(desc);

	// Measure the text
	int mWidth, mHeight;
	pango_cairo_update_layout (canvas->context, layout);
	pango_layout_get_size (layout, &mWidth, &mHeight);

	mWidth /= PANGO_SCALE;
	mHeight /= PANGO_SCALE;

	if (operation == TEXT_OPERATION_MEASURE)
	{
		if (width != NULL)
			*width = (float)mWidth;

		return S_OK;
	}

	float anchorX = 0, anchorY = 0;
	//////////////////////////////////////////////////////////////////////////
	// Text alignment
	switch (currentState().textAlign)
	{
		case TEXT_ALIGN_START:
			anchorX = 0;
			break;
		case TEXT_ALIGN_END:
			anchorX = 1;
			break;
		case TEXT_ALIGN_LEFT:
			anchorX = 0;
			break;
		case TEXT_ALIGN_RIGHT:
			anchorX = 1;
			break;
		case TEXT_ALIGN_CENTER:
			anchorX = 0.5;
			break;
		default:
			return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Internal error"), __FUNCTION__ ": invalid value for text baseline", 0, NULL);
	}

	//////////////////////////////////////////////////////////////////////////
	// Text baseline
	switch (currentState().textBaseline)
	{
		case TEXT_BASELINE_TOP:			// top of the em square
			anchorY = (float)fontAscent;
			break;
		case TEXT_BASELINE_HANGING:
			//anchorY = 0;
			break;
		case TEXT_BASELINE_MIDDLE:		// middle of the em square
			anchorY = (float)(fontAscent - fontDescent) * 0.5f;
			break;
		case TEXT_BASELINE_ALPHABETIC:
			anchorY = 0;
			break;
		case TEXT_BASELINE_IDEOGRAPHIC:
			//anchorY = 0;
			break;
		case TEXT_BASELINE_BOTTOM:		// bottom of the em square
			anchorY = -(float)fontDescent;
			break;
		default:
			return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Internal error"), __FUNCTION__ ": invalid value for text baseline", 0, NULL);
	}

	// Text position
	x -= anchorX * mWidth;
	y += anchorY - (float)fontAscent;	// our text box origin should be the bottom left corner, so we remove the box height

	cairo_save(canvas->context);

	// if text is over maxWidth, then scale the text horizontally such that its width is precisely maxWidth
	if (maxWidth != -1 && mWidth > maxWidth) {

		// translate the anchor point to 0, then scale and translate back
		cairo_translate(canvas->context, x, 0);
		cairo_scale(canvas->context, maxWidth/mWidth, 1);
		cairo_translate(canvas->context, -x, 0);
	}

	// Apply styles & draw text
	if (operation == TEXT_OPERATION_STROKE)
	{
		cairo_path_t* path = cairo_copy_path(canvas->context);

		cairo_new_path(canvas->context);
		cairo_move_to(canvas->context, x, y);

		pango_cairo_update_layout(canvas->context, layout);
		pango_cairo_layout_path(canvas->context, layout);

		drawPath(STYLE_STROKE);

		// Restore the path
		cairo_new_path(canvas->context);
		cairo_append_path(canvas->context, path);
	}
	else if (operation == TEXT_OPERATION_FILL)
	{
		// Draw shadow if needed
		if (canvas->state->isShadowVisible())
		{
			cairo_path_t* path = cairo_copy_path(canvas->context);

			cairo_new_path(canvas->context);
			cairo_move_to(canvas->context, x, y);

			pango_cairo_update_layout(canvas->context, layout);
			pango_cairo_layout_path(canvas->context, layout);

			drawPath(STYLE_FILL);

			// Restore the path
			cairo_new_path(canvas->context);
			cairo_append_path(canvas->context, path);
		} else {
			cairo_move_to(canvas->context, x, y);
			pango_cairo_update_layout(canvas->context, layout);

			canvas->state->applyStyle(STYLE_FILL);
			pango_cairo_show_layout(canvas->context, layout);
		}
	}
	else if (operation == TEXT_OPERATION_PATH)
	{
		pango_cairo_update_layout(canvas->context, layout);
		pango_cairo_layout_path(canvas->context, layout);
	}
	else if (operation == TEXT_OPERATION_PATH_STROKE || operation == TEXT_OPERATION_PATH_FILL)
	{
		cairo_path_t* oldPath = cairo_copy_path(canvas->context);

		/* Decrease tolerance a bit, since it's going to be magnified */
		cairo_set_tolerance (canvas->context, 0.01);

		/* Using cairo_copy_path() here shows our deficiency in handling
		 * Bezier curves, specially around sharper curves.
		 *
		 * Using cairo_copy_path_flat() on the other hand, magnifies the
		 * flattening error with large off-path values. We decreased
		 * tolerance for that reason. Increase tolerance to see that
		 * artifact.
		 */
		cairo_path_t* path = cairo_copy_path_flat(canvas->context);

		cairo_new_path(canvas->context);

		pango_cairo_update_layout(canvas->context, layout);

		PangoLayoutLine *line = pango_layout_get_line_readonly(layout, 0);
		pango_cairo_layout_line_path(canvas->context, line);

		CanvasPath::map_path_onto(canvas->context, path, x, y);

		cairo_path_destroy(path);

		if (operation == TEXT_OPERATION_PATH_FILL)
			drawPath(STYLE_FILL);
		else
			drawPath(STYLE_STROKE);

		// Restore the path
		cairo_new_path(canvas->context);
		cairo_append_path(canvas->context, oldPath);
	}
	else
	{
		// Uh oh, FAIL!
		cairo_restore(canvas->context);
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Internal error"), __FUNCTION__ ": invalid text operation type", 0, NULL);
	}

	cairo_restore(canvas->context);

	canvas->queueDraw();

	return S_OK;
#else
	return CCOMError::DispatchError(NOT_SUPPORTED_ERR, CLSID_CanvasRenderingContext2D, _T("Internal error"), __FUNCTION__ ": text rendering is not enabled", 0, NULL);
#endif
}

//////////////////////////////////////////////////////////////////////////
// Patterns
cairo_pattern_t* CCanvasRenderingContext2D::createNewPatternFromSurface(cairo_surface_t* surface, int width, int height)
{
	if (width == 0 && height == 0)
		return NULL;

	// Copy the content of the surface to a new cairo surface
	cairo_surface_t* newSurface = cairo_win32_surface_create_with_dib(CAIRO_FORMAT_ARGB32, width, height);

	HDC surfaceDC = cairo_win32_surface_get_dc(surface);
	HDC newHDC = cairo_win32_surface_get_dc(newSurface);

	BitBlt(newHDC, 0, 0, width, height, surfaceDC, 0, 0, SRCCOPY);

	cairo_pattern_t* pattern = cairo_pattern_create_for_surface(newSurface);

	if (cairo_pattern_status(pattern) != CAIRO_STATUS_SUCCESS)
		return NULL;

	return pattern;
}

//////////////////////////////////////////////////////////////////////////
// Rect and paths (with support for shadows)
HRESULT CCanvasRenderingContext2D::drawRect(float x, float y, float w, float h, Style style)
{
	cairo_path_t* current_path = cairo_copy_path(canvas->context);

	// Create a new path
	cairo_new_path(canvas->context);
	cairo_rectangle(canvas->context, x, y, w, h);

	drawPath(style);

	// Restore previous path
	cairo_new_path(canvas->context);
	if (current_path->status == CAIRO_STATUS_SUCCESS && current_path->num_data != 0)
		cairo_append_path(canvas->context, current_path);

	cairo_path_destroy(current_path);

	canvas->queueDraw();

	return S_OK;
}

void CCanvasRenderingContext2D::drawPath(Style style)
{
	// invalid style
	if (style == STYLE_SHADOW)
		return;

	ACQUIRE_DRAWING_MUTEX(canvas->hDrawMutex)

	// Draw shadow if needed
	if (canvas->state->isShadowVisible())
	{
		//////////////////////////////////////////////////////////////////////////
		// Calculate extents
		//  - use an identity matrix
		//  - extents is affected by the style of drawing we need to do
		cairo_matrix_t savedMatrix;
		cairo_get_matrix(canvas->context, &savedMatrix);
		cairo_identity_matrix(canvas->context);

		EXTENTS_RECT extents;

		if (style == STYLE_FILL)
			cairo_fill_extents(canvas->context, &extents.x1, &extents.y1, &extents.x2, &extents.y2);
		else // STYLE_STROKE
			cairo_stroke_extents(canvas->context, &extents.x1, &extents.y1, &extents.x2, &extents.y2);

		// restore saved matrix
		cairo_set_matrix(canvas->context, &savedMatrix);

		//////////////////////////////////////////////////////////////////////////
		// Prepare to draw shadows
		canvas->state->applyStyle(style);

		// the context is owned by canvasShadow and will be cleaned up when draw is called
		cairo_t* ctx = shadow->getContext(canvas->context, canvas->state, extents);

		if (ctx)
		{
			// Draw into the shadow context
			if (style == STYLE_FILL)
				cairo_fill(ctx);
			else
				cairo_stroke(ctx);

			// Draw the shadow
			canvas->state->applyStyle(STYLE_SHADOW);
			shadow->draw();
		}
	}

	if (!canvas->state->isGlobalAlphaHandled(style))
	{
		cairo_path_t* current_path = cairo_copy_path(canvas->context);

		cairo_push_group_with_content(canvas->context, CAIRO_CONTENT_COLOR_ALPHA);

		// Restore previous path
		cairo_new_path(canvas->context);
		if (current_path->status == CAIRO_STATUS_SUCCESS && current_path->num_data != 0)
			cairo_append_path(canvas->context, current_path);

		cairo_path_destroy(current_path);

		cairo_set_operator(canvas->context, CAIRO_OPERATOR_SOURCE);
	}

	canvas->state->applyStyle(style);

	if (style == STYLE_FILL)
		cairo_fill_preserve(canvas->context);
	else
		cairo_stroke_preserve(canvas->context);

	if (!canvas->state->isGlobalAlphaHandled(style))
	{
		cairo_pop_group_to_source(canvas->context);

		cairo_paint_with_alpha(canvas->context, canvas->state->isColor(style) ? 1.0 : currentState().globalAlpha);
	}

	RELEASE_DRAWING_MUTEX(canvas->hDrawMutex)
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasRenderingContext2D::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ICanvasRenderingContext2D
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

///////////////////////////////////
//			Attributes			 //
///////////////////////////////////

// compositing
STDMETHODIMP CCanvasRenderingContext2D::put_globalAlpha(float alpha)
{
	// Value must be in range 0.0 (fully transparent) - 1.0 (no additional transparency)
	if (alpha < 0.0 || alpha > 1.0)
		return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid value"), __FUNCTION__ ": invalid value: global alpha should be between 0.0 and 1.0", 0, NULL) : S_OK);

	currentState().globalAlpha = alpha;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_globalAlpha(float* alpha)
{
	*alpha = currentState().globalAlpha;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_globalCompositeOperation(BSTR operation)
{
	cairo_operator_t cairo_op;

#define IS_OPERATION(name, type) \
	if (CComBSTR(operation) == CComBSTR(name)) \
		cairo_op = CAIRO_OPERATOR_##type; \
	else \

	IS_OPERATION("source-atop", ATOP)
	IS_OPERATION("source-in", IN)
	IS_OPERATION("source-out", OUT)
	IS_OPERATION("source-over", OVER)
	IS_OPERATION("destination-atop", DEST_ATOP)
	IS_OPERATION("destination-in", DEST_IN)
	IS_OPERATION("destination-out", DEST_OUT)
	IS_OPERATION("destination-over", DEST_OVER)
	IS_OPERATION("lighter", ADD)
	IS_OPERATION("copy", SOURCE)
	IS_OPERATION("xor", XOR)
	return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid operation"), __FUNCTION__ ": invalid type of operation", 0, NULL) : S_OK);

	cairo_set_operator(canvas->context, cairo_op);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_globalCompositeOperation(BSTR* operation)
{
	cairo_operator_t cairo_op = cairo_get_operator(canvas->context);

#undef IS_OPERATION
#define IS_OPERATION(name, type) \
	if (cairo_op == CAIRO_OPERATOR_##type) {\
		CComBSTR bstr(name); \
		*operation = bstr.Detach(); \
	}

	IS_OPERATION("source-atop", ATOP)
	IS_OPERATION("source-in", IN)
	IS_OPERATION("source-out", OUT)
	IS_OPERATION("source-over", OVER)
	IS_OPERATION("destination-atop", DEST_ATOP)
	IS_OPERATION("destination-in", DEST_IN)
	IS_OPERATION("destination-out", DEST_OUT)
	IS_OPERATION("destination-over", DEST_OVER)
	IS_OPERATION("lighter", ADD)
	IS_OPERATION("copy", SOURCE)
	IS_OPERATION("xor", XOR)

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// colors and styles
STDMETHODIMP CCanvasRenderingContext2D::put_strokeStyle(VARIANT style)
{
	return canvas->state->setStyle(style, STYLE_STROKE);
}

STDMETHODIMP CCanvasRenderingContext2D::get_strokeStyle(VARIANT* style)
{
	return canvas->state->getStyle(style, STYLE_STROKE);
}

STDMETHODIMP CCanvasRenderingContext2D::put_fillStyle(VARIANT style)
{
	return canvas->state->setStyle(style, STYLE_FILL);
}

STDMETHODIMP CCanvasRenderingContext2D::get_fillStyle(VARIANT* style)
{
	return canvas->state->getStyle(style, STYLE_FILL);
}

//////////////////////////////////////////////////////////////////////////
// line caps/joins
STDMETHODIMP CCanvasRenderingContext2D::put_lineWidth(float width)
{
	if (width <= 0)
		return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid operation"), __FUNCTION__ ": line width should be positive", 0, NULL) : S_OK);

	cairo_set_line_width(canvas->context, width);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_lineWidth(float* width)
{
	*width = (float)cairo_get_line_width(canvas->context);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_lineCap(BSTR cap)
{
	cairo_line_cap_t cairo_cap;

#define IS_LINECAP(name, type) \
	if (CComBSTR(cap) == CComBSTR(name)) \
		cairo_cap = CAIRO_LINE_CAP_##type; \
	else \

	IS_LINECAP("butt", BUTT)
	IS_LINECAP("round", ROUND)
	IS_LINECAP("square", SQUARE)
	return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid operation"), __FUNCTION__ ": invalid type of line cap", 0, NULL) : S_OK);

	cairo_set_line_cap(canvas->context, cairo_cap);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_lineCap(BSTR* cap)
{
	cairo_line_cap_t cairo_cap = cairo_get_line_cap(canvas->context);

#undef IS_LINECAP
#define IS_LINECAP(name, type) \
	if (cairo_cap == CAIRO_LINE_CAP_##type) {\
		CComBSTR bstr(name); \
		*cap = bstr.Detach(); \
	}

	IS_LINECAP("butt", BUTT)
	IS_LINECAP("round", ROUND)
	IS_LINECAP("square", SQUARE)

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_lineJoin(BSTR join)
{
	cairo_line_join_t cairo_join;

#define IS_LINEJOIN(name, type) \
	if (CComBSTR(join) == CComBSTR(name)) \
		cairo_join = CAIRO_LINE_JOIN_##type; \
	else \

	IS_LINEJOIN("bevel", BEVEL)
	IS_LINEJOIN("round", ROUND)
	IS_LINEJOIN("miter", MITER)
	return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid operation"), __FUNCTION__ ": invalid type of line join", 0, NULL) : S_OK);

	cairo_set_line_join(canvas->context, cairo_join);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_lineJoin(BSTR* join)
{
	cairo_line_join_t cairo_join = cairo_get_line_join(canvas->context);

#undef IS_LINEJOIN
#define IS_LINEJOIN(name, type) \
	if (cairo_join == CAIRO_LINE_JOIN_##type) {\
		CComBSTR bstr(name); \
		*join = bstr.Detach(); \
	}

	IS_LINEJOIN("bevel", BEVEL)
	IS_LINEJOIN("round", ROUND)
	IS_LINEJOIN("miter", MITER)

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_miterLimit(float limit)
{
	if (limit <= 0)
		return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid operation"), __FUNCTION__ ": miter limit should be positive", 0, NULL) : S_OK);

	cairo_set_miter_limit(canvas->context, limit);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_miterLimit(float* limit)
{
	*limit = (float)cairo_get_miter_limit(canvas->context);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// shadows
STDMETHODIMP CCanvasRenderingContext2D::put_shadowOffsetX(float offset)
{
	currentState().shadowOffsetX = offset;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_shadowOffsetX(float* offset)
{
	*offset = currentState().shadowOffsetX;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_shadowOffsetY(float offset)
{
	currentState().shadowOffsetY = offset;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_shadowOffsetY(float* offset)
{
	*offset = currentState().shadowOffsetY;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_shadowBlur(float size)
{
	if (size < 0)
		return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid operation"), __FUNCTION__ ": shadow blur should be positive or 0", 0, NULL) : S_OK);

	currentState().shadowBlur = size;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_shadowBlur(float* size)
{
	*size = currentState().shadowBlur;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_shadowColor(VARIANT color)
{
	return canvas->state->setStyle(color, STYLE_SHADOW);
}

STDMETHODIMP CCanvasRenderingContext2D::get_shadowColor(VARIANT* color)
{
	return canvas->state->getStyle(color, STYLE_SHADOW);
}

//////////////////////////////////////////////////////////////////////////
// text
STDMETHODIMP CCanvasRenderingContext2D::put_font(BSTR font)
{
	USES_CONVERSION;

	string strFont(OLE2T(font));

	// Check font validity
	PangoFontDescription* desc = pango_font_description_from_string(strFont.c_str());

	int size = pango_font_description_get_size(desc);
	const char* family = pango_font_description_get_family(desc);

	pango_font_description_free(desc);

	if (size == 0 || family == NULL)
	{
		currentState().fontDescription = string(DEFAULT_FONT);

		if (canvas->debugMode)
		{
			if (family == NULL)
				return CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid font"), __FUNCTION__ ": the font family is missing!", 0, NULL);
			else
				return CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid font"), __FUNCTION__ ": the font size is invalid or missing!", 0, NULL);
		}
		else
			return S_OK;
	}

	currentState().fontDescription = strFont;

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_font(BSTR* font)
{
	CComBSTR strFont(currentState().fontDescription.c_str());

	*font = strFont.Detach();

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_textAlign(BSTR align)
{
#define IS_TEXT_ALIGN(name, type) \
	if (CComBSTR(align) == CComBSTR(name)) \
		currentState().textAlign = type; \
	else \

	IS_TEXT_ALIGN("start", TEXT_ALIGN_START)
	IS_TEXT_ALIGN("end", TEXT_ALIGN_END)
	IS_TEXT_ALIGN("left", TEXT_ALIGN_LEFT)
	IS_TEXT_ALIGN("right", TEXT_ALIGN_RIGHT)
	IS_TEXT_ALIGN("center", TEXT_ALIGN_CENTER)
	return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid text alignment"), __FUNCTION__ ": invalid type of alignment", 0, NULL) : S_OK);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_textAlign(BSTR* align)
{
#undef IS_TEXT_ALIGN
#define IS_TEXT_ALIGN(name, type) \
	if (currentState().textAlign == type) {\
		CComBSTR bstr(name); \
		*align = bstr.Detach(); \
	}

	IS_TEXT_ALIGN("start", TEXT_ALIGN_START);
	IS_TEXT_ALIGN("end", TEXT_ALIGN_END);
	IS_TEXT_ALIGN("left", TEXT_ALIGN_LEFT);
	IS_TEXT_ALIGN("right", TEXT_ALIGN_RIGHT);
	IS_TEXT_ALIGN("center", TEXT_ALIGN_CENTER);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::put_textBaseline(BSTR baseline)
{

#define IS_TEXT_BASELINE(name, type) \
	if (CComBSTR(baseline) == CComBSTR(name)) \
		currentState().textBaseline = type; \
	else \

	IS_TEXT_BASELINE("top", TEXT_BASELINE_TOP)
	IS_TEXT_BASELINE("hanging", TEXT_BASELINE_HANGING)
	IS_TEXT_BASELINE("middle", TEXT_BASELINE_MIDDLE)
	IS_TEXT_BASELINE("alphabetic", TEXT_BASELINE_ALPHABETIC)
	IS_TEXT_BASELINE("ideographic", TEXT_BASELINE_IDEOGRAPHIC)
	IS_TEXT_BASELINE("bottom", TEXT_BASELINE_BOTTOM)
	return (canvas->debugMode ? CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid text baseline"), __FUNCTION__ ": invalid type of baseline", 0, NULL) : S_OK);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_textBaseline(BSTR* baseline)
{
#undef IS_TEXT_BASELINE
#define IS_TEXT_BASELINE(name, type) \
	if (currentState().textBaseline == type) {\
		CComBSTR bstr(name); \
		*baseline = bstr.Detach(); \
	}

	IS_TEXT_BASELINE("top", TEXT_BASELINE_TOP);
	IS_TEXT_BASELINE("hanging", TEXT_BASELINE_HANGING);
	IS_TEXT_BASELINE("middle", TEXT_BASELINE_MIDDLE);
	IS_TEXT_BASELINE("alphabetic", TEXT_BASELINE_ALPHABETIC);
	IS_TEXT_BASELINE("ideographic", TEXT_BASELINE_IDEOGRAPHIC);
	IS_TEXT_BASELINE("bottom", TEXT_BASELINE_BOTTOM);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::get_canvas(ICanvas** canvas)
{
	this->canvas->QueryInterface(IID_ICanvas, (void**)canvas);

	return S_OK;
}

///////////////////////////////////
//			  Methods			 //
///////////////////////////////////

// state
STDMETHODIMP CCanvasRenderingContext2D::save()
{
	canvas->state->save();

	cairo_save(canvas->context);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::restore()
{
	if (canvas->state->restore())
		cairo_restore(canvas->context);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// transformations (default transform is the identity matrix)
STDMETHODIMP CCanvasRenderingContext2D::scale(float x, float y)
{
	cairo_scale(canvas->context, x, y);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::rotate(float angle)
{
	cairo_rotate(canvas->context, angle);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::translate(float x, float y)
{
	cairo_translate(canvas->context, x, y);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::transform(float m11, float m12, float m21, float m22, float dx, float dy)
{
	cairo_matrix_t matrix = { m11, m12, m21, m22, dx, dy};

	cairo_transform(canvas->context, &matrix);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::setTransform(float m11, float m12, float m21, float m22, float dx, float dy)
{
	cairo_matrix_t matrix = { m11, m12, m21, m22, dx, dy};

	cairo_set_matrix(canvas->context, &matrix);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// colors and styles
STDMETHODIMP CCanvasRenderingContext2D::createLinearGradient(float x0, float y0, float x1, float y1, ICanvasGradient** gradient)
{
	CComObject<CCanvasGradient>* pGradient;
	CComObject<CCanvasGradient>::CreateInstance(&pGradient);

	// we should not paint anything if we have a zero-length line
	if (x0 != x1 || y0 != y1)
		pGradient->setPattern(cairo_pattern_create_linear(x0, y0, x1, y1));

	pGradient->QueryInterface(IID_ICanvasGradient, (void**)gradient);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::createRadialGradient(float x0, float y0, float r0, float x1, float y1, float r1, ICanvasGradient** gradient)
{
	CHECK_POSITIVE_VALUES(r0, r1);

	CComObject<CCanvasGradient>* pGradient;
	CComObject<CCanvasGradient>::CreateInstance(&pGradient);

	if (x0 != x1 || y0 != y1 || r0 != r1)
		pGradient->setPattern(cairo_pattern_create_radial(x0, y0, r0, x1, y1, r1));

	pGradient->QueryInterface(IID_ICanvasGradient, (void**)gradient);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::createPattern(VARIANT vInput, BSTR repeat, ICanvasPattern** pattern)
{
	cairo_extend_t extend;
	cairo_pattern_t* cairoPattern = NULL;

	//////////////////////////////////////////////////////////////////////////
	// Check input

	// We only accept CCanvasImage and CCanvas
	if (vInput.vt != VT_DISPATCH && vInput.vt != 0x400c)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);

	VARIANT* input = &vInput;

	if (vInput.vt == 0x400c)
	{
		input = vInput.pvarVal;
		if (input->vt != VT_DISPATCH)
			return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);
	}

	//////////////////////////////////////////////////////////////////////////
	// CCanvas
	CCanvas* pCanvas;
	HRESULT result = input->pdispVal->QueryInterface(IID_ICanvas, (void**)&pCanvas);
	if (result != E_NOINTERFACE && pCanvas != NULL) {

		if (pCanvas->getWidth() == 0 || pCanvas->getHeight() == 0)
			return CCOMError::DispatchError(INVALID_STATE_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid canvas"), __FUNCTION__ ": canvas should have a width or a height different from 0", 0, NULL);

		cairoPattern = createNewPatternFromSurface(pCanvas->getSurface(), pCanvas->getWidth(), pCanvas->getHeight());

		goto pattern_loaded;
	}

	//////////////////////////////////////////////////////////////////////////
	// CCanvasImage
	CCanvasImage* pImage;
	result = input->pdispVal->QueryInterface(IID_ICanvasImage, (void**)&pImage);

	if (result == E_NOINTERFACE || pImage == NULL)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);

	cairoPattern = createNewPatternFromSurface(pImage->getSurface(), pImage->getWidth(), pImage->getHeight());


pattern_loaded:
	///////////////////////////////////////////////////////////////////
	// Check repeat value
#define IS_REPEAT(name, type) \
	if (CComBSTR(repeat) == CComBSTR(name)) \
		extend = type; \
	else \

	// NULL or "" defaults to CAIRO_EXTEND_REPEAT
	if (repeat == NULL || CComBSTR(repeat) == CComBSTR(""))
		extend = CAIRO_EXTEND_REPEAT;
	else
	IS_REPEAT("repeat", CAIRO_EXTEND_REPEAT)
	IS_REPEAT("repeat-x", CAIRO_EXTEND_REPEAT) // FIXME handle repeat-x case
	IS_REPEAT("repeat-y", CAIRO_EXTEND_REPEAT) // FIXME handle repeat-y case
	IS_REPEAT("no-repeat", CAIRO_EXTEND_NONE)
	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid repeat value"), __FUNCTION__ ": invalid type of repeat", 0, NULL);

	// Create a new CCanvasPattern instance
	CComObject<CCanvasPattern>* pPattern;
	CComObject<CCanvasPattern>::CreateInstance(&pPattern);
	if (pattern) {
		pPattern->setPattern(cairoPattern);
		pPattern->setExtend(extend);
	}

	pPattern->QueryInterface(IID_ICanvasPattern, (void**)pattern);

	return S_OK;
}

// rects
STDMETHODIMP CCanvasRenderingContext2D::clearRect(float x, float y, float w, float h)
{
	if (w == 0 || h == 0)
		return S_OK;

	cairo_path_t* current_path = cairo_copy_path(canvas->context);

	cairo_save(canvas->context);

	// Create new path to clear the rectangle
	cairo_new_path(canvas->context);
	cairo_rectangle(canvas->context, x, y, w, h);
	cairo_set_operator(canvas->context, CAIRO_OPERATOR_CLEAR);

	ACQUIRE_DRAWING_MUTEX(canvas->hDrawMutex)
	cairo_fill_preserve(canvas->context);
	RELEASE_DRAWING_MUTEX(canvas->hDrawMutex)

	cairo_restore(canvas->context);

	// Restore previous path
	cairo_new_path(canvas->context);
	if (current_path->status == CAIRO_STATUS_SUCCESS && current_path->num_data != 0)
		cairo_append_path(canvas->context, current_path);
	cairo_path_destroy(current_path);

	canvas->queueDraw();

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::fillRect(float x, float y, float w, float h)
{
	if (w == 0 || h == 0)
		return S_OK;

	return drawRect(x, y, w, h, STYLE_FILL);
}

STDMETHODIMP CCanvasRenderingContext2D::strokeRect(float x, float y, float w, float h)
{
	if (w == 0 && h == 0)
		return S_OK;

	return drawRect(x, y, w, h, STYLE_STROKE);
}

//////////////////////////////////////////////////////////////////////////
// paths
STDMETHODIMP CCanvasRenderingContext2D::beginPath()
{
	cairo_new_path(canvas->context);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::closePath()
{
	// Do nothing if the context has no subpath
	if (!cairo_has_current_point(canvas->context))
		return S_OK;

	cairo_close_path(canvas->context);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::moveTo(float x, float y)
{
	cairo_move_to(canvas->context, x, y);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::lineTo(float x, float y)
{
	// Do nothing if the context has no subpath
	if (!cairo_has_current_point(canvas->context))
		return S_OK;

	cairo_line_to(canvas->context, x, y);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::quadraticCurveTo(float cpx, float cpy, float x, float y)
{
	/*
	For the equations below the following variable name prefixes are used:
	   qp0 is the quadratic curve starting point (this is cx, cy - the current point coordinates).
	   qp1 is the quadatric curve control point (this is the cpx,cpy ).
	   qp2 is the quadratic curve ending point (this is the x,y).

	We will convert these points to compute the two needed cubic control points
	(the starting/ending points	are the same for both the quadratic and cubic curves)

	The equations for the two cubic control points are:
	cp0=qp0 and cp3=qp2
	cp1 = qp0 + 2/3 *(qp1-qp0)
	cp2 = cp1 + 1/3 *(qp2-qp0)

	In the code below, we must compute both the x and y terms for each point separately.

	cp1x = qp0x + 2.0/3.0*(qp1x - qp0x);
	cp1y = qp0y + 2.0/3.0*(qp1y - qp0y);
	cp2x = cp1x + (qp2x - qp0x)/3.0;
	cp2y = cp1y + (qp2y - qp0y)/3.0;
	*/

	// Do nothing if the context has no subpath
	if (!cairo_has_current_point(canvas->context))
		return S_OK;

	// the current point is the starting point
	double cx, cy;
	cairo_get_current_point(canvas->context, &cx, &cy);

	double cp1x = cx + 2.0/3.0 * (cpx - cx);
	double cp1y = cy + 2.0/3.0 * (cpy - cy);
	double cp2x = cp1x + 1.0/3.0 * (x - cx);
	double cp2y = cp1y + 1.0/3.0 * (y - cy);

	cairo_curve_to(canvas->context, cp1x, cp1y, cp2x, cp2y, x, y);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
{
	// Do nothing if the context has no subpath
	if (!cairo_has_current_point(canvas->context))
		return S_OK;

	cairo_curve_to(canvas->context, cp1x, cp1y, cp2x, cp2y, x, y);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::arcTo(float x1, float y1, float x2, float y2, float radius)
{
	CHECK_POSITIVE_VALUE(radius);

	// Do nothing if the context has no subpath
	if (!cairo_has_current_point(canvas->context))
		return S_OK;


	double dir, a2, b2, c2, cosx, sinx, d, anx, any, bnx, bny, x3, y3, x4, y4, cx, cy, angle0, angle1;
	bool anticlockwise;

	double x0, y0;
	cairo_get_current_point(canvas->context, &x0, &y0);

	//////////////////////////////////////////////////////////////////////////
	// Check if P0 = P1, if P1 = P2 or if radius = 0
	if ((x0 == x1 && y0 == y1) || (x1 == x2 && y1 == y2) || radius == 0)
	{
		cairo_line_to(canvas->context, x1, y1);
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// collinearity
	// check for area of the triangle determined by the three points
	dir = (x2 - x1) * (y0 - y1) + (y2 - y1) * (x1 - x0);

	if (dir == 0) {
		// connect new point (x1,y1) to (x0,y0) by a straight line
		cairo_line_to(canvas->context, x1, y1);
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	a2 = (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1);
	b2 = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	c2 = (x0 - x2) * (x0 - x2) + (y0 - y2) * (y0 - y2);
	cosx = (a2 + b2 - c2) / (2 * sqrt(a2 * b2));

	sinx = sqrt(1 - cosx * cosx);
	d = radius / ((1 - cosx) / sinx);

	anx = (x1 - x0) / sqrt(a2);
	any = (y1 - y0) / sqrt(a2);
	bnx = (x1 - x2) / sqrt(b2);
	bny = (y1 - y2) / sqrt(b2);
	x3 = x1 - anx * d;
	y3 = y1 - any * d;
	x4 = x1 - bnx * d;
	y4 = y1 - bny * d;
	anticlockwise = (dir < 0);
	cx = x3 + any * radius * (anticlockwise ? 1 : -1);
	cy = y3 - anx * radius * (anticlockwise ? 1 : -1);
	angle0 = atan2((y3 - cy), (x3 - cx));
	angle1 = atan2((y4 - cy), (x4 - cx));

	cairo_line_to(canvas->context, x3, y3);

	// the arc operation draws the line from current point (xc, yc) to arc center too.
	if (anticlockwise)
		cairo_arc_negative(canvas->context, cx, cy, radius, angle0, angle1);
	else
		cairo_arc(canvas->context, cx, cy, radius, angle0, angle1);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::rect(double x, double y, double width, double height)
{
	CHECK_INFINITY(x); CHECK_INFINITY(y); CHECK_INFINITY(width); CHECK_INFINITY(height);
	CHECK_NAN(x); CHECK_NAN(y); CHECK_NAN(width); CHECK_NAN(height);

	cairo_rectangle(canvas->context, x, y, width, height);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::arc(float x, float y, float radius, float startAngle, float endAngle, VARIANT_BOOL anticlockwise)
{
	CHECK_POSITIVE_VALUE(radius);

	// arc() with zero radius draws a line to the start point
	if (radius == 0 || (startAngle == endAngle))
		cairo_line_to(canvas->context, x, y);
	else if (anticlockwise == VARIANT_TRUE)
		cairo_arc_negative(canvas->context, x, y, radius, startAngle, endAngle);
	else
		cairo_arc(canvas->context, x, y, radius, startAngle, endAngle);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::fill()
{
	drawPath(STYLE_FILL);

	canvas->queueDraw();

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::stroke()
{
	drawPath(STYLE_STROKE);

	canvas->queueDraw();

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::clip()
{
	cairo_clip_preserve(canvas->context);

	canvas->queueDraw();
	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::isPointInPath(float x, float y, VARIANT_BOOL* isPresent)
{
	cairo_in_fill(canvas->context, x, y) ? *isPresent = VARIANT_TRUE : *isPresent = VARIANT_FALSE;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// text
STDMETHODIMP CCanvasRenderingContext2D::fillText(BSTR text, float x, float y, VARIANT maxWidth)
{
	return processText(text, x, y, maxWidth, TEXT_OPERATION_FILL, NULL);
}

STDMETHODIMP CCanvasRenderingContext2D::strokeText(BSTR text, float x, float y, VARIANT maxWidth)
{
	return processText(text, x, y, maxWidth, TEXT_OPERATION_STROKE, NULL);
}

STDMETHODIMP CCanvasRenderingContext2D::textAlongPath(BSTR text, BOOL stroke)
{
	VARIANT vNull;
	vNull.vt = VT_ERROR;
	return processText(text,  0, 0, vNull, stroke ? TEXT_OPERATION_PATH_STROKE : TEXT_OPERATION_PATH_FILL, NULL);
}

STDMETHODIMP CCanvasRenderingContext2D::pathText(BSTR text)
{
	VARIANT vNull;
	vNull.vt = VT_ERROR;
	return processText(text, 0, 0, vNull, TEXT_OPERATION_PATH, NULL);
}

STDMETHODIMP CCanvasRenderingContext2D::measureText(BSTR text, ICanvasTextMetrics** metrics)
{
	float width;
	VARIANT vNull;
	vNull.vt = VT_ERROR;
	HRESULT result = processText(text, NULL, NULL, vNull, TEXT_OPERATION_MEASURE, &width);

	if (result != S_OK)
		return result;

	CComObject<CCanvasTextMetrics>* pTextMetrics;
	CComObject<CCanvasTextMetrics>::CreateInstance(&pTextMetrics);
	pTextMetrics->setWidth(width);

	pTextMetrics->QueryInterface(IID_ICanvasTextMetrics, (void**)metrics);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// drawing images
STDMETHODIMP CCanvasRenderingContext2D::loadImage(BSTR path, ICanvasImage** image)
{
	if (CComBSTR(path) == CComBSTR(""))
		return CCOMError::DispatchError(NOT_FOUND_ERR, CLSID_CanvasRenderingContext2D, _T("Path is not valid"), "You need to provide a valid path!", 0, NULL);

	USES_CONVERSION;
	wstring img(OLE2W(path));

	// Create a new CCanvasImage instance
	CComObject<CCanvasImage>* pImage;
	CComObject<CCanvasImage>::CreateInstance(&pImage);

	HRESULT result = pImage->loadImage(img);

	if (result != S_OK) {
		SAFE_RELEASE(pImage);
		return result;
	}

	pImage->QueryInterface(IID_ICanvasImage, (void**)image);

	return S_OK;
}


STDMETHODIMP CCanvasRenderingContext2D::drawImage(VARIANT vInput, float dx, float dy, VARIANT dw, VARIANT dh)
{
	// we have to check the image before calling drawImageRegion
	// we will effectively be checking twice for it...
	// We only accept CCanvasImage and CCanvas
	if (vInput.vt != VT_DISPATCH && vInput.vt != 0x400c)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);

	VARIANT* input = &vInput;

	if (vInput.vt == 0x400c)
	{
		input = vInput.pvarVal;
		if (input->vt != VT_DISPATCH)
			return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);
	}

	//////////////////////////////////////////////////////////////////////////
	// CCanvas
	CCanvas* pCanvas;
	HRESULT result = input->pdispVal->QueryInterface(IID_ICanvas, (void**)&pCanvas);
	if (result != E_NOINTERFACE && pCanvas != NULL)
		return drawImageRegion(vInput, 0, 0, (float)pCanvas->getWidth(), (float)pCanvas->getHeight(), dx, dy, dw, dh);

	//////////////////////////////////////////////////////////////////////////
	// CCanvasImage
	CCanvasImage* pImage;
	result = input->pdispVal->QueryInterface(IID_ICanvasImage, (void**)&pImage);

	if (result == E_NOINTERFACE || pImage == NULL)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);

	return drawImageRegion(vInput, 0, 0, (float)pImage->getWidth(), (float)pImage->getHeight(), dx, dy, dw, dh);
}

STDMETHODIMP CCanvasRenderingContext2D::drawImageRegion(VARIANT vInput, float sx, float sy, float sw, float sh, float dx, float dy, VARIANT dw, VARIANT dh)
{
	// Variables
	float dWidth;
	float dHeight;
	float imgWidth;
	float imgHeight;
	cairo_pattern_t* pattern;

	// We only accept CCanvasImage and CCanvas
	if (vInput.vt != VT_DISPATCH && vInput.vt != 0x400c)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);

	VARIANT* input = &vInput;

	if (vInput.vt == 0x400c)
	{
		input = vInput.pvarVal;
		if (input->vt != VT_DISPATCH)
			return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);
	}

	//////////////////////////////////////////////////////////////////////////
	// CCanvas
	CCanvas* pCanvas;
	HRESULT result = input->pdispVal->QueryInterface(IID_ICanvas, (void**)&pCanvas);
	if (result != E_NOINTERFACE && pCanvas != NULL)
	{
		imgWidth = (float)pCanvas->getWidth();
		imgHeight = (float)pCanvas->getHeight();

		if (pCanvas->getWidth() == 0 || pCanvas->getHeight() == 0)
			return CCOMError::DispatchError(INVALID_STATE_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid canvas"), __FUNCTION__ ": canvas should have a width or a height different from 0", 0, NULL);

		pattern = createNewPatternFromSurface(pCanvas->getSurface(), pCanvas->getWidth(), pCanvas->getHeight());

		goto pattern_loaded;
	}

	//////////////////////////////////////////////////////////////////////////
	// CCanvasImage
	CCanvasImage* pImage;
	result = input->pdispVal->QueryInterface(IID_ICanvasImage, (void**)&pImage);
	if (result == E_NOINTERFACE || pImage == NULL)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);

	imgWidth = (float)pImage->getWidth();
	imgHeight = (float)pImage->getHeight();
	pattern = createNewPatternFromSurface(pImage->getSurface(), pImage->getWidth(), pImage->getHeight());

pattern_loaded:
	if (!pattern || cairo_pattern_status(pattern) != CAIRO_STATUS_SUCCESS)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid input"), __FUNCTION__ ": input should be an image or a canvas", 0, NULL);


	// check for dw and dh - if NAN: ignore and return
	if (dw.vt == VT_R8)
		goto cleanup;
	else if (dw.vt == VT_I4)
		dWidth = (float)dw.lVal;
	else
		dWidth = sw;

	if (dw.vt == VT_R8)
		goto cleanup;
	else if (dh.vt == VT_I4)
		dHeight = (float)dh.lVal;
	else
		dHeight = sh;

	// Nothing to do -- if only this was always that easy
	if (dHeight == 0 || dWidth == 0)
		goto cleanup;

	// Check for negative width and height
	float normalizeX, normalizeY, normalizeH, normalizeW;
	normalizeX = min(sx, (sx + sw));
	normalizeY = min(sy, (sy + sh));
	normalizeW = max(sw, -sw);
	normalizeH = max(sh, -sh);

	// Check that the source rectangle is entirely within the source image,
	// and that none of the sw or sh arguments is zero
	if (normalizeX < 0 || (normalizeX + normalizeW) > imgWidth ||
		normalizeY < 0 || (normalizeY + normalizeH) > imgHeight ||
		sw == 0 || sh == 0)
	{
		cairo_pattern_destroy(pattern);
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_CanvasRenderingContext2D, _T("Out of bounds"), __FUNCTION__ ": the source rectangle should be entirely within the source image, and sw or sh should not be zero", 0, NULL);
	}

	// Apply the transformation to the pattern
	cairo_matrix_t matrix;
	cairo_matrix_init_identity(&matrix);
	cairo_matrix_translate(&matrix, sx, sy);
	cairo_matrix_scale(&matrix, sw/dWidth, sh/dHeight);
	cairo_pattern_set_matrix(pattern, &matrix);

	// Save the current path
	cairo_path_t* path = cairo_copy_path(canvas->context);

	cairo_save(canvas->context);
	cairo_new_path(canvas->context);
	cairo_translate(canvas->context, dx, dy);
	cairo_set_source(canvas->context, pattern);

	ACQUIRE_DRAWING_MUTEX(canvas->hDrawMutex)

	// Draw shadow if needed
	if (canvas->state->isShadowVisible())
	{
		//////////////////////////////////////////////////////////////////////////
		// Calculate extents
		double xmin, ymin, xmax, ymax;
		xmin = 0;
		ymin = 0;
		xmax = dWidth;
		ymax = dHeight;

		double x[3], y[3];
		x[0] = xmin;  y[0] = ymax;
		x[1] = xmax;  y[1] = ymax;
		x[2] = xmax;  y[2] = ymin;

		cairo_user_to_device(canvas->context, &xmin, &ymin);
		xmax = xmin;
		ymax = ymin;
		for (int i = 0; i < 3; i++) {
			cairo_user_to_device(canvas->context, &x[i], &y[i]);
			xmin = min(xmin, x[i]);
			xmax = max(xmax, x[i]);
			ymin = min(ymin, y[i]);
			ymax = max(ymax, y[i]);
		}

		EXTENTS_RECT extents = { xmin,
							     ymin,
							     xmax,
							     ymax};

		//////////////////////////////////////////////////////////////////////////
		// Prepare to draw shadows
		cairo_t* ctx = shadow->getContext(canvas->context, canvas->state, extents);

		if (ctx)
		{
			// Draw into the shadow context

			// Clip to image region
			cairo_rectangle(ctx, 0, 0, dWidth, dHeight);
			cairo_clip(ctx);

			cairo_paint(ctx);

			// Draw the shadow
			canvas->state->applyStyle(STYLE_SHADOW);
			shadow->draw();
		}
	}

	// Clip to image region
	cairo_set_source(canvas->context, pattern);
	cairo_rectangle(canvas->context, 0, 0, dWidth, dHeight);
	cairo_clip(canvas->context);

	cairo_paint_with_alpha(canvas->context, currentState().globalAlpha);

	RELEASE_DRAWING_MUTEX(canvas->hDrawMutex)

	cairo_restore(canvas->context);

	// Restore the path
	cairo_new_path(canvas->context);
	cairo_append_path(canvas->context, path);

	canvas->queueDraw();

cleanup:
	// cleanup
	if (pattern)
		cairo_pattern_destroy(pattern);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// pixel manipulation
STDMETHODIMP CCanvasRenderingContext2D::createImageDataFrom(ICanvasImageData* canvasImageData, ICanvasImageData** data)
{
	// Check that imagedata is valid
	if (canvasImageData == NULL)
		return CCOMError::DispatchError(NOT_SUPPORTED_ERR, CLSID_CanvasRenderingContext2D, _T("CanvasImageData is NULL"), __FUNCTION__ ": you must pass a valid CanvasImageData object", 0, NULL);

	CCanvasImageData* imageData = (CCanvasImageData*) canvasImageData;

	return createImageData((float)imageData->getWidth(), (float)imageData->getHeight(), data);
}

STDMETHODIMP CCanvasRenderingContext2D::createImageData(float sw, float sh, ICanvasImageData** data)
{
	if (sw == 0 || sh == 0)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid parameters"), __FUNCTION__ ": sw or sh cannot be 0", 0, NULL);

	// Get the absolute values
	float w = abs(sw);
	float h = abs(sh);

	// Minimum size is 1x1
	if (w < 1) w = 1;
	if (h < 1) h = 1;

	// Create a new transparent black surface to hold the data
	unsigned char* buffer = (unsigned char*) calloc((size_t)(w*h), 4);
	cairo_surface_t* surface = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32, (int)w, (int)h, (int)w * 4);
	cairo_t* context = cairo_create(surface);

	// Clear surface
	cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
	cairo_new_path(context);
	cairo_rectangle(context, 0, 0, w, h);
	cairo_fill(context);

	// Create a new CCanvasImageData instance
	CComObject<CCanvasImageData>* pImageData;
	CComObject<CCanvasImageData>::CreateInstance(&pImageData);

	pImageData->setData(buffer, (long)w, (long)h);

	pImageData->QueryInterface(IID_ICanvasImageData, (void**)data);

	//cleanup
	cairo_destroy(context);
	cairo_surface_destroy(surface);

	return S_OK;
}

STDMETHODIMP CCanvasRenderingContext2D::getImageData(float sx, float sy, float sw, float sh, ICanvasImageData** data)
{
	if (sw == 0 || sh == 0)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid parameters"), __FUNCTION__ ": sw or sh cannot be 0", 0, NULL);

	float w = abs(sw);
	float h = abs(sh);

	// Minimum size is 1x1
	if (w < 1) w = 1;
	if (h < 1) h = 1;

	// Compute source point
	ADJUST_COORD(sw, sx);
	ADJUST_COORD(sh, sy);

	// Create a new transparent black surface to hold the data
	unsigned char* buffer = (unsigned char*) calloc((size_t)(w * h), 4);
	cairo_surface_t* surface = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32, (int)w, (int)h, (int)w * 4);
	cairo_t* context = cairo_create(surface);

	// Clear surface
	cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
	cairo_new_path(context);
	cairo_rectangle(context, 0, 0, w, h);
	cairo_fill(context);

	// Draw the selected rect into the surface
	cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_surface(context, canvas->getSurface(), -sx, -sy);
	cairo_paint(context);

	// Mirror the surface if we got negative width or height
	//if (sw < 0 && sh < 0)
	//	cairo_rotate(context, M_PI);
	//else if (sw < 0 && sh > 0)
	//else if (sw > 0 && sh < 0)

	// Create a new CCanvasImageData instance
	CComObject<CCanvasImageData>* pImageData;
	CComObject<CCanvasImageData>::CreateInstance(&pImageData);

	pImageData->setData(buffer, (long)w, (long)h);

	pImageData->QueryInterface(IID_ICanvasImageData, (void**)data);

	//cleanup
	cairo_destroy(context);
	cairo_surface_destroy(surface);

	return S_OK;;
}

STDMETHODIMP CCanvasRenderingContext2D::putImageData(ICanvasImageData* canvasImageData, float dx, float dy, VARIANT dirtyX, VARIANT dirtyY, VARIANT dirtyWidth, VARIANT dirtyHeight)
{
	if (canvasImageData == NULL)
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("CanvasImageData is NULL"), __FUNCTION__ ": you must pass a CanvasImageData object", 0, NULL);

	CCanvasImageData* imageData = (CCanvasImageData*) canvasImageData;

	// Check optional arguments
	float diX, diY, diW, diH;
	(dirtyX.vt == VT_I4) ? diX = (float)dirtyX.lVal : diX = 0;
	(dirtyY.vt == VT_I4) ? diY = (float)dirtyY.lVal : diY = 0;
	(dirtyWidth.vt == VT_I4)  ? diW = (float)dirtyWidth.lVal  : diW = (float)imageData->getWidth();
	(dirtyHeight.vt == VT_I4) ? diH = (float)dirtyHeight.lVal : diH = (float)imageData->getHeight();

	//////////////////////////////////////////////////////////////////////////
	// Do some strange things with those

	// 2. If dirtyWidth is negative, let dirtyX be dirtyX+dirtyWidth, and let dirtyWidth be equal to the absolute magnitude of dirtyWidth.
	//    If dirtyHeight is negative, let dirtyY be dirtyY+dirtyHeight, and let dirtyHeight be equal to the absolute magnitude of dirtyHeight.
	if (diW < 0) {
		diX += diW;
		diW = abs(diW);
	}

	if (diH < 0) {
		diY += diH;
		diH = abs(diH);
	}

	// 3. If dirtyX is negative, let dirtyWidth be dirtyWidth+dirtyX, and let dirtyX be zero.
	//    If dirtyY is negative, let dirtyHeight be dirtyHeight+dirtyY, and let dirtyY be zero.
	if (diX < 0) {
		diW += diX;
		diX = 0;
	}

	if (diY < 0) {
		diH += diY;
		diY = 0;
	}

	// 4. If dirtyX+dirtyWidth is greater than the width attribute of the imagedata argument,
	//       let dirtyWidth be the value of that width attribute, minus the value of dirtyX.
	//	  If dirtyY+dirtyHeight is greater than the height attribute of the imagedata argument,
	//		 let dirtyHeight be the value of that height attribute, minus the value of dirtyY.
	if (diX + diW > (float)imageData->getWidth())
		diW = (float)imageData->getWidth() - diX;

	if (diY + diH > (float)imageData->getHeight())
		diH = (float)imageData->getWidth() - diY;

	// 5. If, after those changes, either dirtyWidth or dirtyHeight is negative or zero, stop these steps without affecting the canvas.
	if (diW <= 0 || diH <= 0)
		return S_OK;

	//////////////////////////////////////////////////////////////////////////
	// Create a new surface from the image data
	int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, imageData->getWidth());
	cairo_surface_t* surface = cairo_image_surface_create_for_data(imageData->getData(), CAIRO_FORMAT_ARGB32, imageData->getWidth(), imageData->getHeight(), stride);
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy(surface);
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasRenderingContext2D, _T("Internal error"), __FUNCTION__ ": error while creating a surface from the image data", 0, NULL);
	}

	// Save the current path
	cairo_path_t* path = cairo_copy_path(canvas->context);

	// Copy the surface to our own
	cairo_save(canvas->context);

	cairo_identity_matrix(canvas->context);
	cairo_translate(canvas->context, dx, dy);

	cairo_new_path(canvas->context);
	cairo_rectangle(canvas->context, diX, diY, diW, diH);
	cairo_set_source_surface(canvas->context, surface, 0, 0);
	cairo_set_operator(canvas->context, CAIRO_OPERATOR_SOURCE);

	ACQUIRE_DRAWING_MUTEX(canvas->hDrawMutex)

	cairo_fill(canvas->context);

	RELEASE_DRAWING_MUTEX(canvas->hDrawMutex)

	cairo_restore(canvas->context);

	// Restore the path
	cairo_new_path(canvas->context);
	cairo_append_path(canvas->context, path);

	canvas->queueDraw();

	// Cleanup
	cairo_surface_destroy(surface);

	return S_OK;
}