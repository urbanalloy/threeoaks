///////////////////////////////////////////////////////////////////////////////////////////////
//
// Stars Dream
//
// Copyright (c) 2008, Julien Templier
// All rights reserved.
//
// Adapted from the Stars Screensaver for XBox Media Center
// Copyright (c) 2004 Team XBMC
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
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Utils/stdafx.h"

struct ST_SETTINGS
{	
	int iWidth;
	int iHeight;

	int   iNumStars;
	float fGamma;
	float fBrightness;
	float fSpeed;
	float fZoom;
	float fExpanse;
	bool  bMoveCamera;
};

#define D3DFVF_TLVERTEX D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1

// Size of the version text
#define VERSION_SIZE 30

// MAX/MIN for each slider
#define STARS_DEFAULT 2500
#define STARS_MIN 500
#define STARS_MAX 5000
#define STARS_FREQ 500

#define GAMMA_DEFAULT 1.0f
#define GAMMA_MIN 1
#define GAMMA_MAX 10
#define GAMMA_FREQ 1
#define GAMMA_MULT 10.0f

#define BRIGHTNESS_DEFAULT 0.2f
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 10
#define BRIGHTNESS_FREQ 1
#define BRIGHTNESS_MULT 10.0f

#define SPEED_DEFAULT 10.0f
#define SPEED_MIN 1
#define SPEED_MAX 50
#define SPEED_FREQ 5
#define SPEED_MULT 1.0f

#define ZOOM_DEFAULT 1.0f
#define ZOOM_MIN 5
#define ZOOM_MAX 20
#define ZOOM_FREQ 1
#define ZOOM_MULT 10.0f

#define EXPANSE_DEFAULT 1.5f
#define EXPANSE_MIN 5
#define EXPANSE_MAX 50
#define EXPANSE_FREQ 5
#define EXPANSE_MULT 10.0f

#define CAMERA_DEFAULT false

void SetDefaults();
void LoadSettings();
void SaveSettings();
INT_PTR CALLBACK ConfigureDream(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
