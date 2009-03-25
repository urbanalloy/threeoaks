/*
* Shade Worm Screen Saver for XBox Media Center
* Copyright (c) 2005 MrC
*
* Ver 1.0 27 Feb 2005	MrC
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include "ShadeWorm.h"

#include "DreamSDK.h"

static BOOL IsDreamPaused = FALSE;

ShadeWorm_c* g_shadeWorm = NULL;


//////////////////////////////////////////////////////////////////////////
// Query default settings for the Dream
// return the value for the type of setting
QUERY_DEFAULT_SETTINGS
{
	switch (type)
	{
	case QUERY_D3DFORMAT:
	case QUERY_VIDEOTEXTURE:
	case QUERY_4:
	case QUERY_5:
		return value;
		break;		

	case QUERY_SCREENUPDATEFREQUENCY:
		return DEFAULT_SCREENUPDATEFREQUENCY;
		break;
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
// Notification when:
//  - started
//	- paused
//	- resumed
//  - stopped
//	- settings have changed
NOTIFICATION_PROCEDURE
{
	switch(type) {

		default: 
			break;

		case NOTIFY_STARTED:
			IsDreamPaused = FALSE;
			break;

		case NOTIFY_STOPPED:
			IsDreamPaused = TRUE;
			break;

		case NOTIFY_PAUSED:
			IsDreamPaused = TRUE;
			break;

		case NOTIFY_PLAYING:
			IsDreamPaused = FALSE;
			break;

		case NOTIFY_5:
			break;

		case NOTIFY_SETTINGS_CHANGED:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Setup the plugin
// load Direct3D resources, update registry keys
SETUP_PLUGIN
{
	g_shadeWorm = new ShadeWorm_c;
	g_shadeWorm->Create(pd3dDevice, rect.right - rect.left, rect.bottom - rect.top);

	srand(timeGetTime());

	if (g_shadeWorm)
	{
		if (!g_shadeWorm->Start())
		{
			// Creation failure
			g_shadeWorm->Stop();
			delete g_shadeWorm;
			g_shadeWorm = NULL;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{

}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	if (!g_shadeWorm)
		return 0;

	pd3dDevice->BeginScene();

		g_shadeWorm->SetD3DDevice(pd3dDevice);

		g_shadeWorm->Render();

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	if (g_shadeWorm)
	{
		g_shadeWorm->Stop();
		delete g_shadeWorm;
		g_shadeWorm = NULL;
	}
}
