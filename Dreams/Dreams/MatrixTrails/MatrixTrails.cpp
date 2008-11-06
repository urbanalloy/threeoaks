///////////////////////////////////////////////////////////////////////////////////////////////
//
// MatrixTrails Dream
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

#include <stdio.h>

#include "main.h"
#include "matrixtrails.h"

#include "DreamSDK.h"

////////////////////////////////////////////////////////////////////////////
CMatrixTrails::CMatrixTrails()
{
	m_NumColumns	= gConfig.m_NumColumns;
	m_NumRows		= gConfig.m_NumRows;

	m_Columns = new CColumn[m_NumColumns];

	for (int c = 0; c < m_NumColumns; c++)
	{
		m_Columns[c].Init(m_NumRows);
	}

	m_Texture = null;
	m_VertexBuffer = null;
}

////////////////////////////////////////////////////////////////////////////
CMatrixTrails::~CMatrixTrails()
{
	SAFE_DELETE_ARRAY(m_Columns);
}

////////////////////////////////////////////////////////////////////////////
bool CMatrixTrails::RestoreDevice(LPDIRECT3DDEVICE9 d3dDevice, RECT rect)
{
	int numQuads	= m_NumRows * m_NumColumns;
	d3dDevice->CreateVertexBuffer( 4*numQuads*sizeof(TRenderVertex), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, TRenderVertex::FVF_Flags, D3DPOOL_DEFAULT, &m_VertexBuffer, NULL);

	char contentPath[MAX_PATH] = "";
	getDreamContentPath((char*)&contentPath);

	char fullpath[MAX_PATH] = "";
	sprintf((char*)&fullpath, "%s\\%s", contentPath, "MatrixTrails.tga");
	DVERIFY(D3DXCreateTextureFromFile(d3dDevice, fullpath, &m_Texture))

	m_CharSize.x	= (f32)(rect.right - rect.left)  / (f32)m_NumColumns;
	m_CharSize.y	= (f32)(rect.bottom - rect.top) / (f32)m_NumRows;
	m_CharSize.z	= 0.0f;

	return true;
}

////////////////////////////////////////////////////////////////////////////
void CMatrixTrails::InvalidateDevice()
{
	SAFE_RELEASE(m_Texture); 
	SAFE_RELEASE(m_VertexBuffer); 
}

////////////////////////////////////////////////////////////////////////////
void CMatrixTrails::Update(f32 dt)
{
	for (int c=0; c < m_NumColumns; c++)
		m_Columns[c].Update(dt);
}

////////////////////////////////////////////////////////////////////////////
bool CMatrixTrails::Draw(LPDIRECT3DDEVICE9 d3dDevice)
{
	// Fill	in the vertex buffers with the quads
	TRenderVertex* vert	= NULL;
	if (FAILED(m_VertexBuffer->Lock(0, 0, (void**)&vert, 0)))
		return false;

	f32 posX = 0, posY = 0;
	for (int cNr=0; cNr<m_NumColumns; cNr++)
	{
		vert = m_Columns[cNr].UpdateVertexBuffer(vert, posX, posY, m_CharSize, gConfig.m_CharSizeTex);
		posX += m_CharSize.x;
	}	

	m_VertexBuffer->Unlock();

        // Setup our texture
	d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	 D3DTOP_MODULATE);
	d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE);
	d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
	d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
    d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,	 D3DTOP_DISABLE);
	d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE);

	d3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	d3dDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    d3dDevice->SetTexture(0, m_Texture);
	d3dDevice->SetStreamSource(0, m_VertexBuffer, 0, sizeof(TRenderVertex));
	d3dDevice->SetFVF(TRenderVertex::FVF_Flags);

	CMatrix m;
	m.Identity();
	d3dDevice->SetTransform(D3DTS_PROJECTION, &m);
	d3dDevice->SetTransform(D3DTS_VIEW, &m);
	d3dDevice->SetTransform(D3DTS_WORLD, &m);

	// TODO: Add an index buffer and just do a single draw call here instead or use quads on xbox
	//		 However quad lists doesn't exists on DX PC so then the 'test' build wont work anymore
	int	index = 0;
	for (int cNr=0; cNr<m_NumColumns; cNr++)
	{
		for	(int rNr=0;	rNr<m_NumRows; rNr++)
		{
			d3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,	index, 2 );
			index += 4;
		}
	}

	return true;
}
