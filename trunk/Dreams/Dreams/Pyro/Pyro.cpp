///////////////////////////////////////////////////////////////////////////////////////////////
//
// Pyro Dream
//
// Copyright (c) 2007, Three Oaks Crossing
// All rights reserved.
//
// Adapted from the Pyro Screensaver for XBox Media Center
// Copyright (c) 2004 Team XBMC
//
// Ver 1.0 15 Nov 2004	Chris Barnett (Forza)
//
// Adapted from the Pyro screen saver by
//  Jamie Zawinski <jwz@jwz.org>
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

#include "Utils\stdafx.h"

#include <DreamSDK.h>
#include <stdio.h>
#include <math.h>

#include "Pyro.h"

int g_renderWidth;
int g_renderHeight;

static BOOL IsDreamPaused = FALSE;
BOOL g_initialized = FALSE;


//////////////////////////////////////////////////////////////////////////
// Projectiles
//////////////////////////////////////////////////////////////////////////

static int myrand()
{
	return (rand() << 15) + rand();
}


static struct projectile *get_projectile (void)
{
	struct projectile *p;

	if (free_projectiles)
	{
		p = free_projectiles;
		free_projectiles = p->next_free;
		p->next_free = 0;
		p->dead = false;
		return p;
	}
	else
		return 0;
}

static void free_projectile (struct projectile *p)
{
	p->next_free = free_projectiles;
	free_projectiles = p;
	p->dead = true;
}

static void launch (int xlim, int ylim, int g)
{
	struct projectile *p = get_projectile ();
	int x, dx, xxx;
	double red, green, blue;
	if (! p) return;

	do {
		x = (myrand() % xlim);
		dx = 30000 - (myrand() % 60000);
		xxx = x + (dx * 200);
	} while (xxx <= 0 || xxx >= xlim);

	p->x = x;
	p->y = ylim;
	p->dx = dx;
	p->size = 20000;
	p->decay = 0;
	p->dy = (myrand() % 10000) - 20000;
	p->fuse = ((((myrand() % 500) + 100) * abs (p->dy / g)) / 1000);
	p->primary = true;

	hsv_to_rgb ((double)(myrand() % 360)/360.0, 1.0, 255.0,	&red, &green, &blue);
	//printf("New Projectile at (%d, %d), d(%d, %d), colour(%d,%d,%d)", x, ylim, dx, p->dy, (int)red, (int)green, (int)blue);
	p->colour = D3DCOLOR_XRGB((int)red,(int)green,(int)blue);
	return;
}

static struct projectile *shrapnel(struct projectile *parent)
{
	struct projectile *p = get_projectile ();
	if (! p) return 0;

	p->x = parent->x;
	p->y = parent->y;
	p->dx = (myrand() % 5000) - 2500 + parent->dx;
	p->dy = (myrand() % 5000) - 2500 + parent->dy;
	p->decay = (myrand() % 50) - 60;
	p->size = (parent->size * 2) / 3;
	p->fuse = 0;
	p->primary = false;
	p->colour = parent->colour;
	return p;
}

void hsv_to_rgb (double hue, double saturation, double value, 
				 double *red, double *green, double *blue)
{
	double f, p, q, t;

	if (saturation == 0.0)
	{
		*red   = value;
		*green = value;
		*blue  = value;
	}
	else
	{
		hue *= 6.0; // 0 -> 360 * 6

		if (hue == 6.0)
			hue = 0.0;

		f = hue - (int) hue;
		p = value * (1.0 - saturation);
		q = value * (1.0 - saturation * f);
		t = value * (1.0 - saturation * (1.0 - f));

		switch ((int) hue)
		{
		case 0:
			*red = value;
			*green = t;
			*blue = p;
			break;

		case 1:
			*red = q;
			*green = value;
			*blue = p;
			break;

		case 2:
			*red = p;
			*green = value;
			*blue = t;
			break;

		case 3:
			*red = p;
			*green = q;
			*blue = value;
			break;
		case 4:
			*red = t;
			*green = p;
			*blue = value;
			break;

		case 5:
			*red = value;
			*green = p;
			*blue = q;
			break;
		}
	}
}




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
			return 15;
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

		case 5:
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

	g_renderWidth = rect.right - rect.left;
	g_renderHeight = rect.bottom - rect.top;

	how_many = 1000;
	frequency = 5;
	scatter = 20;

	projectiles = 0;
	free_projectiles = 0;
	projectiles = (struct projectile *) calloc(how_many, sizeof (struct projectile));
	for (int i = 0; i < how_many; i++)
		free_projectile (&projectiles [i]);

	// If we are called with no device initialized, init on first call to render...
	if (pd3dDevice == NULL) {
		return TRUE;
	}

	return InitD3D(pd3dDevice);

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

	if (pd3dDevice == NULL)		
		return 0;

	if (!g_initialized)
		InitD3D(pd3dDevice);

	float width = (float)(rect.right - rect.left);
	float height = (float)(rect.bottom - rect.top);

	float screenWidth = (float)(screen.right - screen.left);
	float screenHeight = (float)(screen.bottom - screen.top);

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

	static int xlim, ylim, real_xlim, real_ylim;
	int g = 100;
	int i;

	if ((myrand() % frequency) == 0)
	{
		real_xlim = g_renderWidth;
		real_ylim = g_renderHeight;
		xlim = real_xlim * 1000;
		ylim = real_ylim * 1000;
		launch ( xlim, ylim, g);
	}

	for (i = 0; i < how_many; i++)
	{
		struct projectile *p = &projectiles [i];
		int old_x, old_y, old_size;
		int size, x, y;
		if (p->dead) continue;
		old_x = p->x >> 10;
		old_y = p->y >> 10;
		old_size = p->size >> 10;
		size = (p->size += p->decay) >> 10;
		x = (p->x += p->dx) >> 10;
		y = (p->y += p->dy) >> 10;
		p->dy += (p->size >> 6);
		if (p->primary) p->fuse--;

		if ((p->primary ? (p->fuse > 0) : (p->size > 0)) &&
			x < real_xlim &&
			y < real_ylim &&
			x > 0 &&
			y > 0)
		{
			DrawRectangle(pd3dDevice, x, y, size, size, p->colour);
		}
		else
		{
			free_projectile (p);
		}

		if (p->primary && p->fuse <= 0)
		{
			int j = (myrand() % scatter) + (scatter/2);
			while (j--)	shrapnel(p);
		}
	}

	pd3dDevice->EndScene();
						
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugins resources
FREE_PLUGIN
{
	free(projectiles);
}


//////////////////////////////////////////////////////////////////////////
BOOL InitD3D( LPDIRECT3DDEVICE9 pd3dDevice )
{
	D3DXMATRIX Ortho2D;	
	D3DXMATRIX Identity;
	D3DXMatrixOrthoLH(&Ortho2D, (float)g_renderWidth, (float)g_renderHeight, 0.0f, 1.0f);
	D3DXMatrixIdentity(&Identity);

	pd3dDevice->SetTransform(D3DTS_PROJECTION, &Ortho2D);
	pd3dDevice->SetTransform(D3DTS_WORLD, &Identity);
	pd3dDevice->SetTransform(D3DTS_VIEW, &Identity);

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	g_initialized = TRUE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void DrawRectangle(LPDIRECT3DDEVICE9 pd3dDevice, int x, int y, int w, int h, D3DCOLOR dwColour)
{
	//Store each point of the triangle together with it's colour
	MYCUSTOMVERTEX cvVertices[] =
	{
		{(float) x, (float) y+h, 0.0f, 1.0f, dwColour,},
		{(float) x, (float) y, 0.0f, 1.0f, dwColour,},
		{(float) x+w, (float) y, 0.0f, 1.0f, dwColour,},
		{(float) x+w, (float) y+h, 0.0f, 1.0f, dwColour,},
	};

	// Draw it
	pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pd3dDevice->SetPixelShader(0);
	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &cvVertices, sizeof(MYCUSTOMVERTEX));

	return;
}

