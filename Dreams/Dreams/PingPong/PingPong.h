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
#include "Utils/stdafx.h"
#include "XBMC/types.h"

////////////////////////////////////////////////////////////////////////////
typedef	struct	TRenderVertex
{
	CVector		pos;
	DWORD		col;
	enum FVF {	FVF_Flags =	D3DFVF_XYZ | D3DFVF_DIFFUSE	};
} TRenderVertex;

////////////////////////////////////////////////////////////////////////////
class CPaddel
{

public:
	CVector		m_Pos;
	CVector		m_Size;
	CRGBA		m_Col;

	CPaddel()
	{
		m_Pos.Zero();
		m_Size.Set(1.0f, 5.0f, 0.0f);
		m_Col.Set(1.0f, 1.0f, 1.0f, 1.0f);
	}
};

////////////////////////////////////////////////////////////////////////////
class CBall
{

public:
	CVector		m_Pos;
	CVector		m_Vel;
	CVector		m_Size;
	CRGBA		m_Col;

	CBall()
	{
		m_Pos.Zero();
		m_Vel.Zero();
		m_Size.Set(1.0f, 1.0f, 0.0f);
		m_Col.Set(1.0f, 1.0f, 1.0f, 1.0f);
	}
};

////////////////////////////////////////////////////////////////////////////
class CPingPong
{

public:
	CPingPong();
	~CPingPong() {}
	bool			RestoreDevice(CRenderD3D* render);
	void			InvalidateDevice(CRenderD3D* render);
	void			Update(f32 dt);
	bool			Draw(CRenderD3D* render);

	CPaddel			m_Paddle[2];
	CBall			m_Ball;

protected:
	// Device objects
	LPDIRECT3DVERTEXBUFFER9		m_VertexBuffer;

	TRenderVertex*	AddQuad(TRenderVertex* vert, const CVector& pos, const CVector& size, DWORD col);
};
