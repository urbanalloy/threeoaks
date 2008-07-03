///////////////////////////////////////////////////////////////////////////////////////////////
//
// TodoList : a ToDo Docklet for ObjectDock
//
// (c) 2004 - Julien Templier
// Parts from Weather Docklet by Jeff Bargmann
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

#ifndef _TODOLIST_H
#define _TODOLIST_H

#include <Docklet/Docklet.h>
#include <Docklet/Console.h>
#include "Todo.h"

///////////////////////////////////////////////////////////////////////////////////////////////

// Include version.h for build (automatically pull last subversion revision number)
#include "version.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// Docklet 'Data' struct.
typedef struct
{
	Docklet*  docklet;
	Todo*	  todo;
	BOOL	  configuring;

	ODConsole* console;

} DOCKLET_DATA;

#endif