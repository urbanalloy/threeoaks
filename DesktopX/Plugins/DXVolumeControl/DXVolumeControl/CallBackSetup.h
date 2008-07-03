///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXVolumeControl - Vista Master Volume Control for DesktopX
//
// Copyright (c)2006-2007, Julien Templier
//
// This work is licensed under the Creative Commons
// Attribution-ShareAlike License. To view a copy of this license, visit
// http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VistaVolumeCallback.h"

extern CVistaVolumeCallback* pVistaVolumeCallback;

void RegisterCallBack();
void UnregisterCallBack();