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

#pragma once

#include <vector>
using std::vector;
#include "FlurryCluster.h"
#include "FlurrySettings.h"

namespace Flurry {

class Group {
		
		private:
			vector<Cluster*> clusters;
			Settings *settings;

		public:
			Group(int preset, Settings *settings);
			~Group();

			void SetSize(int width, int height);
			void PrepareToAnimate(void);
			void AnimateOneFrame(void);

	};

}
