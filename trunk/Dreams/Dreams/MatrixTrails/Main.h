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

#define TEXTURESIZE		256				// Width & height of the texture we are using

////////////////////////////////////////////////////////////////////////////
class CConfig
{
	public:
		int				m_NumColumns;		// Number of character columns
		int				m_NumRows;			// Number of character rows

		CRGBA			m_CharCol;
		f32				m_FadeSpeedMin;		// How quickly the characters fade out
		f32				m_FadeSpeedMax;
		f32				m_CharDelayMin;		// How long we wait before we add a new char
		f32				m_CharDelayMax;

		CVector2		m_CharSizeTex;		// Size of the characters in the texture
		int				m_NumChars;			// Number of characters in the texture

		void			SetDefaults();
		void			LoadSettings();
};

extern	CConfig		gConfig;