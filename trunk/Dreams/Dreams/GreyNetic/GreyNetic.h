// include file for screensaver template

#include "Utils/stdafx.h"

void LoadSettings();

LPDIRECT3DDEVICE9 m_pd3dDevice;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL; // Vertices Buffer

int	m_iWidth;
int m_iHeight;

void hsv_to_rgb(double hue, double saturation, double value, double *red, double *green, double *blue);
BOOL DrawRectangle(int x, int y, int w, int h, D3DCOLOR dwColour);

struct MYCUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex.
    DWORD colour; // The vertex colour.
};