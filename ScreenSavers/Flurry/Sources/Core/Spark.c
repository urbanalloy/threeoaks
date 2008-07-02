/*

Copyright (c) 2002, Calum Robinson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the author nor the names of its contributors may be used
  to endorse or promote products derived from this software without specific
  prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/* Spark.cpp: implementation of the Spark class. */

//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "Std.h"
#include "Spark.h"


__private_extern__ void InitSpark(Spark *s)
{
	int i;
	for (i = 0; i < 3; i++) {
		s->position[i] = RandFlt(-100.0f, 100.0f);
	}
}

#define BIGMYSTERY 1800.0
#define MAXANGLES 16384


__private_extern__ void UpdateSparkColour(Spark *s)
{
	const float rotationsPerSecond = (float) (2.0 * PI * fieldSpeed / MAXANGLES);
	double thisPointInRadians = 2.0 * PI * s->mystery / BIGMYSTERY;
	double thisAngle = info->fTime * rotationsPerSecond;
	float cycleTime = 20.0f;
	float colorRot;
	float redPhaseShift;
	float greenPhaseShift; 
	float bluePhaseShift;
	float baseRed;
	float baseGreen;
	float baseBlue;
	float colorTime;
	
	switch (info->currentColorMode) {
		case rainbowColorMode: cycleTime = 1.5f; break;
		case tiedyeColorMode: cycleTime = 4.5f; break;
		case cyclicColorMode: cycleTime = 20.0f; break;
		case slowCyclicColorMode: cycleTime = 120.0f; break;
	}

	colorRot = (float) (2.0 * PI/cycleTime);
	redPhaseShift = 0.0f;
	greenPhaseShift = cycleTime / 3.0f; 
	bluePhaseShift = cycleTime * 2.0f / 3.0f;
	colorTime = info->fTime;
	
	switch (info->currentColorMode) {
	case whiteColorMode:
		baseRed = 0.1875f;
		baseGreen = 0.1875f;
		baseBlue = 0.1875f;
		break;
	case multiColorMode:
		baseRed = 0.0625f;
		baseGreen = 0.0625f;
		baseBlue = 0.0625f;
		break;
	case darkColorMode:
		baseRed = 0.0f;
		baseGreen = 0.0f;
		baseBlue = 0.0f;
		break;
	default:
		if (info->currentColorMode < slowCyclicColorMode) {
			colorTime = (info->currentColorMode / 6.0f) * cycleTime;
		} else {
			colorTime = info->fTime + info->flurryRandomSeed;
		}
		baseRed   = 0.109375f * ((float) cos((colorTime+redPhaseShift) * colorRot) + 1.0f);
		baseGreen = 0.109375f * ((float) cos((colorTime+greenPhaseShift) * colorRot) + 1.0f);
		baseBlue  = 0.109375f * ((float) cos((colorTime+bluePhaseShift) * colorRot) + 1.0f);
		break;
	}
	
	s->color[0] = baseRed   + 0.0625f * (0.5f + (float) cos((15.0 * (thisPointInRadians + 3.0 * thisAngle)))
	                                          + (float) sin((7.0  * (thisPointInRadians + thisAngle))));
	s->color[1] = baseGreen + 0.0625f * (0.5f + (float) sin(((thisPointInRadians) + thisAngle)));
	s->color[2] = baseBlue  + 0.0625f * (0.5f + (float) cos((37.0 * (thisPointInRadians + thisAngle))));
}


__private_extern__ void UpdateSpark(Spark *s)
{
    const float rotationsPerSecond = (float) (2.0 * PI * fieldSpeed / MAXANGLES);
	double thisPointInRadians = 2.0 * PI * s->mystery / BIGMYSTERY;
    double thisAngle = info->fTime * rotationsPerSecond;
    double cf;
    int i;
    double tmpX1,tmpY1,tmpZ1;
    double tmpX2,tmpY2,tmpZ2;
    double tmpX3,tmpY3,tmpZ3;
    double tmpX4,tmpY4,tmpZ4;
    double rotation;
    double cr;
    double sr;
    //float cycleTime = 20.0f;
    float old[3];

	UpdateSparkColour(s);
    
    for (i = 0; i < 3; i++) {
        old[i] = s->position[i];
    }
    
	cf = cos(7.0 * thisAngle) + cos(3.0 * thisAngle) + cos(13.0 * thisAngle);
    cf /= 6.0f;
    cf += 2.0f;

    s->position[0] = (float)(fieldRange * cf * cos(11.0 * (thisPointInRadians + (3.0  * thisAngle))));
    s->position[1] = (float)(fieldRange * cf * sin(12.0 * (thisPointInRadians + (4.0  * thisAngle))));
    s->position[2] = (float)(fieldRange *      cos(23.0 * (thisPointInRadians + (12.0 * thisAngle))));
    
    rotation = thisAngle * 0.501 + 5.01 * (double) s->mystery / (double) BIGMYSTERY;
    cr = cos(rotation);
    sr = sin(rotation);
    tmpX1 = s->position[0] * cr - s->position[1] * sr;
    tmpY1 = s->position[1] * cr + s->position[0] * sr;
    tmpZ1 = s->position[2];
    
    tmpX2 = tmpX1 * cr - tmpZ1 * sr;
    tmpY2 = tmpY1;
    tmpZ2 = tmpZ1 * cr + tmpX1 * sr;
    
    tmpX3 = tmpX2;
    tmpY3 = tmpY2 * cr - tmpZ2 * sr;
    tmpZ3 = tmpZ2 * cr + tmpY2 * sr + seraphDistance;
    
    rotation = thisAngle * 2.501 + 85.01 * (double) s->mystery / (double) BIGMYSTERY;
    cr = cos(rotation);
    sr = sin(rotation);
    tmpX4 = tmpX3 * cr - tmpY3 * sr;
    tmpY4 = tmpY3 * cr + tmpX3 * sr;
    tmpZ4 = tmpZ3;
    
    s->position[0] = (float) tmpX4 + RandBell(5.0f * fieldCoherence);
    s->position[1] = (float) tmpY4 + RandBell(5.0f * fieldCoherence);
    s->position[2] = (float) tmpZ4 + RandBell(5.0f * fieldCoherence);

    for (i = 0; i < 3; i++) {
        s->delta[i] = (s->position[i] - old[i]) / info->fDeltaTime;
    }
}
