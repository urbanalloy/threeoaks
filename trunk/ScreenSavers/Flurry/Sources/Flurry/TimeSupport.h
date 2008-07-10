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

#pragma once

extern void TimeSupport_Init(void);
extern void TimeSupport_HideHostDelay(double delay);