/*
 * Flurry for Windows.
 *
 * One flurry group is a list of flurry clusters.
 *
 * Created 2/23/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#include "FlurryGroup.h"
#include "FlurryPreset.h"
#include "FlurrySettings.h"

#include <math.h> // unconfuse cpp, which will see it inside extern "C" with __cplusplus defined
extern "C" {
	#include "Core/gl_saver.h"
	#include "Core/Texture.h"
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

FlurryGroup::FlurryGroup(int preset)
{
	if (preset > (signed)g_visuals.size()) {
		_RPT2(_CRT_WARN, "Invalid preset %d (max %d); using default\n",
			  preset, g_visuals.size());
		preset = 0;
	}
	FlurrySpec *visual = g_visuals[preset];

	for (int i = 0; i < (signed)visual->clusters.size(); i++) {
		clusters.push_back(new FlurryCluster(visual->clusters[i]));
	}
}


FlurryGroup::~FlurryGroup(void)
{
	for (int i = 0; i < (signed)clusters.size(); i++) {
		delete clusters[i];
	}
}


void
FlurryGroup::SetSize(int width, int height)
{
	for (int i = 0; i < (signed)clusters.size(); i++) {
		clusters[i]->SetSize(width, height);
	}
}


void
FlurryGroup::PrepareToAnimate(void)
{
	if (!iBugBlockMode) {
		// Found this by accident; looks cool.  Freakshow option #2.
		MakeTexture();
	}

	for (int i = 0; i < (signed)clusters.size(); i++) {
		clusters[i]->PrepareToAnimate();
	}
}


void
FlurryGroup::AnimateOneFrame(void)
{
	for (int i = 0; i < (signed)clusters.size(); i++) {
		clusters[i]->AnimateOneFrame();
	}
}
