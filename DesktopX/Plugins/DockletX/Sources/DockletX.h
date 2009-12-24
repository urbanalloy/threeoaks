///////////////////////////////////////////////////////////////////////////////////////////////
//
// DockletX - Docklet support plugin for DesktopX
//
// Copyright (c) 2006-2009, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include <vector>
#include <map>
using namespace std;

#include "Docklet.h"

#define PLUGIN_NAME			"DockletX"
#define PLUGIN_AUTHOR		"Julien Templier"
#define PLUGIN_CAPABILITY	"ObjectDock Docklet Support"
#define PLUGIN_PLATFORM		SD_OS_XP
#define PLUGIN_HOSTS		SD_HOST_DESKTOPX | SD_HOST_OBJECTBAR

//////////////////////////////////////////////////////////////////////////
// Plugin configuration
typedef struct {
	char path[MAX_PATH];
	Docklet::DOCKLET_INFO info;
} DOCKLET_LIST;

typedef struct
{
	std::vector<DOCKLET_LIST> docklets;
	int index;
} PLUGIN_DATA;