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
#define _USE_MATH_DEFINES
#include <math.h>

#include "CanvasShadow.h"

const float CanvasShadow::MAX_SIGMA = 35;
const float CanvasShadow::SCALE_FACTOR = 3 * sqrt(2 * (float)M_PI) / 4;


cairo_t* CanvasShadow::getContext(cairo_t* ctx, CanvasState* state, EXTENTS_RECT extents)
{
	// Store values
	originalContext = ctx;
	this->extents = extents;
	this->state = state;

	// Calculate standard deviation for Gaussian blur	
	if (state->currentState().shadowBlur < 8)
		sigma = state->currentState().shadowBlur / 2;
	else
		sigma = sqrt(state->currentState().shadowBlur);

	// Clamp sigma
	if (sigma > MAX_SIGMA)
		sigma = MAX_SIGMA;

	blurRadius = (int)floor(sigma*SCALE_FACTOR + 0.5);

	extents.x1 -= blurRadius;
	extents.y1 -= blurRadius;
	extents.x2 += blurRadius;
	extents.y2 += blurRadius;

	// Initialize our surface
	surface = cairo_image_surface_create(CAIRO_FORMAT_A8, (int)abs(extents.x2-extents.x1), (int)abs(extents.y2-extents.y1));
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		surface = NULL;
		return NULL;
	}

	// Use a device offset so callers don't need to worry about translating
	// coordinates, they can draw as if this was part of the destination context
	// at the coordinates of rect.
	cairo_surface_set_device_offset(surface, -extents.x1, -extents.y1);

	context = cairo_create(surface);
	if (cairo_status(context) != CAIRO_STATUS_SUCCESS) {
		context = NULL;
		return NULL;
	}
	
	// Copy the original context data to our new context
	copyContext();
	cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);

	return context;
}


void CanvasShadow::draw()
{
#ifdef DEBUG
	_ASSERT(context != NULL);
	_ASSERT(surface != NULL);
#endif
	// check that we have been initialized properly
	if (!context || !surface)
		return;

	// Save the current context matrix and set the context for drawing the shadow
	cairo_matrix_t savedMatrix;
	cairo_get_matrix(originalContext, &savedMatrix);

	cairo_identity_matrix(originalContext);
	cairo_translate(originalContext, state->currentState().shadowOffsetX, state->currentState().shadowOffsetY);

	// draw the shadow
	if (blurRadius != 0)
		applyBlur();

	cairo_mask_surface(originalContext, surface, 0, 0);

	cairo_set_matrix(originalContext, &savedMatrix);

	// Cleanup the context and surface
	cairo_destroy(context);
	cairo_surface_destroy(surface);	
	context = NULL; surface = NULL;
}

//////////////////////////////////////////////////////////////////////////
// Utils
bool CanvasShadow::copyContext()
{
	if (context == NULL || originalContext == NULL)
		return false;
	
	// Apply the original matrix
	cairo_matrix_t originalMatrix;
	cairo_get_matrix(originalContext, &originalMatrix);
	cairo_transform(context, &originalMatrix);

	// Add the path
	cairo_path_t* originalPath = cairo_copy_path(originalContext);
	cairo_new_path(context);
	cairo_append_path(context, originalPath);
	cairo_path_destroy(originalPath);

	// Get patterns if any
	cairo_pattern_t* originalPattern = cairo_get_source(originalContext);

	cairo_pattern_t* pattern;

	if (originalPattern)
		pattern = cairo_pattern_reference(originalPattern);
	else
		pattern = cairo_pattern_create_rgba(0, 0, 0, 0);

	cairo_set_source(context, pattern);

	// Apply current status
	cairo_set_line_width(context, cairo_get_line_width(originalContext));
	cairo_set_line_cap(context, cairo_get_line_cap(originalContext));
	cairo_set_line_join(context, cairo_get_line_join(originalContext));
	cairo_set_miter_limit(context, cairo_get_miter_limit(originalContext));
	cairo_set_fill_rule(context, cairo_get_fill_rule(originalContext));

	return true;
}

//
void CanvasShadow::applyBlur()
{
	// Blur radius must be > 2
	blurRadius = max(blurRadius, 2);

	unsigned char* buffer = cairo_image_surface_get_data(surface);
	int stride = cairo_image_surface_get_stride(surface);		
	int rows = cairo_image_surface_get_height(surface);	

	// Create temporary buffer
	unsigned char* tempBuffer = (unsigned char*) calloc((size_t)(stride * rows), sizeof(char));
	if (tempBuffer == NULL)
		return;

	blur(buffer, tempBuffer, HORIZONTAL, blurRadius, stride, rows);
	blur(tempBuffer, buffer, HORIZONTAL, blurRadius, stride, rows);
	blur(buffer, tempBuffer, HORIZONTAL, blurRadius, stride, rows);

	blur(tempBuffer, buffer, VERTICAL, blurRadius, stride, rows);
	blur(buffer, tempBuffer, VERTICAL, blurRadius, stride, rows);
	blur(tempBuffer, buffer, VERTICAL, blurRadius, stride, rows);

	free(tempBuffer);
}


void CanvasShadow::blur(unsigned char* input, unsigned char* output, Direction direction, int blurRadius, int stride, int rows)
{
	int deltaX =  blurRadius / 2;
	int deltaY = (blurRadius & 1) ? deltaX : deltaX - 1;

	int boxSize = deltaX + deltaY + 1;

	if (direction == HORIZONTAL)
	{	
	    for (int y = 0; y < rows; y++) {

			int sum = 0;
			
			for (int i = 0; i < boxSize; i++) {
				int pos = i - deltaX;
				pos = max(pos, 0);
				pos = min(pos, stride - 1);
				sum += input[stride * y + pos];
			}
			
			for (int x = 0; x < stride; x++) {
	            int tmp = x - deltaX;
	            int last = max(tmp, 0);
	            int next = min(tmp + boxSize, stride - 1);
	
		        output[stride * y + x] = (unsigned char)(sum/boxSize);
	
		        sum += input[stride * y + next] -
		               input[stride * y + last];	       
		    }
		}		
	} 
	else
	{
		for (int x = 0; x < stride; x++) {

			int sum = 0;

			for (int i = 0; i < boxSize; i++) {
				int pos = i - deltaX;
				pos = max(pos, 0);
				pos = min(pos, rows - 1);
				sum += input[stride * pos + x];
			}

			for (int y = 0; y < rows; y++) {
				int tmp = y - deltaX;
				int last = max(tmp, 0);
				int next = min(tmp + boxSize, rows - 1);

				output[stride * y + x] = (unsigned char)(sum/boxSize);

				sum += input[stride * next + x] -
					   input[stride * last + x];
			}
		}
	}
}