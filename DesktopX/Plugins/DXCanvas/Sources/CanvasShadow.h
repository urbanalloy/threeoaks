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

#include <cairo.h>
#include <cairo-win32.h>
#include <pango/pangocairo.h>
#include <pango/pangowin32.h>

#include "CanvasState.h"

struct EXTENTS_RECT
{
	double x1;
	double y1;
	double x2;
	double y2;
};

class CanvasShadow {

public:

	CanvasShadow() :
		  context(NULL),
		  surface(NULL),
		  sigma(0),
		  blurRadius(0)
	{};

	~CanvasShadow()
	{
		if (context)
			cairo_destroy(context);

		if (surface)
			cairo_surface_destroy(surface);
	}

private:
	enum Direction
	{
		HORIZONTAL,
		VERTICAL
	};

	static const float MAX_SIGMA;
	static const float GAUSSIAN_SCALE_FACTOR;
	float sigma;
	int blurRadius;

	// Our surface and context
	cairo_surface_t *surface;
	cairo_t* context;
	EXTENTS_RECT extents;

	// the main cairo context
	cairo_t* originalContext;
	CanvasState* state;

	bool copyContext();
	void applyBlur();

	void computeLobes(int radius, int lobes[3][2]);
	void blur(unsigned char* input, unsigned char* output, Direction direction, int leftLobe, int rightLobe, int stride, int rows);

public:

	cairo_t* getContext(cairo_t* ctx, CanvasState* state, EXTENTS_RECT extents);
	void draw();
};