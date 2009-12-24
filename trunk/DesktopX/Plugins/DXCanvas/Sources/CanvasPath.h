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