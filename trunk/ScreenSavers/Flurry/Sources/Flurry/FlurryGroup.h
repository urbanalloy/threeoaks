/*
 * Flurry for Windows.
 *
 * One flurry group is a list of clusters.
 *
 * Created 2/23/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#ifndef __FLURRYGROUP_H_
#define __FLURRYGROUP_H_

#include <vector>
using std::vector;
#include "FlurryCluster.h"


class FlurryGroup {
	public:
		FlurryGroup(int preset);
		~FlurryGroup();

		void SetSize(int width, int height);
		void PrepareToAnimate(void);
		void AnimateOneFrame(void);

	private:
		vector<FlurryCluster *> clusters;
};


#endif // __FLURRYGROUP_H_
