///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : TimeSupport class
//
// Handle time-related things in a way that lets the existing Flurry code
// be happy without modifications.  Includes a rate limiter.
//
// (c) 2003 Matt Ginzton (magi@cs.stanford.edu)
// (c) 2006-2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <crtdbg.h>
#include <math.h> // unconfuse cpp, which will see it inside extern "C" with __cplusplus defined
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
