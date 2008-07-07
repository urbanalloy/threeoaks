///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Group class
//
// One flurry group is a list of clusters.
//
// (c) 2003 Matt Ginzton (magi@cs.stanford.edu)
// (c) 2006-2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include "FlurryGroup.h"
#include "FlurryPreset.h"
#include "FlurrySettings.h"

#include <math.h> // unconfuse cpp, which will see it inside extern "C" with __cplusplus defined
extern "C" {
	#include "Core/gl_saver.h"
	#include "Core/Texture.h"
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

Group::Group(int preset)
{
	if (preset > (signed)g_visuals.size()) {
		_RPT2(_CRT_WARN, "Invalid preset %d (max %d); using default\n",
			  preset, g_visuals.size());
		preset = 0;
	}
	Spec *visual = g_visuals[preset];

	for (int i = 0; i < (signed)visual->clusters.size(); i++) {
		clusters.push_back(new Cluster(visual->clusters[i]));
	}
}


Group::~Group(void)
{
	for (int i = 0; i < (signed)clusters.size(); i++) {
		delete clusters[i];
	}
}


void Group::SetSize(int width, int height)
{
	for (int i = 0; i < (signed)clusters.size(); i++) {
		clusters[i]->SetSize(width, height);
	}
}


void Group::PrepareToAnimate(void)
{
	if (!iBugBlockMode) {
		// Found this by accident; looks cool.  Freakshow option #2.
		MakeTexture();
	}

	for (int i = 0; i < (signed)clusters.size(); i++) {
		clusters[i]->PrepareToAnimate();
	}
}


void Group::AnimateOneFrame(void)
{
	for (int i = 0; i < (signed)clusters.size(); i++) {
		clusters[i]->AnimateOneFrame();
	}
}
