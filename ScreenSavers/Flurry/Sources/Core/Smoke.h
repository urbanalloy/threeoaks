// Smoke.h: interface for the Smoke class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SMOKE_H_
#define __SMOKE_H_


#include "gl_saver.h"
#include "SmokeParticle.h"

#define NUMSMOKEPARTICLES 3600

typedef struct SmokeV  
{
	SmokeParticleV p[NUMSMOKEPARTICLES / 4];
	int nextParticle;
	int nextSubParticle;
	float lastParticleTime;
	int firstTime;
	long frame;
	float old[3];
	floatToVector seraphimVertices[NUMSMOKEPARTICLES * 2 + 1];
	floatToVector seraphimColors[NUMSMOKEPARTICLES * 4 + 1];
	float seraphimTextures[NUMSMOKEPARTICLES * 2 * 4];
} SmokeV;


__private_extern__ void InitSmoke(SmokeV *s);
__private_extern__ void UpdateSmoke_ScalarBase(SmokeV *s);
__private_extern__ void DrawSmoke_Scalar(SmokeV *s);


#endif // __SMOKE_H_
