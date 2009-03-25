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

#include <stdio.h>
#include <DreamSDK.h>

#include "Life.h"
#include "resource.h"
#include "version.h"

#include "Utils/registry.h"
#include "Utils/dialogs.h"

int	g_iWidth;
int g_iHeight;
float g_fRatio;

Grid grid;
int PALETTE_SIZE = sizeof(grid.palette)/sizeof(DWORD);

LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL; // Vertices Buffer

static BOOL IsDreamInitialized = FALSE;

// Mutex
HANDLE hMutex;

float frand(){return ((float) rand() / (float) RAND_MAX);}

DWORD randColor()
{
	float h=(float)(rand()%360), s = 0.3f + 0.7f*frand(), v=0.67f+0.25f*frand();
	if (grid.colorType == COLOR_NEIGHBORS || grid.colorType == COLOR_TIME)
	{
		s = 0.9f + 0.1f*frand();
		//v = 0.5f + 0.3f*frand();
	}
	return HSVtoRGB(h,s,v);
}

void SeedGrid()
{
	memset(grid.cells,0, grid.width*grid.height*sizeof(Cell));
	for ( int i = 0; i<grid.width*grid.height; i++ )
	{
		grid.cells[i].lifetime = 0;
		if (rand() % 4 == 0)
		{
			grid.cells[i].state = ALIVE;
			grid.cells[i].nextstate = grid.cells[i].state;
			if (grid.colorType == COLOR_TIME)
				grid.cells[i].color = grid.palette[grid.cells[i].lifetime];
			else
			{
				grid.cells[i].color = randColor();
			}
		}
	}
}


void presetPalette()
{
	grid.palette[11] = 0xFF2222FF; //block

	grid.palette[2]  = 0xFFFF0066;  //blinker
	grid.palette[24] = 0xFFFF33FF;  //inside

	grid.palette[12] = 0xFFAA00FF; //behive

	grid.palette[36] = 0xFF008800; //dot
	grid.palette[5]  = 0xFFDDDD00; //cross

	grid.palette[10]  = 0xFFAA0000; //ship
	grid.palette[13]  = 0xFF0099CC; //ship

}

void CreateGrid()
{
	int i, cellmin, cellmax;
	cellmin = (int)sqrt((float)(g_iWidth*g_iHeight/(int)(grid.maxSize*grid.maxSize*g_fRatio)));
	cellmax = (int)sqrt((float)(g_iWidth*g_iHeight/(int)(grid.minSize*grid.minSize*g_fRatio)));
	grid.cellSizeX = rand()%(cellmax - cellmin + 1) + cellmin;
	grid.cellSizeY = grid.cellSizeX > 5 ? (int)(g_fRatio * grid.cellSizeX) : grid.cellSizeX;
	grid.width = g_iWidth/grid.cellSizeX;
	grid.height = g_iHeight/grid.cellSizeY;

	if (grid.cellSizeX <= grid.cellLineLimit )
		grid.spacing = 0;
	else grid.spacing = 1;


	if (grid.fullGrid)
		delete grid.fullGrid;
	grid.fullGrid = new Cell[grid.width*(grid.height+2)+2]; 
	memset(grid.fullGrid,0, (grid.width*(grid.height+2)+2) * sizeof(Cell));
	grid.cells = &grid.fullGrid[grid.width + 1];
	grid.frameCounter = 0;
	do
	{
		grid.colorType = rand()%3;
	} while (!(grid.allowedColoring & (1 << grid.colorType)) && grid.allowedColoring != 0);
	grid.ruleset = 0;

	for (i=0; i< PALETTE_SIZE; i++)
		grid.palette[i] = randColor();

	grid.maxColor = MAX_COLOR;
	if (grid.colorType == COLOR_TIME && (rand()%100 < grid.presetChance))
		for (i=0; i< MAX_COLOR; i++)
			grid.palette[i] = COLOR_TIMES[i];
	else
		grid.maxColor += (rand()%2)*(rand()%60);  //make it shimmer sometimes
	if (grid.colorType == COLOR_TIME && rand()%3)
	{
		for (i=grid.maxColor-1; i<PALETTE_SIZE; i++)
			grid.palette[i] = LerpColor(grid.palette[grid.maxColor-1],grid.palette[PALETTE_SIZE-1],(float)(i-grid.maxColor+1)/(float)(PALETTE_SIZE-grid.maxColor));
		grid.maxColor = PALETTE_SIZE;
	}
	if (grid.colorType == COLOR_NEIGHBORS)
	{
		if (rand()%100 < grid.presetChance)
			presetPalette();
		reducePalette();
	}  
	SeedGrid();
}



int * rotateBits(int * bits)
{
	int temp;
	temp = bits[0];
	bits[0] = bits[2];
	bits[2] = bits[7];
	bits[7] = bits[5];
	bits[5] = temp;  
	temp = bits[1];
	bits[1] = bits[4];
	bits[4] = bits[6];
	bits[6] = bits[3];
	bits[3] = temp;
	return bits;
}
int * flipBits(int * bits)
{
	int temp;
	temp = bits[0];
	bits[0] = bits[2];
	bits[2] = temp;
	temp = bits[3];
	bits[3] = bits[4];
	bits[4] = temp;
	temp = bits[5];
	bits[5] = bits[7];
	bits[7] = temp;
	return bits;
}
int packBits(int * bits)
{
	int packed = 0;
	for(int j = 0; j<8; j++)
		packed |= bits[j] << j;
	return packed;
}
void unpackBits(int num, int * bits)
{  
	for(int i=0; i<8; i++)
		bits[i] = (num & (1<<i))>>i ;
}
// This simplifies the neighbor palette based off of symmetry
void reducePalette()
{
	int i = 0, bits[8], inf, temp;
	for(i = 0; i < 256; i++)
	{
		inf = i;
		unpackBits(i, bits);
		for(int k = 0; k < 2; k++)
		{
			for(int j = 0; j<4; j++)
				if ((temp = packBits(rotateBits(bits))) < inf)
					inf = temp;
			flipBits(bits);
		}
		grid.palette[i] = grid.palette[inf];
	}
}

void DrawGrid()
{
	for(int i = 0; i<grid.width*grid.height; i++ )
		if (grid.cells[i].state != DEAD)
			DrawRectangle((i%grid.width)*grid.cellSizeX,(i/grid.width)*grid.cellSizeY, 
			grid.cellSizeX - grid.spacing, grid.cellSizeY - grid.spacing, grid.cells[i].color);
}

void UpdateStates()
{
	for(int i = 0; i<grid.width*grid.height; i++ )
		grid.cells[i].state = grid.cells[i].nextstate;
}

void StepLifetime()
{
	int i;
	for(i = 0; i<grid.width*grid.height; i++ )
	{
		int count = 0;
		if(grid.cells[i-grid.width-1].state) count++;
		if(grid.cells[i-grid.width  ].state) count++;
		if(grid.cells[i-grid.width+1].state) count++;
		if(grid.cells[i           -1].state) count++;
		if(grid.cells[i           +1].state) count++;
		if(grid.cells[i+grid.width-1].state) count++;
		if(grid.cells[i+grid.width  ].state) count++;
		if(grid.cells[i+grid.width+1].state) count++;

		if(grid.cells[i].state == DEAD)
		{
			grid.cells[i].lifetime = 0;
			if (count == 3 || (grid.ruleset && count == 6))
			{
				grid.cells[i].nextstate = ALIVE;
				grid.cells[i].color = grid.palette[0];
			}
		}
		else 
		{
			if (count == 2 || count == 3)
			{
				grid.cells[i].lifetime++;
				if (grid.cells[i].lifetime >= grid.maxColor)
					grid.cells[i].lifetime = grid.maxColor - 1;
				grid.cells[i].color = grid.palette[grid.cells[i].lifetime];
			}
			else
				grid.cells[i].nextstate = DEAD;
		}
	}
	UpdateStates();
}

void StepNeighbors()
{
	UpdateStates();
	int i;
	for(i = 0; i<grid.width*grid.height; i++ )
	{
		int count = 0;
		int neighbors = 0;
		if(grid.cells[i-grid.width-1].state) {count++; neighbors |=  1;}
		if(grid.cells[i-grid.width  ].state) {count++; neighbors |=  2;}
		if(grid.cells[i-grid.width+1].state) {count++; neighbors |=  4;}
		if(grid.cells[i           -1].state) {count++; neighbors |=  8;}
		if(grid.cells[i           +1].state) {count++; neighbors |= 16;}
		if(grid.cells[i+grid.width-1].state) {count++; neighbors |= 32;}
		if(grid.cells[i+grid.width  ].state) {count++; neighbors |= 64;}
		if(grid.cells[i+grid.width+1].state) {count++; neighbors |=128;}

		if(grid.cells[i].state == DEAD)
		{
			if (count == 3 || (grid.ruleset && (neighbors == 0x7E || neighbors == 0xDB)))
			{
				grid.cells[i].nextstate = ALIVE;
				grid.cells[i].color = grid.palette[neighbors];
			}
		}
		else 
		{
			if (count != 2 && count != 3)
				grid.cells[i].nextstate = DEAD;
			grid.cells[i].color = grid.palette[neighbors];
		}
	}
}


void StepColony()
{
	D3DCOLOR foundColors[8];
	int i;
	for(i = 0; i<grid.width*grid.height; i++ )
	{
		int count = 0;
		if(grid.cells[i-grid.width-1].state) foundColors[count++] = grid.cells[i-grid.width-1].color;
		if(grid.cells[i-grid.width  ].state) foundColors[count++] = grid.cells[i-grid.width  ].color;
		if(grid.cells[i-grid.width+1].state) foundColors[count++] = grid.cells[i-grid.width+1].color;
		if(grid.cells[i           -1].state) foundColors[count++] = grid.cells[i           -1].color;
		if(grid.cells[i           +1].state) foundColors[count++] = grid.cells[i           +1].color;
		if(grid.cells[i+grid.width-1].state) foundColors[count++] = grid.cells[i+grid.width-1].color;
		if(grid.cells[i+grid.width  ].state) foundColors[count++] = grid.cells[i+grid.width  ].color;
		if(grid.cells[i+grid.width+1].state) foundColors[count++] = grid.cells[i+grid.width+1].color;

		if(grid.cells[i].state == DEAD)
		{
			if (count == 3 || (grid.ruleset && count == 6))
			{
				if (foundColors[0] == foundColors[2])
					grid.cells[i].color = foundColors[0];
				else 
					grid.cells[i].color = foundColors[1];
				grid.cells[i].nextstate = ALIVE;
			}
		}
		else if (count != 2 && count != 3)
			grid.cells[i].nextstate = DEAD;

	}
	UpdateStates();
}

void Step()
{
	switch(grid.colorType)
	{
	case COLOR_COLONY:    StepColony(); break;
	case COLOR_TIME:    StepLifetime(); break;
	case COLOR_NEIGHBORS:  StepNeighbors(); break;
	}
}

D3DCOLOR HSVtoRGB( float h, float s, float v )
{
	int i;
	float f;
	int r, g, b, p, q, t, m;

	if( s == 0 ) { // achromatic (grey)
		r = g = b = (int)(255*v);
		return D3DCOLOR_RGBA(r,g,b,255);
	}

	h /= 60;      // sector 0 to 5
	i = (int)( h );
	f = h - i;      // frational part of h
	m = (int)(255*v);
	p = (int)(m * ( 1 - s ));
	q = (int)(m * ( 1 - s * f ));
	t = (int)(m * ( 1 - s * ( 1 - f ) ));


	switch( i ) {
		case 0: return D3DCOLOR_RGBA(m,t,p,255);
		case 1: return D3DCOLOR_RGBA(q,m,p,255);
		case 2: return D3DCOLOR_RGBA(p,m,t,255);
		case 3: return D3DCOLOR_RGBA(p,q,m,255);
		case 4: return D3DCOLOR_RGBA(t,p,m,255);
		default: break;    // 5
	}
	return D3DCOLOR_RGBA(m,p,q,255);
}


void DrawRectangle(int x, int y, int w, int h, D3DCOLOR dwColour)
{
	VOID* pVertices;
	//Store each point of the triangle together with it's colour
	CUSTOMVERTEX cvVertices[] =
	{
		{(float) x, (float) y+h, 0.0f, 0.5, dwColour,},
		{(float) x, (float) y, 0.0f, 0.5, dwColour,},
		{(float) x+w, (float) y+h, 0.0f, 0.5, dwColour,},
		{(float) x+w, (float) y, 0.0f, 0.5, dwColour,},
	};

	//Get a pointer to the vertex buffer vertices and lock the vertex buffer
	g_pVertexBuffer->Lock(0, 4*sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);

	//Copy our stored vertices values into the vertex buffer
	memcpy(pVertices, cvVertices, sizeof(cvVertices));

	//Unlock the vertex buffer
	g_pVertexBuffer->Unlock();

	// Draw it
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}


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
		return 1;
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

	g_pd3dDevice = pd3dDevice;
	g_iWidth = rect.right - rect.left;
	g_iHeight = rect.bottom - rect.top;
	g_fRatio = 1.0f;

	// Load the settings
	//Create the vertex buffer from our device
	g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL);
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));

	memset(&grid,0, sizeof(Grid));

	hMutex = CreateMutex(NULL, false, "BioGenesisMutex");

	SetDefaults();
	LoadSettings();
	CreateGrid();

	SeedGrid();

	IsDreamInitialized = TRUE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{
	// Try to acquire mutex
	DWORD dwWaitMutex = WaitForSingleObject(hMutex, INFINITE);

	switch (dwWaitMutex) 
	{
		case WAIT_OBJECT_0: 
		{
			if (grid.frameCounter++ == grid.resetTime)
				CreateGrid();

			Step();

			ReleaseMutex(hMutex);
			break;
		}						

		default:	
			break;
	}	
}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{

	g_pd3dDevice = pd3dDevice;

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

		D3DXMATRIX m_World;
		D3DXMatrixIdentity( &m_World );
		pd3dDevice->SetTransform(D3DTS_WORLD, &m_World);

		// Try to acquire mutex
		DWORD dwWaitMutex = WaitForSingleObject(hMutex, INFINITE);

		switch (dwWaitMutex) 
		{
			case WAIT_OBJECT_0: 
			{
				DrawGrid();

				ReleaseMutex(hMutex);
				break;
			}						

			default:	
				break;
		}	

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Configure Dream
extern HINSTANCE hInst;
CONFIG_PLUGIN
{
	if (IsDreamInitialized == FALSE) 
		return FALSE;

	return (BOOL)DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_CONFIG), hWndParent, ConfigureDream, (LPARAM)&grid);
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	g_pVertexBuffer->Release();
	delete grid.fullGrid;

	CloseHandle(hMutex);
}

//////////////////////////////////////////////////////////////////////////
// Configuration
//////////////////////////////////////////////////////////////////////////
INT_PTR CALLBACK ConfigureDream(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	Grid *config = (Grid*) GetProp(hDlg, "config");

	switch(iMsg)
	{
		case WM_INITDIALOG:
		{
			// Set the version text
			char version[VERSION_SIZE];
			sprintf_s(version, VERSION_SIZE*sizeof(char), "v%i.%i Build %i",VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			SetDlgItemText(hDlg, IDC_BUILD,version);

			// Try to acquire mutex
			DWORD dwWaitMutex = WaitForSingleObject(hMutex, INFINITE);

			switch (dwWaitMutex) 
			{
				case WAIT_OBJECT_0: 
				{
					// Get Data
					SetProp(hDlg, "config", (HANDLE) (char*) lParam);
					config = (Grid*) GetProp(hDlg, "config");

					if(!config) {
						MessageBox(hDlg, "Error initializing Dialog: config not present", "Initialization Error", MB_OK|MB_ICONERROR);
						EndDialog(hDlg, 0);
					}

					SET_ITEM(IDC_ITERATIONS, GRID_RESET_TIME_MIN, GRID_RESET_TIME_MAX, GRID_RESET_TIME_FREQ, config->resetTime);
					SET_ITEM(IDC_MIN_SIZE, GRID_MIN_SIZE_MIN, GRID_MIN_SIZE_MAX, GRID_MIN_SIZE_FREQ, config->minSize);
					SET_ITEM(IDC_MAX_SIZE, GRID_MAX_SIZE_MIN, GRID_MAX_SIZE_MAX, GRID_MAX_SIZE_FREQ, config->maxSize);
					SET_ITEM(IDC_GRID_LINES, GRID_CELL_LINE_LIMIT_MIN, GRID_CELL_LINE_LIMIT_MAX, GRID_CELL_LINE_LIMIT_FREQ, config->cellLineLimit);
					SET_ITEM(IDC_PRESET, GRID_PRESET_CHANCE_MIN, GRID_PRESET_CHANCE_MAX, GRID_PRESET_CHANCE_FREQ, config->presetChance);
					
					SendDlgItemMessage(hDlg, IDC_COLONY, BM_SETCHECK, IS_ALLOWED(config->allowedColoring, COLOR_COLONY) ? BST_CHECKED : BST_UNCHECKED, NULL);
					SendDlgItemMessage(hDlg, IDC_LIFETIME, BM_SETCHECK, IS_ALLOWED(config->allowedColoring, COLOR_TIME) ? BST_CHECKED : BST_UNCHECKED, NULL);
					SendDlgItemMessage(hDlg, IDC_NEIGHBOR, BM_SETCHECK, IS_ALLOWED(config->allowedColoring, COLOR_NEIGHBORS) ? BST_CHECKED : BST_UNCHECKED, NULL);

					ReleaseMutex(hMutex);
					break;
				}						

				default:	
					break;
			}		

			break;
		}	

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_DEFAULT:
					RESET_ITEM(IDC_ITERATIONS, GRID_RESET_TIME);
					RESET_ITEM(IDC_MIN_SIZE, GRID_MIN_SIZE);
					RESET_ITEM(IDC_MAX_SIZE, GRID_MAX_SIZE);
					RESET_ITEM(IDC_GRID_LINES, GRID_CELL_LINE_LIMIT);
					RESET_ITEM(IDC_PRESET, GRID_PRESET_CHANCE);
					
					SendDlgItemMessage(hDlg, IDC_COLONY, BM_SETCHECK, IS_ALLOWED(GRID_ALLOWED_COLORING, COLOR_COLONY) ? BST_CHECKED : BST_UNCHECKED, NULL);
					SendDlgItemMessage(hDlg, IDC_LIFETIME, BM_SETCHECK, IS_ALLOWED(GRID_ALLOWED_COLORING, COLOR_TIME) ? BST_CHECKED : BST_UNCHECKED, NULL);
					SendDlgItemMessage(hDlg, IDC_NEIGHBOR, BM_SETCHECK, IS_ALLOWED(GRID_ALLOWED_COLORING, COLOR_NEIGHBORS) ? BST_CHECKED : BST_UNCHECKED, NULL);					
					break;

				case IDOK:
				{					
					if(!config)
						break;	

					// Try to acquire mutex
					DWORD dwWaitMutex = WaitForSingleObject(hMutex, INFINITE);

					switch (dwWaitMutex) 
					{
						case WAIT_OBJECT_0: 
						{
							config->resetTime = (int)SendDlgItemMessage(hDlg, IDC_ITERATIONS, TBM_GETPOS, NULL, NULL);
							config->minSize = (int)SendDlgItemMessage(hDlg, IDC_MIN_SIZE, TBM_GETPOS, NULL, NULL);
							config->maxSize = (int)SendDlgItemMessage(hDlg, IDC_MAX_SIZE, TBM_GETPOS, NULL, NULL);
							config->cellLineLimit = (int)SendDlgItemMessage(hDlg, IDC_GRID_LINES, TBM_GETPOS, NULL, NULL);
							config->presetChance = (int)SendDlgItemMessage(hDlg, IDC_PRESET, TBM_GETPOS, NULL, NULL);
							
							bool color_colony = (SendDlgItemMessage(hDlg, IDC_COLONY, BM_GETCHECK, NULL, NULL) == BST_CHECKED ? true : false);
							bool color_lifetime = (SendDlgItemMessage(hDlg, IDC_LIFETIME, BM_GETCHECK, NULL, NULL) == BST_CHECKED ? true : false);
							bool color_neighbors = (SendDlgItemMessage(hDlg, IDC_NEIGHBOR, BM_GETCHECK, NULL, NULL) == BST_CHECKED ? true : false);

							config->allowedColoring = GRID_ALLOWED_COLORING;

							if (color_colony)
								config->allowedColoring ^= (1 << COLOR_COLONY);

							if (color_lifetime)
								config->allowedColoring ^= (1 << COLOR_TIME);

							if (color_neighbors)
								config->allowedColoring ^= (1 << COLOR_NEIGHBORS);

							// Set the next frame to reset the grid
							config->frameCounter = config->resetTime;

							SaveSettings();

							ReleaseMutex(hMutex);
							break;
						}						

						default:	
							break;
					}	
				}

				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
					break;	
		}
		break;

		case WM_DESTROY:
			RemoveProp(hDlg, "config");
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return FALSE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// Set the default settings
void SetDefaults()
{
	grid.minSize = GRID_MIN_SIZE;
	grid.maxSize = GRID_MAX_SIZE;
	grid.spacing = GRID_SPACING;
	grid.resetTime = GRID_RESET_TIME;
	grid.presetChance = GRID_PRESET_CHANCE;
	grid.allowedColoring = GRID_ALLOWED_COLORING;
	grid.cellLineLimit = GRID_CELL_LINE_LIMIT;
}

//////////////////////////////////////////////////////////////////////////
// Load configuration from registry
void LoadSettings()
{
	HKEY key;	
	DWORD size, val;

	char configPath[MAX_PATH];	
	sprintf((char*)&configPath, "%s\\%s", DESKSCAPES_KEY, DREAM_NAME);

	// Open parent key
	if (RegOpenKeyEx(HKEY_CURRENT_USER, configPath, NULL, KEY_READ, &key) != ERROR_SUCCESS)
		return;

	// Load & check config keys	
	LOAD_KEY("MinSize", grid.minSize);	
	LOAD_KEY("MaxSize", grid.maxSize);
	LOAD_KEY("Spacing", grid.spacing);
	LOAD_KEY("ResetTime", grid.resetTime);
	LOAD_KEY("PresetChance", grid.presetChance);
	LOAD_KEY("AllowedColoring", grid.allowedColoring);
	LOAD_KEY("CellLineLimit", grid.cellLineLimit);

	RegCloseKey(key);
}

//////////////////////////////////////////////////////////////////////////
// Save configuration to registry
void SaveSettings() 
{
	HKEY key;
	DWORD val;

	char configPath[MAX_PATH];	
	sprintf((char*)&configPath, "%s\\%s", DESKSCAPES_KEY, DREAM_NAME);

	// Open parent key
	if (RegCreateKeyEx(HKEY_CURRENT_USER, configPath, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL) != ERROR_SUCCESS)
		return;

	// Save config
	SAVE_KEY("MinSize", grid.minSize);	
	SAVE_KEY("MaxSize", grid.maxSize);
	SAVE_KEY("Spacing", grid.spacing);
	SAVE_KEY("ResetTime", grid.resetTime);
	SAVE_KEY("PresetChance", grid.presetChance);
	SAVE_KEY("AllowedColoring", grid.allowedColoring);
	SAVE_KEY("CellLineLimit", grid.cellLineLimit);

	RegCloseKey(key);
}