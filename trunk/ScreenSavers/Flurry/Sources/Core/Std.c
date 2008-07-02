// Precompiled "Std" Include Header.

#include "Std.h"

//UInt8  sys_glBPP=32;
//int SSMODE = FALSE;
//int currentVideoMode = 0;
//int cohesiveness = 7;
//int fieldStrength;
//int colorCoherence = 7;
//int fieldIncoherence = 0;
//int ifieldSpeed = 120;

__private_extern__ __inline float FastDistance2D(float x, float y)
{
	// this function computes the distance from 0,0 to x,y with ~3.5% error
	float mn;
	// first compute the absolute value of x,y
	x = (x < 0.0f) ? -x : x;
	y = (y < 0.0f) ? -y : y;
	
	// compute the minimum of x,y
	mn = x<y?x:y;
	
	// return the distance
	//return(x+y-(mn*0.5f)-(mn*0.25f)+(mn*0.0625f));
	return x+y-mn*.6875;
	
}
