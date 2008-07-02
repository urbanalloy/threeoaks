#ifndef __GL_SAVER_H_
#define __GL_SAVER_H_

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


#endif // __GL_SAVER_H_
