/*
 * CrystalMorph Screensaver for XBox Media Center
 * Copyright (c) 2004 Team XBMC
 *
 * Ver 1.0 2007-02-12 by Asteron  http://asteron.projects.googlepages.com/home
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2  of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include "Fractal.h"
#include "fractalcontroller.h"
#include "fractal.h"
#include "XBMC/XmlDocument.h"

#include <DreamSDK.h>
#include <stdio.h>

FractalSettings settings;

LPDIRECT3DDEVICE9 g_pd3dDevice;

int	g_iWidth;
int g_iHeight;
float g_fRatio;

void Step()
{
  if (settings.frame > settings.nextTransition)
	{
    settings.nextTransition = settings.frame + settings.transitionTime;

    if (settings.animationCountdown == 0)
    {
      settings.fractalcontroller->SetAnimation(true);
      settings.animationCountdown = 3+rand()%5;
      settings.nextTransition = settings.frame + settings.animationTime;
    }
    else
    {
      settings.fractalcontroller->SetAnimation(false);
      
      if (settings.animationCountdown-- == 1)
      {
        settings.fractalcontroller->SetToRandomFractal(3+rand()%3);
      }
      else
      {
        static int preset = rand()%6;
        if (frand() < settings.presetChance)
          settings.fractalcontroller->SetDesiredToPreset((preset+rand()%5)%6);
        else
        {
          if (rand()%70 == 0) settings.fractalcontroller->SetToRandomFractal(8);
          else settings.fractalcontroller->SetToRandomFractal();
        }
      }
      settings.fractalcontroller->StartMorph();
    }
	}
	settings.fractalcontroller->UpdateFractalData();
	settings.frame++;
}

D3DXVECTOR3 g_lightDir = D3DXVECTOR3(-0.5f, -0.5f, 0.5f);

void CreateLight()
{
  // Fill in a light structure defining our light
  D3DLIGHT9 light;
  memset( &light, 0, sizeof(D3DLIGHT9) );
  light.Type    = D3DLIGHT_POINT;
  light.Ambient = (D3DXCOLOR)D3DCOLOR_RGBA(128,0,0,255);
  light.Diffuse = (D3DXCOLOR)D3DCOLOR_RGBA(255,255,255,255);
  light.Specular = (D3DXCOLOR)D3DCOLOR_RGBA(255,255,255,255);
  light.Range   = 30.0f;
  light.Position = D3DXVECTOR3(5,5,-10);
  light.Attenuation0 = 0.5f;
  light.Attenuation1 = 0.02f;
  light.Attenuation2 = 0.0f;

  // Create a direction for our light - it must be normalized 
  light.Direction = g_lightDir;

  // Tell the device about the light and turn it on
  g_pd3dDevice->SetLight( 0, &light );
  g_pd3dDevice->LightEnable( 0, TRUE ); 
  g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
  //d3dSetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(50,50,50) );

}

////////////////////////////////////////////////////////////////////////////////

// stuff for the background plane

struct BG_VERTEX 
{
    D3DXVECTOR4 position;
    DWORD       color;
};

BG_VERTEX g_BGVertices[4];

////////////////////////////////////////////////////////////////////////////////

// fill in background vertex array with values that will
// completely cover screen
void SetupGradientBackground( DWORD dwTopColor, DWORD dwBottomColor )
{
	float x1 = -0.5f;
	float y1 = -0.5f;
	float x2 = (float)g_iWidth - 0.5f;
    float y2 = (float)g_iHeight - 0.5f;
	
	g_BGVertices[0].position = D3DXVECTOR4( x2, y1, 0.0f, 1.0f );
    g_BGVertices[0].color = dwTopColor;

    g_BGVertices[1].position = D3DXVECTOR4( x2, y2, 0.0f, 1.0f );
    g_BGVertices[1].color = dwBottomColor;

    g_BGVertices[2].position = D3DXVECTOR4( x1, y1, 0.0f, 1.0f );
    g_BGVertices[2].color = dwTopColor;

    g_BGVertices[3].position = D3DXVECTOR4( x1, y2, 0.0f, 1.0f );
    g_BGVertices[3].color = dwBottomColor;
	
	return;
}

///////////////////////////////////////////////////////////////////////////////


void RenderGradientBackground()
{
  static MorphColor colora(300),colorb(300);
  SetupGradientBackground(colora.getColor(),0);//colorb.getColor());
  colora.incrementColor();colorb.incrementColor();

    // clear textures
    g_pd3dDevice->SetTexture( 0, NULL );
	g_pd3dDevice->SetTexture( 1, NULL );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	// don't write to z-buffer
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    
	g_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_BGVertices, sizeof(BG_VERTEX) );

	// restore state
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
}

void SetCamera()
{
  D3DXMATRIX m_World;
  D3DXMatrixIdentity( &m_World );
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_World);

	D3DXMATRIX m_View;// = ViewMatrix(D3DXVECTOR(0,-20,20), D3DXVECTOR(0,0,0), D3DXVECTOR(0,-1,0), 0);
	D3DXVECTOR3 from = D3DXVECTOR3(0,0,-2);
	D3DXVECTOR3 to   = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 up   = D3DXVECTOR3(0,1,0);
	D3DXMatrixLookAtLH(&m_View,&from,&to,&up);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_View);

	D3DXMATRIX m_Projection; 
	D3DXMatrixPerspectiveFovLH(&m_Projection, D3DX_PI/4.0f, (float)(g_iWidth)/(float)(g_iHeight)*g_fRatio, 0.1f, 1000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_Projection);
}

void RenderSetup()
{
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
}

static BOOL IsDreamPaused = FALSE;

//////////////////////////////////////////////////////////////////////////
// Query default settings for the Dream
// return the value for the type of setting
QUERY_DEFAULT_SETTINGS
{
	switch (type)
	{
	case QUERY_D3DFORMAT:
	case QUERY_VIDEOTEXTURE:
	case QUERY_4:
	case QUERY_5:
		return value;
		break;		

	case QUERY_SCREENUPDATEFREQUENCY:
		return 1;
		break;
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
// Notification when:
//  - started
//	- paused
//	- resumed
//  - stopped
//	- settings have changed
NOTIFICATION_PROCEDURE
{
	switch(type) {

		default: 
			break;

		case NOTIFY_STARTED:
			IsDreamPaused = FALSE;
			break;

		case NOTIFY_STOPPED:
			IsDreamPaused = TRUE;
			break;

		case NOTIFY_PAUSED:
			IsDreamPaused = TRUE;
			break;

		case NOTIFY_PLAYING:
			IsDreamPaused = FALSE;
			break;

		case NOTIFY_5:
			break;

		case NOTIFY_SETTINGS_CHANGED:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Setup the plugin
// load Direct3D resources, update registry keys
SETUP_PLUGIN
{
	g_pd3dDevice = pd3dDevice;
	g_iWidth = rect.right - rect.left;
	g_iHeight = rect.bottom - rect.top;
	g_fRatio = 1.0f;

	// Load the settings
	//Create the vertex buffer from our device
	RenderSetup();
	SetCamera();
	LoadSettings();
	settings.fractal = new Fractal();
	settings.fractalcontroller = new FractalController(settings.fractal, &settings);
	settings.fractalcontroller->SetMorphSpeed(settings.morphSpeed);
	CreateLight();

	SetupGradientBackground( D3DCOLOR_RGBA(0,0,0,255), D3DCOLOR_RGBA(0,60,60,255) );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{

}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	g_pd3dDevice = pd3dDevice;

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

	CreateLight();
	SetCamera();
	RenderSetup();
	RenderGradientBackground();
	glInit();
	static int sx = (rand()%2)*2-1, sy = (rand()%2)*2-1;
	glRotatef(sx*settings.frame*0.0051f,sy*settings.frame*0.0032f,0);
	Step();
	settings.fractal->Render();

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	delete settings.fractal;
	delete settings.fractalcontroller;
}

void SetDefaults()
{
  settings.frame = 0;
  settings.iMaxDepth = 5;
  settings.iMaxObjects = 4000;
  settings.nextTransition = 500;
  settings.morphSpeed = 3.0f;
  settings.presetChance = 0.3f;
  settings.animationTime = 3000;
  settings.transitionTime = 500;

  settings.animationCountdown = 3+rand()%5;
}

// Load settings from the [screensavername].xml configuration file
// the name of the screensaver (filename) is used as the name of
// the xml file - this is sent to us by XBMC when the Init func
// is called.
void LoadSettings()
{
	XmlNode node, childNode;
	CXmlDocument doc;
	
	// Set up the defaults
	SetDefaults();

	char contentPath[MAX_PATH];
	getDreamContentPath((char*)&contentPath);

	char szXMLFile[MAX_PATH];
	sprintf((char*)&szXMLFile, "%s\\%s", contentPath, "Resources\\Config.xml");

	// Load the config file
	if (doc.Load(szXMLFile) >= 0)
	{
		node = doc.GetNextNode(XML_ROOT_NODE);
		while(node > 0)
		{
			if (_strcmpi(doc.GetNodeTag(node),"screensaver"))
			{
				node = doc.GetNextNode(node);
				continue;
			}
			if (childNode = doc.GetChildNode(node,"maxobjects")){
        settings.iMaxObjects = atoi(doc.GetNodeText(childNode));
			}
      if (childNode = doc.GetChildNode(node,"maxcutoffdepth")){
        settings.iMaxDepth = atoi(doc.GetNodeText(childNode));
			}
      if (childNode = doc.GetChildNode(node,"morphspeed")){
        settings.morphSpeed = (float)atof(doc.GetNodeText(childNode));
			}
      if (childNode = doc.GetChildNode(node,"animationtime")){
        settings.animationTime = 60*atoi(doc.GetNodeText(childNode));
			}
      if (childNode = doc.GetChildNode(node,"transitiontime")){
        settings.nextTransition = settings.transitionTime = 60*atoi(doc.GetNodeText(childNode));
			}
      if (childNode = doc.GetChildNode(node,"presetchance")){
        settings.presetChance = (float)atof(doc.GetNodeText(childNode));
			}

			node = doc.GetNextNode(node);
		}
		doc.Close();
	}
}