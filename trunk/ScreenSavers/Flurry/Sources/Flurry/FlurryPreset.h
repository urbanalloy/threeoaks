///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Preset class
//
// Flurry preset specification
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

#include <atlbase.h>
#include <vector>
using std::vector;

namespace Flurry {

	class ClusterSpec {

		public:
			int nStreams;
			int color;
			float thickness;
			float speed;
	};

	class Spec {

			private:
				bool ParseFromString(char *format);

		public:
			Spec(char *format);
			~Spec();

			bool valid;
			char *name;
			vector<ClusterSpec> clusters;

			bool WriteToString(char *format, int formatLen);	
	};
}

using namespace Flurry;

/*
* Exported globals from this module
*/
extern vector<int> g_multiMonPreset;
extern vector<Spec *> g_visuals;

/*
 * Exported functions
 */

void Visuals_Read(CRegKey& reg);
void Visuals_Write(CRegKey& reg);

void Monitors_Read(CRegKey& reg);
void Monitors_Write(CRegKey& reg);
