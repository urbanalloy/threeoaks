////////////////////////////////////////////////////////////////////////////
//
// Author:
//   Joakim Eriksson
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utils/stdafx.h"
#include "XBMC/types.h"

/***************************** D E F I N E S *******************************/

#define NUMLINES		100

/****************************** M A C R O S ********************************/
/***************************** C L A S S E S *******************************/

////////////////////////////////////////////////////////////////////////////
//
typedef	struct	TRenderVertex
{
	f32			x, y, z, w;
	DWORD		col;
	enum FVF {	FVF_Flags =	D3DFVF_XYZRHW | D3DFVF_DIFFUSE };
} TRenderVertex;

////////////////////////////////////////////////////////////////////////////
// 
class CRenderD3D
{
public:
	void			Init();
	bool			RestoreDevice();
	void			InvalidateDevice();
	bool			Draw();
	void			DrawLine(const CVector2& pos1, const CVector2& pos2, const CRGBA& col1, const CRGBA& col2);

	LPDIRECT3DDEVICE9	GetDevice()		{ return m_D3dDevice; }

	s32				m_NumLines;
	int				m_Width;
	int				m_Height;

	TRenderVertex*				m_Verts;

	// Device objects
	LPDIRECT3DDEVICE9			m_D3dDevice;
	LPDIRECT3DVERTEXBUFFER9		m_VertexBuffer;
};

/***************************** G L O B A L S *******************************/

extern	CRenderD3D	gRender;

/***************************** I N L I N E S *******************************/
