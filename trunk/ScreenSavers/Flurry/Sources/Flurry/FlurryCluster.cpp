/*
 * Flurry for Windows.
 *
 * Encapsulate the Flurry code away from the non-Flurry-specific screensaver
 * code.  Knows how to create and draw a single flurry cluster.
 *
 * Created 2/23/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#include "FlurryCluster.h"
#include "FlurryPreset.h"
#include "FlurrySettings.h"
#include "TimeSupport.h"

#include <math.h> // unconfuse cpp, which will see it inside extern "C" with __cplusplus defined
extern "C" {
	#include "Core/gl_saver.h"
}


///////////////////////////////////////////////////////////////////////////
//
// private functions


///////////////////////////////////////////////////////////////////////////
//
// public functions

/*
 * Note: the Flurry base code keeps everything in a global variable named
 * info.  We want to instance it, for multimon support (several separate
 * Flurries), so we allocate several such structures, but to avoid changing
 * the base code, we set info = current->globals before calling into it.
 * Obviously, not thread safe.
 */

FlurryCluster::FlurryCluster(const FlurryClusterSpec& spec)
{
	oldFrameTime = TimeInSecondsSinceStart();

	flurryData = FlurryAlloc();

	// specialize
	flurryData->numStreams = spec.nStreams;
	flurryData->currentColorMode = (ColorModes)spec.color;
	flurryData->streamExpansion = spec.thickness;
	flurryData->star->rotSpeed = spec.speed;
}


FlurryCluster::~FlurryCluster()
{
	int i;
	for (i = 0; i < MAXNUMPARTICLES; i++) {
		free(flurryData->p[i]);
	}
	free(flurryData->s);
	free(flurryData->star);
	for (i = 0; i < 64; i++) {
		free(flurryData->spark[i]);
	}
	free(flurryData);
}


void FlurryCluster::SetSize(int width, int height)
{
	// make this flurry cluster current
	BecomeCurrent();
	// resize it
	GLResize(width, height);
}


void FlurryCluster::PrepareToAnimate()
{
	// make this flurry cluster current
	BecomeCurrent();
	// initialize it
	GLSetupRC();
}


void FlurryCluster::AnimateOneFrame()
{
	// make this flurry cluster current
	BecomeCurrent();

	// Calculate the amount of progress made since the last frame
	// The Flurry core code does this itself, but we do our own calculation
	// here, and if we don't like the answer, we adjust our copy and then
	// tell the timer to lie so that when the core code reads it, it gets
	// the adjusted value.
	double newFrameTime = TimeInSecondsSinceStart();
	double deltaFrameTime = newFrameTime - oldFrameTime;
	if (iMaxFrameProgressInMs > 0) {
		double maxFrameTime = iMaxFrameProgressInMs / 1000.0;
		double overtime = deltaFrameTime - maxFrameTime;

		if (overtime > 0) {
			_RPT3(_CRT_WARN, "Delay: hiding %g seconds (last=%g limit=%g)\n",
				  overtime, deltaFrameTime, maxFrameTime);
			TimeSupport_HideHostDelay(overtime);
			deltaFrameTime -= overtime;
			newFrameTime -= overtime;
		}
	}
	oldFrameTime = newFrameTime;

	// dim the existing screen contents
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.0, 0.0, 0.0, 5.0 * deltaFrameTime);
	glRectd(0, 0, info->sys_glWidth, info->sys_glHeight);

	// then render the new frame blended over what's already there
	GLRenderScene();
	glFlush();
}


FlurryCoreData *FlurryCluster::FlurryAlloc()
{
	int i;
	FlurryCoreData *flurry = (FlurryCoreData *)malloc(sizeof *flurry);
	flurry->flurryRandomSeed = RandFlt(0.0f, 300.0f);
	
	flurry->numStreams = 5;
	flurry->streamExpansion = 100;
	flurry->currentColorMode = tiedyeColorMode;
	
	for (i = 0; i < MAXNUMPARTICLES; i++) {
		flurry->p[i] = (struct Particle *)malloc(sizeof(Particle));
	}
	
	flurry->s = (struct SmokeV *)malloc(sizeof(SmokeV));
	InitSmoke(flurry->s);
	
	flurry->star = (struct Star *)malloc(sizeof(Star));
	InitStar(flurry->star);
	flurry->star->rotSpeed = 1.0;
	
	for (i = 0; i < 64; i++) {
		flurry->spark[i] = (struct Spark *)malloc(sizeof(Spark));
		InitSpark(flurry->spark[i]);
	}

	return flurry;
}


void FlurryCluster::BecomeCurrent()
{
	info = flurryData;
}
