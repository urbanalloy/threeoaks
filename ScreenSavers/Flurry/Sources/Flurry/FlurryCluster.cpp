///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Settings class
//
// Encapsulate the Flurry code away from the non-Flurry-specific screensaver
// code.  Glue clode around the single flurry cluster that's what the core
// code knows how to render.
//
// (c) 2003 - Matt Ginzton (magi@cs.stanford.edu)
// (c) 2006-2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include "FlurryCluster.h"
#include "FlurryPreset.h"
#include "FlurrySettings.h"
#include "TimeSupport.h"

#include <math.h> // unconfuse cpp, which will see it inside extern "C" with __cplusplus defined
extern "C" {
	#include "Core/gl_saver.h"
}

using namespace Flurry;


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

Cluster::Cluster(const ClusterSpec& spec, Settings* settings)
{
	oldFrameTime = TimeInSecondsSinceStart();

	this->settings = settings;

	data = Alloc();

	// specialize
	data->numStreams = spec.nStreams;
	data->currentColorMode = (ColorModes)spec.color;
	data->streamExpansion = spec.thickness;
	data->star->rotSpeed = spec.speed;
}


Cluster::~Cluster()
{
	for (int i = 0; i < MAXNUMPARTICLES; i++) {
		free(data->p[i]);
	}
	free(data->s);
	free(data->star);
	for (int i = 0; i < 64; i++) {
		free(data->spark[i]);
	}
	free(data);
}


void Cluster::SetSize(int width, int height)
{
	// make this flurry cluster current
	BecomeCurrent();
	// resize it
	GLResize(width, height);
}


void Cluster::PrepareToAnimate()
{
	// make this flurry cluster current
	BecomeCurrent();
	// initialize it
	GLSetupRC();
}


void Cluster::AnimateOneFrame()
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
	if (settings->iMaxFrameProgressInMs > 0) {
		double maxFrameTime = settings->iMaxFrameProgressInMs / 1000.0;
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


CoreData *Cluster::Alloc()
{

	CoreData *flurry = (CoreData *)malloc(sizeof *flurry);

	flurry->flurryRandomSeed = RandFlt(0.0f, 300.0f);
	flurry->numStreams = 5;
	flurry->streamExpansion = 100;
	flurry->currentColorMode = tiedyeColorMode;
	
	for (int i = 0; i < MAXNUMPARTICLES; i++) {
		flurry->p[i] = (struct Particle *)malloc(sizeof(Particle));
	}
	
	flurry->s = (struct SmokeV *)malloc(sizeof(SmokeV));
	InitSmoke(flurry->s);
	
	flurry->star = (struct Star *)malloc(sizeof(Star));
	InitStar(flurry->star);
	flurry->star->rotSpeed = 1.0;
	
	for (int i = 0; i < 64; i++) {
		flurry->spark[i] = (struct Spark *)malloc(sizeof(Spark));
		InitSpark(flurry->spark[i]);
	}

	return flurry;
}


void Cluster::BecomeCurrent()
{
	info = data;
}
