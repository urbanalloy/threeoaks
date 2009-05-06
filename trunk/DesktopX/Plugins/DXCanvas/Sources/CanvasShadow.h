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