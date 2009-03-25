///////////////////////////////////////////////////////////////////////////////////////////////
//
// PingPong Dream
//
// Copyright (c) 2008, Julien Templier
// All rights reserved.
//
// Adapted from the PingPong Screensaver for XBox Media Center
// Copyright (c) 2005 Joakim Eriksson <je@plane9.com>
//
// Thanks goes to Warren for his 'TestXBS' program!
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Main.h"
#include "PingPong.h"
#include "XBMC/timer.h"
#include <time.h>

#include "DreamSDK.h"

CPingPong*		gPingPong = null;
CRenderD3D		gRender;
CTimer*			gTimer = null;
CRGBA			gCol[3];

static BOOL IsDreamPaused = FALSE;

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
	LoadSettings();

	gRender.m_D3dDevice = pd3dDevice;
	gRender.m_Width	= rect.right - rect.left;
	gRender.m_Height= rect.bottom - rect.top;

	srand((u32)time(null));
	gPingPong = new CPingPong();
	if (!gPingPong)
		return FALSE;

	gPingPong->m_Paddle[0].m_Col = gCol[0];
	gPingPong->m_Paddle[1].m_Col = gCol[1];
	gPingPong->m_Ball.m_Col = gCol[2];

	gTimer = new CTimer();
	gTimer->Init();
	if (!gPingPong->RestoreDevice(&gRender))
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{
	if (!gPingPong)
		return;
		
	gTimer->Update();
	gPingPong->Update(gTimer->GetDeltaTime());
}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	if (!gPingPong)
		return 0;

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

		gRender.m_D3dDevice = pd3dDevice;
		gPingPong->Draw(&gRender);

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	if (!gPingPong)
		return;

	gPingPong->InvalidateDevice(&gRender);

	SAFE_DELETE(gPingPong);
	SAFE_DELETE(gTimer);
}

////////////////////////////////////////////////////////////////////////////
void LoadSettings()
{
	// Set up the defaults
	SetDefaults();
}

////////////////////////////////////////////////////////////////////////////
// set any default values for your screensaver's parameters
void SetDefaults()
{
	for (int i=0; i<3; i++)
		gCol[i].Set(1.0f, 1.0f, 1.0f, 1.0f);	
}


