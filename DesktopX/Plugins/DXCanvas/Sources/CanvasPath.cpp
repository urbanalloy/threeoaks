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
// Uses code from a Cairo example (public domain?)
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "CanvasPath.h"

/* Returns Euclidean distance between two points */
double CanvasPath::two_points_distance (cairo_path_data_t *a, cairo_path_data_t *b)
{
	double dx, dy;

	dx = b->point.x - a->point.x;
	dy = b->point.y - a->point.y;

	return sqrt (dx * dx + dy * dy);
}

/* Returns length of a Bezier curve.
* Seems like computing that analytically is not easy.  The
* code just flattens the curve using cairo and adds the length
* of segments.
*/
double CanvasPath::curve_length (double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3)
{
	cairo_surface_t *surface;
	cairo_t *cr;
	cairo_path_t *path;
	cairo_path_data_t *data, current_point;
	int i;
	double length;

	surface = cairo_image_surface_create (CAIRO_FORMAT_A8, 0, 0);
	cr = cairo_create (surface);
	cairo_surface_destroy (surface);

	cairo_move_to (cr, x0, y0);
	cairo_curve_to (cr, x1, y1, x2, y2, x3, y3);

	length = 0;
	path = cairo_copy_path_flat (cr);
	for (i=0; i < path->num_data; i += path->data[i].header.length) {
		data = &path->data[i];
		switch (data->header.type) {

	case CAIRO_PATH_MOVE_TO:
		current_point = data[1];
		break;

	case CAIRO_PATH_LINE_TO:
		length += two_points_distance (&current_point, &data[1]);
		current_point = data[1];
		break;

	default:
	case CAIRO_PATH_CURVE_TO:
	case CAIRO_PATH_CLOSE_PATH:
		break;
		}
	}
	cairo_path_destroy (path);

	cairo_destroy (cr);

	return length;
}

#pragma warning(push)
#pragma warning(disable: 4701)

/* Compute parametrization info.  That is, for each part of the
 * cairo path, tags it with its length.
 *
 * Free returned value with g_free().
 */
CanvasPath::parametrization_t* CanvasPath::parametrize_path (cairo_path_t *path)
{
	cairo_path_data_t *data;
	cairo_path_data_t last_move_to;
	cairo_path_data_t current_point;
	parametrization_t *parametrization;

	parametrization = (parametrization_t *)malloc (path->num_data * sizeof (parametrization[0]));

	for (int i = 0; i < path->num_data; i += path->data[i].header.length) {
		data = &path->data[i];
		parametrization[i] = 0.0;
		switch (data->header.type) {
			case CAIRO_PATH_MOVE_TO:
				last_move_to = data[1];
				current_point = data[1];
				break;
			case CAIRO_PATH_CLOSE_PATH:
				/* Make it look like it's a line_to to last_move_to */
				data = (&last_move_to) - 1;
				/* fall through */
			case CAIRO_PATH_LINE_TO:
				parametrization[i] = two_points_distance (&current_point, &data[1]);
				current_point = data[1];
				break;
			case CAIRO_PATH_CURVE_TO:
				/* naive curve-length, treating bezier as three line segments:
				parametrization[i] = two_points_distance (&current_point, &data[1])
				+ two_points_distance (&data[1], &data[2])
				+ two_points_distance (&data[2], &data[3]);
				*/
				parametrization[i] = curve_length (current_point.point.x, current_point.point.x,
					data[1].point.x, data[1].point.y,
					data[2].point.x, data[2].point.y,
					data[3].point.x, data[3].point.y);

				current_point = data[3];
				break;
			default:
				break;
		}
	}

	return parametrization;
}
#pragma warning(pop)

/* Project a path using a function.  Each point of the path (including
* Bezier control points) is passed to the function for transformation.
*/
void CanvasPath::transform_path (cairo_path_t *path, transform_point_func_t f, void *closure)
{
	int i;
	cairo_path_data_t *data;

	for (i=0; i < path->num_data; i += path->data[i].header.length) {
		data = &path->data[i];
		switch (data->header.type) {
	case CAIRO_PATH_CURVE_TO:
		f (closure, &data[3].point.x, &data[3].point.y);
		f (closure, &data[2].point.x, &data[2].point.y);
	case CAIRO_PATH_MOVE_TO:
	case CAIRO_PATH_LINE_TO:
		f (closure, &data[1].point.x, &data[1].point.y);
		break;
	case CAIRO_PATH_CLOSE_PATH:
		break;
	default:
		break;
		}
	}
}

#pragma warning(push)
#pragma warning(disable: 4701)

/* Project a point X,Y onto a parameterized path.  The final point is
* where you get if you walk on the path forward from the beginning for X
* units, then stop there and walk another Y units perpendicular to the
* path at that point.  In more detail:
*
* There's three pieces of math involved:
*
*   - The parametric form of the Line equation
*     http://en.wikipedia.org/wiki/Line
*
*   - The parametric form of the Cubic B�zier curve equation
*     http://en.wikipedia.org/wiki/B%C3%A9zier_curve
*
*   - The Gradient (aka multi-dimensional derivative) of the above
*     http://en.wikipedia.org/wiki/Gradient
*
* The parametric forms are used to answer the question of "where will I be
* if I walk a distance of X on this path".  The Gradient is used to answer
* the question of "where will I be if then I stop, rotate left for 90
* degrees and walk straight for a distance of Y".
*/
void CanvasPath::point_on_path (parametrized_path_t *param, double *x, double *y)
{
	int i;
	double ratio, the_y = *y, the_x = *x, dx, dy;
	cairo_path_data_t *data, last_move_to, current_point;
	cairo_path_t *path = param->path;
	parametrization_t *parametrization = param->parametrization;

	if (path->data == NULL)
		return;

	for (i=0; i + path->data[i].header.length < path->num_data &&
		(the_x > parametrization[i] ||
		path->data[i].header.type == CAIRO_PATH_MOVE_TO);
	i += path->data[i].header.length) {
		the_x -= parametrization[i];
		data = &path->data[i];
		switch (data->header.type) {
	case CAIRO_PATH_MOVE_TO:
		current_point = data[1];
		last_move_to = data[1];
		break;
	case CAIRO_PATH_LINE_TO:
		current_point = data[1];
		break;
	case CAIRO_PATH_CURVE_TO:
		current_point = data[3];
		break;
	case CAIRO_PATH_CLOSE_PATH:
		break;
	default:
		break;
		}
	}
	data = &path->data[i];

	switch (data->header.type) {

  case CAIRO_PATH_MOVE_TO:
	  break;
  case CAIRO_PATH_CLOSE_PATH:
	  /* Make it look like it's a line_to to last_move_to */
	  data = (&last_move_to) - 1;
	  /* fall through */
  case CAIRO_PATH_LINE_TO:
	  {
		  ratio = the_x / parametrization[i];
		  /* Line polynomial */
		  *x = current_point.point.x * (1 - ratio) + data[1].point.x * ratio;
		  *y = current_point.point.y * (1 - ratio) + data[1].point.y * ratio;

		  /* Line gradient */
		  dx = -(current_point.point.x - data[1].point.x);
		  dy = -(current_point.point.y - data[1].point.y);

		  /*optimization for: ratio = the_y / sqrt (dx * dx + dy * dy);*/
		  ratio = the_y / parametrization[i];
		  *x += -dy * ratio;
		  *y +=  dx * ratio;
	  }
	  break;
  case CAIRO_PATH_CURVE_TO:
	  {
		  /* FIXME the formulas here are not exactly what we want, because the
		  * Bezier parametrization is not uniform.  But I don't know how to do
		  * better.  The caller can do slightly better though, by flattening the
		  * Bezier and avoiding this branch completely.  That has its own cost
		  * though, as large y values magnify the flattening error drastically.
		  */

		  double ratio_1_0, ratio_0_1;
		  double ratio_2_0, ratio_0_2;
		  double ratio_3_0, ratio_2_1, ratio_1_2, ratio_0_3;
		  double _1__4ratio_1_0_3ratio_2_0, _2ratio_1_0_3ratio_2_0;

		  ratio = the_x / parametrization[i];

		  ratio_1_0 = ratio;
		  ratio_0_1 = 1 - ratio;

		  ratio_2_0 = ratio_1_0 * ratio_1_0; /*      ratio  *      ratio  */
		  ratio_0_2 = ratio_0_1 * ratio_0_1; /* (1 - ratio) * (1 - ratio) */

		  ratio_3_0 = ratio_2_0 * ratio_1_0; /*      ratio  *      ratio  *      ratio  */
		  ratio_2_1 = ratio_2_0 * ratio_0_1; /*      ratio  *      ratio  * (1 - ratio) */
		  ratio_1_2 = ratio_1_0 * ratio_0_2; /*      ratio  * (1 - ratio) * (1 - ratio) */
		  ratio_0_3 = ratio_0_1 * ratio_0_2; /* (1 - ratio) * (1 - ratio) * (1 - ratio) */

		  _1__4ratio_1_0_3ratio_2_0 = 1 - 4 * ratio_1_0 + 3 * ratio_2_0;
		  _2ratio_1_0_3ratio_2_0    =     2 * ratio_1_0 - 3 * ratio_2_0;

		  /* Bezier polynomial */
		  *x = current_point.point.x * ratio_0_3
			  + 3 *   data[1].point.x * ratio_1_2
			  + 3 *   data[2].point.x * ratio_2_1
			  +       data[3].point.x * ratio_3_0;
		  *y = current_point.point.y * ratio_0_3
			  + 3 *   data[1].point.y * ratio_1_2
			  + 3 *   data[2].point.y * ratio_2_1
			  +       data[3].point.y * ratio_3_0;

		  /* Bezier gradient */
		  dx =-3 * current_point.point.x * ratio_0_2
			  + 3 *       data[1].point.x * _1__4ratio_1_0_3ratio_2_0
			  + 3 *       data[2].point.x * _2ratio_1_0_3ratio_2_0
			  + 3 *       data[3].point.x * ratio_2_0;
		  dy =-3 * current_point.point.y * ratio_0_2
			  + 3 *       data[1].point.y * _1__4ratio_1_0_3ratio_2_0
			  + 3 *       data[2].point.y * _2ratio_1_0_3ratio_2_0
			  + 3 *       data[3].point.y * ratio_2_0;

		  ratio = the_y / sqrt (dx * dx + dy * dy);
		  *x += -dy * ratio;
		  *y +=  dx * ratio;
	  }
	  break;
  default:
	  break;
	}
}
#pragma warning(pop)

/* Projects the current path of cr onto the provided path. */
void CanvasPath::map_path_onto (cairo_t *cr, cairo_path_t *path, float x, float y)
{
	cairo_path_t *current_path;
	parametrized_path_t param;

	param.path = path;
	param.parametrization = parametrize_path (path);

	current_path = cairo_copy_path (cr);
	cairo_new_path (cr);
	cairo_move_to(cr, x, y);

	transform_path (current_path, (transform_point_func_t) point_on_path, &param);

	cairo_append_path (cr, current_path);

	cairo_path_destroy (current_path);
	free (param.parametrization);
}
