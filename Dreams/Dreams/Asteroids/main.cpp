////////////////////////////////////////////////////////////////////////////
//
// Asteroids Screensaver for XBox Media Center
// Copyright (c) 2005 Joakim Eriksson <je@plane9.com>
//
// The TestXBS framework and program is made by Warren
//
////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "asteroids.h"
#include "XBMC/timer.h"
#include <time.h>

#include "DreamSDK.h"


CAsteroids*	gAsteroids = null;
CRenderD3D		gRender;
CTimer*			gTimer = null;

static BOOL IsDreamPaused = FALSE;

// The states we change that we should restore
DWORD	gStoredState[][2] =
{
	{ D3DRS_ZENABLE, 0},
	{ D3DRS_LIGHTING, 0},
	{ D3DRS_COLORVERTEX, 0},
	{ D3DRS_FILLMODE, 0},
	{ D3DRS_ALPHABLENDENABLE, 0},
	{ D3DRS_MULTISAMPLEANTIALIAS, 0},
	{ D3DRS_ANTIALIASEDLINEENABLE, 0},
	{ 0, 0}
};

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
	gRender.m_D3dDevice = pd3dDevice;
	gRender.m_Width	= rect.right - rect.left;
	gRender.m_Height= rect.bottom - rect.top;

	srand((u32)timeGetTime());
	gAsteroids = new CAsteroids();
	if (!gAsteroids)
		return FALSE;
	gTimer = new CTimer();
	gTimer->Init();
	if (!gRender.RestoreDevice())	
		return FALSE;

	if (!gAsteroids->RestoreDevice(&gRender))
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{
	if (!gAsteroids)
		return;

	gTimer->Update();
	gAsteroids->Update(gTimer->GetDeltaTime());
}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	if (!gAsteroids)
		return 0;

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

		gRender.m_D3dDevice = pd3dDevice;
		gAsteroids->Draw(&gRender);
		gRender.Draw();

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	if (!gAsteroids)
		return;
	gRender.InvalidateDevice();
	gAsteroids->InvalidateDevice(&gRender);
	SAFE_DELETE(gAsteroids);
	SAFE_DELETE(gTimer);
}


////////////////////////////////////////////////////////////////////////////
// 
void CRenderD3D::Init()
{
	m_NumLines = 0;
	m_Verts = null;
}

////////////////////////////////////////////////////////////////////////////
// 
bool CRenderD3D::RestoreDevice()
{
	LPDIRECT3DDEVICE9	d3dDevice = GetDevice();

	for (int i=0; gStoredState[i][0] != 0; i++)
		d3dDevice->SetRenderState((D3DRENDERSTATETYPE)gStoredState[i][0], gStoredState[i][1]);

	m_D3dDevice->CreateVertexBuffer( 2*NUMLINES*sizeof(TRenderVertex), NULL, TRenderVertex::FVF_Flags, D3DPOOL_DEFAULT, &m_VertexBuffer, NULL);
	return true;
}

////////////////////////////////////////////////////////////////////////////
// 
void CRenderD3D::InvalidateDevice()
{
	LPDIRECT3DDEVICE9	d3dDevice = GetDevice();

	SAFE_RELEASE( m_VertexBuffer ); 

	for (int i=0; gStoredState[i][0] != 0; i++)
		d3dDevice->SetRenderState((D3DRENDERSTATETYPE)gStoredState[i][0], gStoredState[i][1]);
}

////////////////////////////////////////////////////////////////////////////
// 
bool CRenderD3D::Draw()
{
	LPDIRECT3DDEVICE9	d3dDevice = GetDevice();

	if (m_NumLines == 0)
		return true;

	m_VertexBuffer->Unlock();
	m_Verts = null;

	// Setup our texture
	d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	 D3DTOP_SELECTARG1);
	d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE);
	d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
	d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
	d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,	 D3DTOP_DISABLE);
	d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE);

	d3dDevice->SetRenderState(D3DRS_ZENABLE,	FALSE);
	d3dDevice->SetRenderState(D3DRS_LIGHTING,	FALSE);
	d3dDevice->SetRenderState(D3DRS_COLORVERTEX,TRUE);
	d3dDevice->SetRenderState(D3DRS_FILLMODE,    D3DFILL_SOLID );

	// Activate line AA
	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);	
	d3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);	
	d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_D3dDevice->SetTexture( 0, null );
	m_D3dDevice->SetStreamSource(	0, m_VertexBuffer, 0, sizeof(TRenderVertex) );
	m_D3dDevice->SetFVF( TRenderVertex::FVF_Flags	);

	CMatrix	m;
	m.Identity();
	m_D3dDevice->SetTransform(D3DTS_PROJECTION, &m);
	m_D3dDevice->SetTransform(D3DTS_VIEW, &m);
	m_D3dDevice->SetTransform(D3DTS_WORLD, &m);

	m_D3dDevice->DrawPrimitive( D3DPT_LINELIST,	0, m_NumLines );

	m_NumLines = 0;
	return true;
}

////////////////////////////////////////////////////////////////////////////
// 
void CRenderD3D::DrawLine(const CVector2& pos1, const CVector2& pos2, const CRGBA& col1, const CRGBA& col2)
{
	if (m_NumLines >= NUMLINES)
	{
		Draw();
	}

	if (m_Verts == null)
	{
		m_VertexBuffer->Lock( 0, 0, (void**)&m_Verts, 0);
	}

	m_Verts->x = pos1.x; m_Verts->y = pos1.y; m_Verts->z = 0.0f; m_Verts->w = 0.0f; m_Verts->col = col1.RenderColor();	m_Verts++;
	m_Verts->x = pos2.x; m_Verts->y = pos2.y; m_Verts->z = 0.0f; m_Verts->w = 0.0f; m_Verts->col = col2.RenderColor();	m_Verts++;
	
	m_NumLines++;
}
