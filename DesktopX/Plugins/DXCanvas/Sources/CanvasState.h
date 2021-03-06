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

#include "CSSColorParser.h"
#include "CanvasGradient.h"
#include "CanvasPattern.h"

#include <string>
#include <stack>
using namespace std;

//////////////////////////////////////////////////////////////////////////
// Default values
#define DEFAULT_ALPHA		1.0
#define DEFAULT_LINE_WIDTH  1.0
#define DEFAULT_LINE_CAP	CAIRO_LINE_CAP_BUTT
#define DEFAULT_LINE_JOIN	CAIRO_LINE_JOIN_MITER
#define DEFAULT_MITER_LIMIT 10.0
#define DEFAULT_OPERATOR	CAIRO_OPERATOR_OVER

#define DEFAULT_SHADOW_X	0.0
#define DEFAULT_SHADOW_Y	0.0
#define DEFAULT_SHADOW_BLUR 0.0

#define DEFAULT_FONT		"sans-serif 10px"
//////////////////////////////////////////////////////////////////////////

// Style
enum Style {
	STYLE_FILL = 1,
	STYLE_STROKE = 2,
	STYLE_SHADOW = 3
};

enum TextAlign {
	TEXT_ALIGN_START, // default
	TEXT_ALIGN_END,
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_RIGHT,
	TEXT_ALIGN_CENTER
};

enum TextBaseline {
	TEXT_BASELINE_TOP,
	TEXT_BASELINE_HANGING,
	TEXT_BASELINE_MIDDLE,
	TEXT_BASELINE_ALPHABETIC, // default
	TEXT_BASELINE_IDEOGRAPHIC,
	TEXT_BASELINE_BOTTOM
};

// Needed for the back reference to the canvas
class CCanvas;

class CanvasState {

public:
	struct State
	{
		CCanvasGradient* fillStyleGradient;
		CCanvasGradient* strokeStyleGradient;
		CCanvasPattern* fillStylePattern;
		CCanvasPattern* strokeStylePattern;

		RGBAColor fillColor;
		RGBAColor strokeColor;

		float globalAlpha;

		float shadowOffsetX;
		float shadowOffsetY;
		float shadowBlur;
		RGBAColor shadowColor;

		string fontDescription;
		TextAlign textAlign;
		TextBaseline textBaseline;

		State() :
			fillStyleGradient(NULL),
			strokeStyleGradient(NULL),
			fillStylePattern(NULL),
			strokeStylePattern(NULL),
			globalAlpha(DEFAULT_ALPHA),
			shadowOffsetX(DEFAULT_SHADOW_X),
			shadowOffsetY(DEFAULT_SHADOW_Y),
			shadowBlur(DEFAULT_SHADOW_BLUR),
			fontDescription(string(DEFAULT_FONT)),
			textAlign(TEXT_ALIGN_START),
			textBaseline(TEXT_BASELINE_ALPHABETIC)
			{
				shadowColor.a = 0.0f;
			}

		State (State const& old)
		{
			this->fillStyleGradient = old.fillStyleGradient;
			this->strokeStyleGradient = old.strokeStyleGradient;
			this->fillStylePattern = old.fillStylePattern;
			this->strokeStylePattern = old.strokeStylePattern;

			this->fillColor = old.fillColor;
			this->strokeColor = old.strokeColor;

			this->globalAlpha = old.globalAlpha;

			this->shadowOffsetX = old.shadowOffsetX;
			this->shadowOffsetY = old.shadowOffsetY;
			this->shadowBlur = old.shadowBlur;
			this->shadowColor = old.shadowColor;

			this->fontDescription = old.fontDescription;
			this->textAlign = old.textAlign;
			this->textBaseline = old.textBaseline;
		}

	};

	CanvasState(CCanvas* canvas)
	{
		this->canvas = canvas;

		parser = new CCSSColorParser();

		contextStack = new stack<State>();
		contextStack->push(State());
	};

	~CanvasState()
	{
		SAFE_DELETE(parser);
		SAFE_DELETE(contextStack);
	}

private:

	CCanvas* canvas;
	stack<State>* contextStack;
	CCSSColorParser* parser;

public:

	State& currentState();
	void save();
	bool restore();
	void clear();

	// Shadows
	bool isShadowVisible();

	// Styles
	bool isGlobalAlphaHandled(Style type);
	bool isColor(Style type);

	void applyStyle(Style type);
	HRESULT setStyle(VARIANT style, Style type);
	HRESULT getStyle(VARIANT* style, Style type);
};