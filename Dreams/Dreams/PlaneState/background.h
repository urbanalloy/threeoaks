////////////////////////////////////////////////////////////////////////////
//
// Author:
//   Joakim Eriksson
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "animatorfloat.h"

////////////////////////////////////////////////////////////////////////////
class CBackground
{
public:
					CBackground();
					~CBackground();
	bool			RestoreDevice(CRenderD3D* render);
	void			InvalidateDevice(CRenderD3D* render);
	void			Update(f32 dt);
	bool			Draw(CRenderD3D* render);

	CVectorAnimator	m_RotAnim;			// Used to rotate the background

	// Device objects
	LPDIRECT3DVERTEXBUFFER9		m_VertexBuffer;
	LPDIRECT3DTEXTURE9			m_Texture;

	bool			CreateTexture(CRenderD3D* render);
};