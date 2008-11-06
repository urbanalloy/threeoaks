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
#include <stdio.h>
#include <stdlib.h>

/***************************** C L A S S E S *******************************/

////////////////////////////////////////////////////////////////////////////
// 
class CRenderD3D
{
public:
	LPDIRECT3DDEVICE9	GetDevice()		{ return m_D3dDevice; }

	LPDIRECT3DDEVICE9	m_D3dDevice;
	
	int			m_Width;
	int			m_Height;

};

/***************************** G L O B A L S *******************************/

void LoadSettings();
void SetDefaults();
