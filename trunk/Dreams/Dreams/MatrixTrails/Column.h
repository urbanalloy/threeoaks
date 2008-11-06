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
#include "XBMC/types.h"

struct TRenderVertex;

////////////////////////////////////////////////////////////////////////////
class CChar
{
	public:
		f32			m_Intensity;
		int			m_CharNr;
		CChar() : m_Intensity(1.0f), m_CharNr(0) {}
};

////////////////////////////////////////////////////////////////////////////
class CColumn
{
	public:
		CChar*			m_Chars;
		int				m_NumChars;
		f32				m_Delay,m_CharDelay;
		int				m_CurChar;
		f32				m_FadeSpeed;

						CColumn();
						~CColumn();
		void			Init(int numChars);
		void			Update(f32 dt);
	TRenderVertex*	UpdateVertexBuffer(TRenderVertex* verts, f32 posX, f32 posY, const CVector& charSize, const CVector2& charSizeTex);
};