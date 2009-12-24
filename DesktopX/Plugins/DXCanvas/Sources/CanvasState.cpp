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
#include <comsvcs.h>
#include "COMError.h"

#include "Canvas.h"
#include "CanvasState.h"

CanvasState::State& CanvasState::currentState()
{
	return contextStack->top();
}

//////////////////////////////////////////////////////////////////////////
void CanvasState::save()
{
	State current(contextStack->top());

	contextStack->push(current);
}

bool CanvasState::restore()
{
	// No saved state -> do nothing
	if (contextStack->size() == 1) // the first element in the stack is the main state
		return false;

	contextStack->pop();

	return true;
}

void CanvasState::clear()
{
	// Cleanup the context stack
	while (!contextStack->empty())
		contextStack->pop();

	contextStack->push(State());
}

//////////////////////////////////////////////////////////////////////////
// Shadows and surfaces
//////////////////////////////////////////////////////////////////////////

bool CanvasState::isShadowVisible()
{
	// If fully transparent, no need to draw shadow
	if (currentState().shadowColor.a == 0.0f)
		return FALSE;

	// Default values
	bool isDefaultModified = currentState().shadowOffsetX != 0 ||
							 currentState().shadowOffsetY != 0 ||
							(currentState().shadowColor.a != 0.0f ||
							 currentState().shadowColor.r != 0 ||
						     currentState().shadowColor.g != 0 ||
							 currentState().shadowColor.b != 0);

	return isDefaultModified;
}

//////////////////////////////////////////////////////////////////////////
// Styles
//////////////////////////////////////////////////////////////////////////

// We need to use an intermediate surface when drawing with a non solid color or using a pattern/image
bool CanvasState::isGlobalAlphaHandled(Style type)
{
	return currentState().globalAlpha == 1.0 || isColor(type);
}

bool CanvasState::isColor(Style type)
{
	switch (type)
	{
		case STYLE_FILL:
			if (currentState().fillStyleGradient == NULL && currentState().fillStylePattern == NULL)
				return true;
			break;

		case STYLE_STROKE:
			if (currentState().strokeStyleGradient == NULL && currentState().strokeStylePattern == NULL)
				return true;
			break;

		default:
			return true;
	}

	return false;
}


void CanvasState::applyStyle(Style style)
{
	switch (style)
	{
		case STYLE_FILL:
		{
			if (currentState().fillStyleGradient)
				currentState().fillStyleGradient->apply(canvas->context);
			else if (currentState().fillStylePattern)
				currentState().fillStylePattern->apply(canvas->context);
			else {
				//float premul = currentState().fillColor.a*currentState().globalAlpha;
				cairo_set_source_rgba(canvas->context, currentState().fillColor.r/255.0,
													   currentState().fillColor.g/255.0,
													   currentState().fillColor.b/255.0,
													   currentState().fillColor.a * currentState().globalAlpha);
			}
		}
		break;

		case STYLE_STROKE:
		{
			if (currentState().strokeStyleGradient)
				currentState().strokeStyleGradient->apply(canvas->context);
			else if (currentState().strokeStylePattern)
				currentState().strokeStylePattern->apply(canvas->context);
			else
				cairo_set_source_rgba(canvas->context, currentState().strokeColor.r/255.0,
													   currentState().strokeColor.g/255.0,
													   currentState().strokeColor.b/255.0,
													   currentState().strokeColor.a * currentState().globalAlpha);
		}
		break;

		case STYLE_SHADOW:
		{
			cairo_set_source_rgba(canvas->context, currentState().shadowColor.r/255.0,
												   currentState().shadowColor.g/255.0,
												   currentState().shadowColor.b/255.0,
												   currentState().shadowColor.a);

			break;
		}
	}
}

HRESULT CanvasState::setStyle(VARIANT style, Style type)
{
	// We accept two type of variant: VT_BSTR and VT_DISPATCH (containing a CCanvasGradient)
	if (style.vt != VT_DISPATCH && style.vt != VT_BSTR)
		return (canvas->debugMode ? CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid style"), __FUNCTION__ ": style should be a color, gradient or pattern", 0, NULL) : S_OK);

	USES_CONVERSION;

	//////////////////////////////////////////////////////////////////////////
	// VT_BSTR
	if (style.vt == VT_BSTR)
	{
		string color(OLE2T(style.bstrVal));
		RGBAColor parsedColor;
		if (!parser->parseColor(color, &parsedColor))
		{
			if (canvas->debugMode) {
				char error[1000];
				sprintf_s(error, __FUNCTION__ ": Failed to parse color value (value: %s)", OLE2T(style.bstrVal));
				return CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasRenderingContext2D, _T("Failed to parse color value"), error, 0, NULL);
			} else
				return S_OK;
		}

		switch (type)
		{
			case STYLE_FILL:
				currentState().fillColor = parsedColor;
				currentState().fillStyleGradient = NULL;
				currentState().fillStylePattern = NULL;
				break;

			case STYLE_STROKE:
				currentState().strokeColor = parsedColor;
				currentState().strokeStyleGradient = NULL;
				currentState().strokeStylePattern = NULL;
				break;

			case STYLE_SHADOW:
				currentState().shadowColor = parsedColor;
				break;
		}

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// VT_DISPATCH
	CCanvasGradient* pGradient;
	HRESULT result = style.pdispVal->QueryInterface(IID_ICanvasGradient, (void**)&pGradient);

	if (result != E_NOINTERFACE && pGradient != NULL)
	{
		switch (type)
		{
			case STYLE_FILL:
				currentState().fillStyleGradient = pGradient;
				currentState().fillStylePattern = NULL;
				break;

			case STYLE_STROKE:
				currentState().strokeStyleGradient = pGradient;
				currentState().strokeStylePattern = NULL;
				break;
		}

		return S_OK;
	}

	CCanvasPattern* pPattern;
	result = style.pdispVal->QueryInterface(IID_ICanvasPattern, (void**)&pPattern);

	if (result != E_NOINTERFACE && pPattern != NULL)
	{
		switch (type)
		{
		case STYLE_FILL:
			currentState().fillStyleGradient = NULL;
			currentState().fillStylePattern = pPattern;
			break;

		case STYLE_STROKE:
			currentState().strokeStyleGradient = NULL;
			currentState().strokeStylePattern = pPattern;
			break;
		}

		return S_OK;
	}

	return (canvas->debugMode ? CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Invalid style"), __FUNCTION__ ": style should be a color, gradient or pattern", 0, NULL) : S_OK);
}

HRESULT CanvasState::getStyle(VARIANT* style, Style type)
{
	CComVariant v;

	// Gradient or Pattern
	if (currentState().fillStyleGradient != NULL ||
		currentState().fillStylePattern != NULL ||
		currentState().strokeStyleGradient != NULL ||
		currentState().strokeStylePattern != NULL
		)
	{
#define CHECK_INTERFACE(iid, comPtr) \
	if (type == STYLE_FILL) { \
	if (currentState().fillStyleGradient) \
	result = currentState().fillStyleGradient->QueryInterface(iid, (void**)&comPtr); \
	else \
	result = currentState().fillStylePattern->QueryInterface(iid, (void**)&comPtr); \
	} else { \
	if (currentState().strokeStyleGradient) \
	result = currentState().strokeStyleGradient->QueryInterface(iid, (void**)&comPtr); \
	else \
	result = currentState().strokeStylePattern->QueryInterface(iid, (void**)&comPtr); }

		// Gradient type
		CComPtr<CCanvasGradient>* pGradient = NULL;
		HRESULT result;

		CHECK_INTERFACE(IID_ICanvasGradient, pGradient);

		if (result != E_NOINTERFACE && pGradient != NULL)
		{
			v.vt = VT_DISPATCH;
			v.pdispVal = pGradient->Detach();

			v.Detach(style);

			return S_OK;
		}

		// Pattern type
		CComPtr<CCanvasPattern>* pPattern = NULL;

		CHECK_INTERFACE(IID_ICanvasPattern, pPattern);

		if (result != E_NOINTERFACE && pPattern != NULL)
		{
			v.vt = VT_DISPATCH;
			v.pdispVal = pPattern->Detach();

			v.Detach(style);

			return S_OK;
		}

		// Failed to get the current style!
		return CCOMError::DispatchError(TYPE_MISMATCH_ERR, CLSID_CanvasRenderingContext2D, _T("Internal error"), __FUNCTION__ ": internal error when retrieving the current style", 0, NULL);;
	}

	// Simple color
	RGBAColor* sColor;
	if (type == STYLE_FILL)
		sColor = &currentState().fillColor;
	else if (type == STYLE_STROKE)
		sColor = &currentState().strokeColor;
	else // STYLE_SHADOW
		sColor = &currentState().shadowColor;

	CComBSTR color(parser->toString(sColor).c_str());

	v.vt = VT_BSTR;
	v.bstrVal = color.Detach();

	v.Detach(style);

	return S_OK;
}