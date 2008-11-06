///////////////////////////////////////////////////////////////////////////////////////////////
//
// BioGenesis Dream
//
// Copyright (c) 2008, Julien Templier
// All rights reserved.
//
// BioGenesis Screensaver for XBox Media Center
// Copyright (c) 2004 Team XBMC
//
// Ver 1.0 2007-02-12 by Asteron  http://asteron.projects.googlepages.com/home
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


//////////////////////////////////////////////////////////////////////////
// Grid & Cells
//////////////////////////////////////////////////////////////////////////

struct Cell
{
	DWORD color; // The cell color.
	short lifetime;
	BYTE nextstate, state; 
};


#define DEAD 0
#define ALIVE 1
#define COLOR_TIME 0
#define COLOR_COLONY 1
#define COLOR_NEIGHBORS 2

struct Grid
{
	int minSize;
	int maxSize;
	int width;
	int height;
	int spacing;
	int resetTime;
	int cellSizeX;
	int cellSizeY;
	int colorType;
	int ruleset;
	int frameCounter;
	int maxColor;
	int presetChance;
	int allowedColoring;
	int cellLineLimit;
	DWORD palette[800];
	Cell * cells;
	Cell * fullGrid;
};

DWORD COLOR_TIMES[] = {  
	D3DCOLOR_RGBA(30,30,200,255),
	D3DCOLOR_RGBA(120,10,255,255),
	D3DCOLOR_RGBA(50,100,250,255),
	D3DCOLOR_RGBA(0,250,200,255),
	D3DCOLOR_RGBA(60,250,40,255),
	D3DCOLOR_RGBA(244,200,40,255),
	D3DCOLOR_RGBA(250,100,30,255),
	D3DCOLOR_RGBA(255,10,20,255)
};
int MAX_COLOR = sizeof(COLOR_TIMES)/sizeof(DWORD);


// Settings
#define GRID_RESET_TIME 2000
#define GRID_RESET_TIME_MIN 500
#define GRID_RESET_TIME_MAX 8000
#define GRID_RESET_TIME_FREQ 500

#define GRID_PRESET_CHANCE 30
#define GRID_PRESET_CHANCE_MIN 0
#define GRID_PRESET_CHANCE_MAX 100
#define GRID_PRESET_CHANCE_FREQ 10

#define GRID_MIN_SIZE 50
#define GRID_MIN_SIZE_MIN 20
#define GRID_MIN_SIZE_MAX 100
#define GRID_MIN_SIZE_FREQ 10

#define GRID_MAX_SIZE 250
#define GRID_MAX_SIZE_MIN 200
#define GRID_MAX_SIZE_MAX 300
#define GRID_MAX_SIZE_FREQ 10

#define GRID_CELL_LINE_LIMIT 3
#define GRID_CELL_LINE_LIMIT_MIN 1
#define GRID_CELL_LINE_LIMIT_MAX 10
#define GRID_CELL_LINE_LIMIT_FREQ 1

#define GRID_ALLOWED_COLORING 7

#define GRID_SPACING 1

void DrawRectangle(int x, int y, int w, int h, D3DCOLOR dwColour);
void CreateGrid();
void reducePalette();

#define IS_ALLOWED(coloring, color) \
	!(coloring & (1 << color))

//////////////////////////////////////////////////////////////////////////
// DirectX & Misc
//////////////////////////////////////////////////////////////////////////
LPDIRECT3DDEVICE9 g_pd3dDevice;

struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex.
	DWORD color; // The vertex colour.
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

#define LerpColor(s,e,r) (0xFF000000+((int)(((((e)&0xFF0000)*r)+((s)&0xFF0000)*(1.0f-r)))&0xFF0000)+((int)(((((e)&0xFF00)*r)+((s)&0xFF00)*(1.0f-r)))&0xFF00)+((int)(((((e)&0xFF)*r)+((s)&0xFF)*(1.0f-r)))&0xFF))

D3DCOLOR HSVtoRGB( float h, float s, float v );

//////////////////////////////////////////////////////////////////////////
// Dream related
//////////////////////////////////////////////////////////////////////////

#define DREAM_NAME "BioGenesis"

// Size of the version text
#define VERSION_SIZE 30

void LoadSettings();
void SaveSettings();
void SetDefaults();

INT_PTR CALLBACK ConfigureDream(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);