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

#pragma once
#include "Utils/stdafx.h"
#include "XBMC/types.h"
#include "column.h"

////////////////////////////////////////////////////////////////////////////
typedef	struct	TRenderVertex
{
	CVector		pos;
	f32			w;
	DWORD		col;
	f32			u, v;
	enum FVF {	FVF_Flags =	D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1};
} TRenderVertex;

////////////////////////////////////////////////////////////////////////////
class CMatrixTrails
{
public:
					CMatrixTrails();
					~CMatrixTrails();
	bool			RestoreDevice(LPDIRECT3DDEVICE9 d3dDevice, RECT rect);
	void			InvalidateDevice();
	void			Update(f32 dt);
	bool			Draw(LPDIRECT3DDEVICE9 d3dDevice);

protected:
	int				m_NumColumns;
	int				m_NumRows;
	CColumn*		m_Columns;
	CVector			m_CharSize, m_CharSizeTex;

	// Device objects
	LPDIRECT3DVERTEXBUFFER9		m_VertexBuffer;
	LPDIRECT3DTEXTURE9			m_Texture;
};