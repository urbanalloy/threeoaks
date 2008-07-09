///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Settings class
//
// Encapsulate the Flurry code away from the non-Flurry-specific screensaver
// code.  Glue clode around the single flurry cluster that's what the core
// code knows how to render.
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

#include "FlurryPreset.h"
#include "FlurrySettings.h"

typedef struct global_info_t CoreData;

namespace Flurry {

	class Cluster {

		private:
			CoreData *Alloc(void);
			void BecomeCurrent(void);

			CoreData *data;
			double oldFrameTime;

			DWORD maxFrameProgressInMs;

		public:
			Cluster(const ClusterSpec& spec, Settings *settings);
			~Cluster();

			void SetSize(int width, int height);
			void PrepareToAnimate(void);
			void AnimateOneFrame(void);
	};

}
