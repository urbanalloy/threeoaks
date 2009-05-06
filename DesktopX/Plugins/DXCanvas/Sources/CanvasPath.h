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

//////////////////////////////////////////////////////////////////////////
// Encapsulate mapping text onto a path
//
// FIXNME: "corruption" when filing disjointed segments, as the characters might
//         end up being stretched a lot.
class CanvasPath {

private:

	typedef double parametrization_t;

	/* Simple struct to hold a path and its parametrization */
	typedef struct {
		cairo_path_t *path;
		parametrization_t *parametrization;
	} parametrized_path_t;

	typedef void (*transform_point_func_t) (void *closure, double *x, double *y);

	static parametrization_t* parametrize_path(cairo_path_t *path);
	static void transform_path(cairo_path_t *path, transform_point_func_t f, void *closure);
	static void point_on_path (parametrized_path_t *param, double *x, double *y);
	static double two_points_distance (cairo_path_data_t *a, cairo_path_data_t *b);
	static double curve_length (double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);

public:

	static void map_path_onto (cairo_t *cr, cairo_path_t *path, float x, float y);
};