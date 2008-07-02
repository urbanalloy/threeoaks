/*
 * Flurry for Windows.
 *
 * Handle time-related things in a way that lets the existing Flurry code
 * be happy without modifications.  Includes a rate limiter.
 *
 * Created 8/10/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#ifndef _TIMESUPPORT_H_
#define _TIMESUPPORT_H_


extern void TimeSupport_Init(void);
extern void TimeSupport_HideHostDelay(double delay);


#endif // _TIMESUPPORT_H_
