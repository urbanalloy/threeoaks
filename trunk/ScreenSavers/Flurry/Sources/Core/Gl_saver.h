///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry
//
// Copyright (c) 2002, Calum Robinson
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// o Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// o Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// o Neither the name of the author nor the names of its contributors may
//   be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gl_platform.h"
#include "Smoke.h"
#include "Star.h"
#include "Spark.h"
#include "Particle.h"
#include "Std.h"

#define OPT_MODE_SCALAR_BASE		0x0
#define OPT_MODE_SCALAR_FRSQRTE		0x1
#define OPT_MODE_VECTOR_SIMPLE		0x2
#define OPT_MODE_VECTOR_UNROLLED	0x3

typedef enum _ColorModes
{
	redColorMode = 0,
	magentaColorMode,
	blueColorMode,
	cyanColorMode,
	greenColorMode,
	yellowColorMode,
	slowCyclicColorMode,
	cyclicColorMode,
	tiedyeColorMode,
	rainbowColorMode,
	whiteColorMode,
	multiColorMode,
	darkColorMode
} ColorModes;

#define MAXNUMPARTICLES 2500

typedef struct global_info_t {
	float flurryRandomSeed;
	float fTime;
	float fOldTime;
	float fDeltaTime;
	#define gravity 1500000.0f
	int sys_glWidth;
	int sys_glHeight;
	float drag;
	#define MouseX 0
	#define MouseY 0
	#define MouseDown 0
	
	ColorModes currentColorMode;
	float streamExpansion;
	int numStreams;
	
	#define incohesion 0.07f
	#define colorIncoherence 0.15f
	#define streamSpeed 450.0f
	#define fieldCoherence 0
	#define fieldSpeed 12.0f
	#define numParticles 250
	#define starSpeed 50
	#define seraphDistance 2000.0f
	#define streamSize 25000.0f
	#define fieldRange 1000.0f
	#define streamBias 7.0f
	
	int dframe;
	float starfieldColor[MAXNUMPARTICLES * 4 * 4];
	float starfieldVertices[MAXNUMPARTICLES * 2 * 4];
	float starfieldTextures[MAXNUMPARTICLES * 2 * 4];
	int starfieldColorIndex;
	int starfieldVerticesIndex;
	int starfieldTexturesIndex;
	Particle *p[MAXNUMPARTICLES];
	SmokeV *s;
	Star *star;
	Spark *spark[64];

    int optMode;
} global_info_t;

__private_extern__ global_info_t *info;

#define kNumSpectrumEntries 512

__private_extern__ void OTSetup();
__private_extern__ double TimeInSecondsSinceStart();

__private_extern__ void GLSetupRC(void);
__private_extern__ void GLRenderScene(void);
__private_extern__ void GLResize(int w, int h);