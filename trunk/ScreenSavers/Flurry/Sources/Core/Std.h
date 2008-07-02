//-------------------------------------------------------------------
//  File:   Std.h
//  Created:  02/12/00 9:01:PM
//  Author:   Aaron Hilton
//  Comments: Standard header file to include all source files.
//            (Precompiled header)
//-------------------------------------------------------------------
#ifndef __STD_h_
#define __STD_h_

#include <math.h>
#include <stdlib.h>
#include "PTypes.h"
#include "Gl_saver.h"


INLINE float FastDistance2D(float x, float y)
{
	// this function computes the distance from 0,0 to x,y with ~3.5% error
	float mn;
	// first compute the absolute value of x,y
	x = (x < 0.0f) ? -x : x;
	y = (y < 0.0f) ? -y : y;
	
	// compute the minimum of x,y
	mn = (x < y) ? x : y;
	
	// return the distance
	return x + y - mn * 0.6875f;
	
}


#define RandFlt(min, max) (min + (max - min) * rand() / (float) RAND_MAX)
#define RandBell(scale) (scale * (1.0f - (rand() + rand() + rand()) / ((float) RAND_MAX * 1.5f)))


#endif // __STD_h_
