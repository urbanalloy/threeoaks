/*
* Silverwave Screensaver for XBox Media Center
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

#include "Utils\stdafx.h"
#include <stdio.h>
#include <DreamSDK.h>

#include "Water.h"
#include "Effect.h"
#include "XBMC\XmlDocument.h"
#include "Util.h"

static BOOL IsDreamPaused = FALSE;

WaterSettings world;
AnimationEffect * effects[] = {

	new EffectBoil(),
	new EffectTwist(),
	new EffectBullet(),
	new EffectRain(),
	new EffectSwirl(),
	new EffectXBMCLogo(),
	NULL,
	//new EffectText(),
};

LPDIRECT3DTEXTURE9  g_Texture = NULL;

int  m_iWidth;
int m_iHeight;
D3DXVECTOR3 g_lightDir;
float g_shininess = 0.4f;

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
		return 15;
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
void CreateLight(LPDIRECT3DDEVICE9 device)
{
	// Fill in a light structure defining our light
	D3DLIGHT9 light;
	memset( &light, 0, sizeof(D3DLIGHT9) );
	light.Type    = D3DLIGHT_POINT;
	light.Ambient = (D3DXCOLOR)D3DCOLOR_RGBA(0,0,0,255);
	light.Diffuse = (D3DXCOLOR)D3DCOLOR_RGBA(255,255,255,255);
	light.Specular = (D3DXCOLOR)D3DCOLOR_RGBA(150,150,150,255);
	light.Range   = 300.0f;
	light.Position = D3DXVECTOR3(0,-5,5);
	light.Attenuation0 = 0.5f;
	light.Attenuation1 = 0.02f;
	light.Attenuation2 = 0.0f;

	// Create a direction for our light - it must be normalized 
	light.Direction = g_lightDir;

	// Tell the device about the light and turn it on
	device->SetLight( 0, &light );
	device->LightEnable( 0, TRUE ); 
	device->SetRenderState(D3DRS_LIGHTING, TRUE);

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
	float x2 = (float)m_iWidth - 0.5f;
	float y2 = (float)m_iHeight - 0.5f;

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
void RenderGradientBackground(LPDIRECT3DDEVICE9 device)
{
	// clear textures
	device->SetTexture( 0, NULL );
	device->SetTexture( 1, NULL );
	device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
	device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	// don't write to z-buffer
	device->SetRenderState( D3DRS_ZENABLE, FALSE ); 

	device->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_BGVertices, sizeof(BG_VERTEX) );

	return;
}

void LoadEffects()
{
	int i = 0;
	while(effects[i] != NULL)
		effects[i++]->init(&world);
	world.effectCount = i;
}

void SetMaterial(LPDIRECT3DDEVICE9 device)
{
	D3DMATERIAL9 mat;

	// Set the RGBA for diffuse reflection.
	if (world.isTextureMode)
	{
		mat.Diffuse.r = 1.0f;
		mat.Diffuse.g = 1.0f;
		mat.Diffuse.b = 1.0f;
		mat.Diffuse.a = 1.0f;
	}
	else
	{
		mat.Diffuse.r = 0.5f;
		mat.Diffuse.g = 0.5f;
		mat.Diffuse.b = 0.5f;
		mat.Diffuse.a = 1.0f;
	}

	// Set the RGBA for ambient reflection.
	mat.Ambient.r = 0.5f;
	mat.Ambient.g = 0.5f;
	mat.Ambient.b = 0.5f;
	mat.Ambient.a = 1.0f;

	// Set the color and sharpness of specular highlights.
	mat.Specular.r = g_shininess;
	mat.Specular.g = g_shininess;
	mat.Specular.b = g_shininess;
	mat.Specular.a = 1.0f;
	mat.Power = 100.0f;

	// Set the RGBA for emissive color.
	mat.Emissive.r = 0.0f;
	mat.Emissive.g = 0.0f;
	mat.Emissive.b = 0.0f;
	mat.Emissive.a = 0.0f;

	device->SetMaterial(&mat);
}

void LoadTexture(LPDIRECT3DDEVICE9 device)
{
	// Setup our texture
	//long hFile;
	int numTextures = 0;
	static char szSearchPath[512];
	static char szPath[512];
	static char foundTexture[1024];

	strcpy(szPath, world.szTextureSearchPath);
	if (world.szTextureSearchPath[strlen(world.szTextureSearchPath) - 1] != '\\')
		strcat(szPath,"\\");
	strcpy(szSearchPath,szPath);
	strcat(szSearchPath,"*");

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( szSearchPath, &fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			int len = (int)strlen(fd.cFileName);
			if (len < 4 || (_strcmpi(fd.cFileName + len - 4, ".txt") == 0))
				continue;

			if (rand() % (numTextures+1) == 0) // after n textures each has 1/n prob
			{
				strcpy(foundTexture,szPath);
				strcat(foundTexture,fd.cFileName);
			}
			numTextures++;      
		}while( FindNextFile( hFind, &fd));
		FindClose( hFind);
	}


	if (g_Texture != NULL && numTextures > 0)
	{
		g_Texture->Release();
		g_Texture = NULL;
	}

	if (numTextures > 0)
		D3DXCreateTextureFromFileA(device, foundTexture, &g_Texture);
}

void SetCamera(LPDIRECT3DDEVICE9 device)
{

	D3DXMATRIX m_World;
	D3DXMatrixIdentity( &m_World );
	device->SetTransform(D3DTS_WORLD, &m_World);

	D3DXMATRIX m_View;
	D3DXVECTOR3 from = D3DXVECTOR3(0,0,20);
	D3DXVECTOR3 to   = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 up   = D3DXVECTOR3(0,-1.0f,0);

	D3DXMatrixLookAtLH(&m_View,&from,&to,&up);
	device->SetTransform(D3DTS_VIEW, &m_View);

	float aspectRatio = 720.0f / 480.0f;
	D3DXMATRIX m_Projection; 
	D3DXMatrixPerspectiveFovLH(&m_Projection, D3DX_PI/4, aspectRatio, 1.0, 1000.0);
	device->SetTransform(D3DTS_PROJECTION, &m_Projection);
}


//////////////////////////////////////////////////////////////////////////
// Setup the plugin
// load Direct3D resources, update registry keys
SETUP_PLUGIN
{

	srand(timeGetTime());

	memset(&world,0,sizeof(WaterSettings));

	m_iWidth = rect.right - rect.left;
	m_iHeight = rect.bottom - rect.top;

	world.scaleX = 1.0f;
	LoadSettings(pd3dDevice);
	CreateLight(pd3dDevice);
	LoadEffects();

	if (world.isTextureMode)
	{
		LoadTexture(pd3dDevice);
		world.effectCount--; //get rid of logo effect
	}

	world.effectType = rand()%world.effectCount;
	world.frame = 0;
	world.nextEffectTime = 0;

	return TRUE;
}

void SetDefaults()
{
	world.frame = 0;
	world.nextEffectTime = 0;
	world.isWireframe = false;
	world.isTextureMode = false;
}

void SetupRenderState(LPDIRECT3DDEVICE9 device)
{
	SetCamera(device);
	SetMaterial(device);

	device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
	device->SetRenderState( D3DRS_LIGHTING, TRUE );
	device->SetRenderState( D3DRS_ZENABLE, TRUE); //D3DZB_TRUE );
	device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	device->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );
	device->SetRenderState( D3DRS_SPECULARENABLE, g_shininess > 0 );
	if(world.isWireframe)
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	if (world.isTextureMode)
	{
		device->SetTextureStageState(0, D3DTSS_COLOROP,	 D3DTOP_MODULATE);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE);
		device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		device->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
		device->SetTextureStageState(1, D3DTSS_COLOROP,	 D3DTOP_DISABLE);
		device->SetTextureStageState(1, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE);

		device->SetTexture( 0, g_Texture );
		device->SetTexture( 1, NULL );
	}
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
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

	//RenderGradientBackground(pd3dDevice);
	CreateLight(pd3dDevice);
	SetupRenderState(pd3dDevice);

	world.frame++;

	if (world.isTextureMode && world.nextTextureTime>0 && (world.frame % world.nextTextureTime) == 0)
		LoadTexture(pd3dDevice);

	if (world.frame > world.nextEffectTime)
	{
		if ((rand() % 3)==0)
			incrementColor();
		//static limit = 0;if (limit++>3)
		world.effectType += 1;//+rand() % (ANIM_MAX-1);
		world.effectType %= world.effectCount;
		effects[world.effectType]->reset();
		world.nextEffectTime = world.frame + effects[world.effectType]->minDuration() + 
			rand() % (effects[world.effectType]->maxDuration() - effects[world.effectType]->minDuration());
	}
	effects[world.effectType]->apply();
	world.waterField->Step();
	world.waterField->Render(pd3dDevice);

	pd3dDevice->EndScene();
	return 0;
}

/////////////////////////////////////////////////////////////////////////
// Free plugins resources
FREE_PLUGIN
{
	delete world.waterField;
	world.waterField = NULL;
	for (int i = 0; effects[i] != NULL; i++)
		delete effects[i];
	if (g_Texture != NULL)
	{
		g_Texture->Release();
		g_Texture = NULL;
	}

}


// Load settings from the xml configuration file
void LoadSettings(LPDIRECT3DDEVICE9 device)
{
	XmlNode node, childNode;
	CXmlDocument doc;

	char contentPath[MAX_PATH];
	getDreamContentPath((char*)&contentPath);

	char szXMLFile[MAX_PATH];
	sprintf(szXMLFile, "%s\\%s", (char*)&contentPath, "Resources\\Config.xml");

	OutputDebugString("Loading XML: ");
	OutputDebugString(szXMLFile);
	float xmin = -10.0f, 
		xmax = 10.0f, 
		ymin = -10.0f, 
		ymax = 10.0f, 
		height = 0.0f, 
		elasticity = 0.5f, 
		viscosity = 0.05f, 
		tension = 1.0f, 
		blendability = 0.04f;

	int xdivs = 50; 
	int ydivs = 50;
	int divs = 50;

	SetDefaults();

	sprintf(world.szTextureSearchPath, "%s\\%s", (char *)contentPath, "Resources\\textures"); 

	g_lightDir = D3DXVECTOR3(0.0f,0.6f,-0.8f);
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
			if (childNode = doc.GetChildNode(node,"wireframe")){
				world.isWireframe = strcmp(doc.GetNodeText(childNode),"true") == 0;
			}
			if (childNode = doc.GetChildNode(node,"elasticity")){
				elasticity = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"viscosity")){
				viscosity = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"tension")){
				tension = (float)atof(doc.GetNodeText(childNode));
			}      
			if (childNode = doc.GetChildNode(node,"blendability")){
				tension = (float)atof(doc.GetNodeText(childNode));
			}      
			if (childNode = doc.GetChildNode(node,"xmin")){
				xmin = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"xmax")){
				xmax = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"ymin")){
				ymin = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"ymax")){
				ymax = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"lightx")){
				g_lightDir.x = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"lighty")){
				g_lightDir.y = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"lightz")){
				g_lightDir.z = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"shininess")){
				g_shininess = (float)atof(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"texturemode")){
				world.isTextureMode = strcmp(doc.GetNodeText(childNode),"true") == 0;
			}
			if (childNode = doc.GetChildNode(node,"nexttexture")){
				world.nextTextureTime = (int)(30*(float)atof(doc.GetNodeText(childNode)));
			}
			if (childNode = doc.GetChildNode(node,"quality")){
				divs = atoi(doc.GetNodeText(childNode));
			}
			node = doc.GetNextNode(node);
		}
		doc.Close();
	}
	float scaleRatio = (xmax-xmin)/(ymax-ymin);
	int totalPoints = (int)(divs * divs * scaleRatio);
	//world.scaleX = 0.5f;
	xdivs = (int)sqrt(totalPoints * scaleRatio / world.scaleX);
	ydivs = totalPoints / xdivs;
	//xdivs = 144; ydivs= 90;
	world.waterField = new WaterField(device, xmin, xmax, ymin, ymax, xdivs, ydivs, height, elasticity, viscosity, tension, blendability, world.isTextureMode);
}


