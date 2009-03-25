///////////////////////////////////////////////////////////////////////////////////////////////
//
// Dream SDK
//
// Copyright (c) 2007-2008, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//	1. Redistributions of source code must retain the above copyright notice, this list of
//		 conditions and the following disclaimer. 
//	2. Redistributions in binary form must reproduce the above copyright notice, this list
//		 of conditions and the following disclaimer in the documentation and/or other materials
//		 provided with the distribution. 
//	3. The name of the author may not be used to endorse or promote products derived from this
//		 software without specific prior written permission. 
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//	POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define QUERY_D3DFORMAT 1
#define QUERY_VIDEOTEXTURE 2
#define QUERY_SCREENUPDATEFREQUENCY 3
#define QUERY_4 4
#define QUERY_5 5

#define NOTIFY_SIGNATURE 0
#define NOTIFY_STARTED 1
#define NOTIFY_STOPPED 2
#define NOTIFY_PAUSED 3
#define NOTIFY_PLAYING 4 
#define NOTIFY_5 5
#define NOTIFY_SETTINGS_CHANGED 6

#define GET_SIGNATURE_ID 1234
#define DEFAULT_SCREENUPDATEFREQUENCY 30

//////////////////////////////////////////////////////////////////////////
// Registry

#define DESKSCAPES_KEY   "Software\\Stardock\\Deskscapes"

void getDreamContentPath(char *path);


//////////////////////////////////////////////////////////////////////////
// Functions

#define QUERY_DEFAULT_SETTINGS int QueryDefaultSettings(int type, int value)
#define NOTIFICATION_PROCEDURE void NotificationProcedure(int type, int value)
#define SETUP_PLUGIN BOOL SetupPlugin(LPDIRECT3DDEVICE9 pd3dDevice, RECT rect, int nMonitors, HWND hDeskscapes)
#define SETUP_PLUGIN_PER_SCREEN BOOL SetupPluginPerScreen(LPDIRECT3DDEVICE9 pd3dDevice, RECT rect, int monitor, HWND hDeskscapes)
#define CONFIG_PLUGIN BOOL ConfigPlugin(HWND hWndParent)
#define UPDATE_SCENE void UpdateScene()
#define RENDER_SCENE_ON_SCREEN int RenderSceneOnScreen(BOOL isRenderingDisabled, LPDIRECT3DDEVICE9 pd3dDevice, RECT rect, RECT screen, HWND hDeskscapes)
#define FREE_PLUGIN void FreePlugin(HWND hDeskscapes, int nMonitors, LPDIRECT3DDEVICE9 pd3dDevice)