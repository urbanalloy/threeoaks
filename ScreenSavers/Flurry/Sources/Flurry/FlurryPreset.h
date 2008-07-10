///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Preset class
//
// Flurry preset specification
//
// Copyright (c) 2003, Matt Ginzton
// Copyright (c) 2005-2008, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// o Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// o Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// o Neither the name of the author nor the names of its contributors may
//   be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
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
