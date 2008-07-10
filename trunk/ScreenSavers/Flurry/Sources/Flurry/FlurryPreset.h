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
#include <string>

using std::string;
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
			void ParseTemplate();

			int ColorModeFromName(char *colorName);
			const char* ColorModeToName(int colorMode);

			const static char *colorTable[];			
		
		public:
			Spec(char *format);
			Spec(Spec& arg);
			~Spec();

			bool isValid;
			string name;
			string specTemplate;

			vector<ClusterSpec> clusters;

			bool WriteToString(char *format, int formatLen);	
	};

}
