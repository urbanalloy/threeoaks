///////////////////////////////////////////////////////////////////////////////////////////////
//
// MatrixTrails Dream
//
// Copyright (c) 2008, Julien Templier
// All rights reserved.
//
// Adapted from the Matrix Trails Screensaver for XBox Media Center
// Copyright (c) 2005 Joakim Eriksson <je@plane9.com>
//
// Thanks goes to Warren for his 'TestXBS' program!
// Matrix Symbol Font by Lexandr (mCode 1.5 - http://www.deviantart.com/deviation/2040700/)
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

#include "Utils/stdafx.h"
#include <DreamSDK.h>

#include "main.h"
#include "matrixtrails.h"
#include "XBMC/timer.h"
#include <time.h>

CMatrixTrails*	gMatrixTrails = null;
CTimer*			gTimer = null;
CConfig			gConfig;

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
	gConfig.SetDefaults();
	gConfig.LoadSettings();

	gMatrixTrails = new CMatrixTrails();
	if (!gMatrixTrails)
		return FALSE;

	gTimer = new CTimer();
	gTimer->Init();
	if (!gMatrixTrails->RestoreDevice(pd3dDevice, rect))
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{
	if (!gMatrixTrails)
		return;

	gTimer->Update();
	gMatrixTrails->Update(gTimer->GetDeltaTime()/4);
}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	if (!gMatrixTrails || pd3dDevice == NULL)
		return -1;

	//pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_COLORVALUE(0, 0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

		gMatrixTrails->Draw(pd3dDevice);

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	if (!gMatrixTrails)
		return;

	gMatrixTrails->InvalidateDevice();

	SAFE_DELETE(gMatrixTrails);
	SAFE_DELETE(gTimer);
}


////////////////////////////////////////////////////////////////////////////
//
void CConfig::SetDefaults()
{
	m_CharDelayMin	= 0.015f;
	m_CharDelayMax	= 0.060f;
	m_FadeSpeedMin	= 1.0f;
	m_FadeSpeedMax	= 1.5f;
	m_NumColumns	= 50;
	m_NumRows		= 40;
	m_CharCol.Set(0.0f, 1.0f, 0.0f, 1.0f);

	m_NumChars		= 32;
	m_CharSizeTex.x = 32.0f/TEXTURESIZE;
	m_CharSizeTex.y = 26.0f/TEXTURESIZE;
}

////////////////////////////////////////////////////////////////////////////
void CConfig::LoadSettings()
{
	// Check for values in registry
}
