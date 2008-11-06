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

#pragma once

#include "Utils\stdafx.h"

#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

typedef struct _D3DTLVERTEX {
	float sx; /* Screen coordinates */
	float sy;
	float sz;
	float rhw; /* Reciprocal of homogeneous w */
	D3DCOLOR color; /* Vertex color */
	float tu; /* Texture coordinates */
	float tv;
	_D3DTLVERTEX() { }
	_D3DTLVERTEX(const D3DVECTOR& v, float _rhw,
		D3DCOLOR _color, 
		float _tu, float _tv)
	{ sx = v.x; sy = v.y; sz = v.z; rhw = _rhw;
	color = _color; 
	tu = _tu; tv = _tv;
	}
} D3DTLVERTEX, *LPD3DTLVERTEX;


#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL; // Vertices Buffer

struct MYCUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex.
	DWORD colour; // The vertex colour.
};

BOOL InitD3D( LPDIRECT3DDEVICE9 pd3dDevice );
void DrawRectangle(LPDIRECT3DDEVICE9 pd3dDevice, int x, int y, int w, int h, D3DCOLOR dwColour);

//////////////////////////////////////////////////////////////////////////
// Projectile related
struct projectile {
	int x, y;	/* position */
	int dx, dy;	/* velocity */
	int decay;
	int size;
	int fuse;
	bool primary;
	bool dead;
	//XColor color;
	//D3DCOLORVALUE colour;
	D3DCOLOR colour;
	struct projectile *next_free;
};

static struct projectile *projectiles, *free_projectiles;
static unsigned int default_fg_pixel;
static int how_many, frequency, scatter;

static struct projectile *get_projectile(void);
static void free_projectile(struct projectile *p);
static void launch(int xlim, int ylim, int g);
static struct projectile *shrapnel(struct projectile *parent);

void hsv_to_rgb(double hue, double saturation, double value, double *red, double *green, double *blue);