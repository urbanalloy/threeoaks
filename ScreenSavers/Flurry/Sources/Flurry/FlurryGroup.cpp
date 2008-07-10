///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Group class
//
// One flurry group is a list of clusters.
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

#include "FlurryGroup.h"

#include <math.h> // unconfuse cpp, which will see it inside extern "C" with __cplusplus defined
extern "C" {
	#include "Core/gl_saver.h"
	#include "Core/Texture.h"
}

using namespace Flurry;

///////////////////////////////////////////////////////////////////////////
//
// public functions

/*
 * Note: the Flurry base code keeps everything in a global variable named
 * info.  We want to instance it, for multi-monitor support (several separate
 * Flurries), so we allocate several such structures, but to avoid changing
 * the base code, we set info = current->globals before calling into it.
 * Obviously, not thread safe.
 */
Group::Group(int preset, Settings *settings)
{
	bugBlockMode = settings->bugBlockMode;

	if (preset > (signed)settings->visuals.size()) {
		_RPT2(_CRT_WARN, "Invalid preset %d (max %d); using default\n", preset, settings->visuals.size());
		preset = 0;
	}

	for (int i = 0; i < (signed)settings->visuals[preset]->clusters.size(); i++) {
		clusters.push_back(new Cluster(settings->visuals[preset]->clusters[i], settings));
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
	if (!bugBlockMode) {
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
