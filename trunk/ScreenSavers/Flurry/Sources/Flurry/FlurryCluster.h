/*
 * Flurry for Windows.
 *
 * Encapsulate the Flurry code away from the non-Flurry-specific screensaver
 * code.  Glue clode around the single flurry cluster that's what the core
 * code knows how to render.
 *
 * Created 2/23/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#ifndef __FLURRYCLUSTER_H_
#define __FLURRYCLUSTER_H_

#include <vector>
using std::vector;
#include "FlurryPreset.h"


typedef struct global_info_t FlurryCoreData;

class FlurryCluster {
	public:
		FlurryCluster(const FlurryClusterSpec& spec);
		~FlurryCluster();

		void SetSize(int width, int height);
		void PrepareToAnimate(void);
		void AnimateOneFrame(void);

	private:
		FlurryCoreData *FlurryAlloc(void);
		void BecomeCurrent(void);

		FlurryCoreData *flurryData;
		double oldFrameTime;
};


#endif // __FLURRYCLUSTER_H_
