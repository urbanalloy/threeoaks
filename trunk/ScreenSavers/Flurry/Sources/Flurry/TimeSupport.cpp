///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : TimeSupport class
//
// Handle time-related things in a way that lets the existing Flurry code
// be happy without modifications.  Includes a rate limiter.
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

#include <windows.h>
#include <crtdbg.h>
#include <math.h> // un-confuse cpp, which will see it inside extern "C" with __cplusplus defined
extern "C" {
	#include "Core/gl_saver.h"
}


static double g_delay = 0.0;

/*
 *
 * TimeSupport_Init():
 *
 * Zero the running timer that the Flurry core code maintains.
 */
void TimeSupport_Init(void)
{
	OTSetup();
	g_delay = 0.0;
}


/*
 * TimeSupport_HideHostDelay()
 *
 * Pretend some time that passed, didn't.
 */

void
TimeSupport_HideHostDelay(double delay)
{
	g_delay += delay;
	_RPT2(_CRT_WARN, "  total delay=%g sec, fake runtime=%g sec\n",
		  delay, TimeInSecondsSinceStart());
}


/*
 *
 * CurrentTime():
 *
 * The Flurry base code assumes this Macintosh API function -- fake it.
 *
 * In fact, it's a good place to lie to the Flurry code to adjust the
 * behavior that depends on frame rate.  All the Flurry code depends on
 * the return value of TimeInSecondsSinceStart(), which depends on
 * CurrentTime().
 */
extern "C" double CurrentTime(void)
{
	return (timeGetTime() / 1000.0) - g_delay;
}
