/*
 * GreyNetic Screensaver for XBox Media Center
 * Copyright (c) 2004 Team XBMC
 *
 * Ver 1.0 26 Fed 2005	Dylan Thurston (Dinomight)
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

/*
 *
 * Greynetic was inspired by the Xscreensaver hack of the same name
 * This is a very basic kinda boring screen saver, but it works.
 * if you find any bugs please let me know
 * dinomight@gmail.com
 *
*/

#include <stdio.h>

#include "GreyNetic.h"
#include "XBMC/XmlDocument.h"

#include "DreamSDK.h"


#define MAX_BOXES 10000

struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex color
};

	int		NumberOfBoxes = MAX_BOXES;

	int		MaxSizeX = 200;
	int		MinSizeX = 0;
	int		MaxSizeY = 200;
	int		MinSizeY = 0;
	int		MaxSquareSize = 200;
	int		MinSquareSize = 0;
	int		MaxAlpha = 255;
	int		MinAlpha = 0;
	int		MaxRed = 255;
	int		MinRed = 0;
	int		MaxGreen = 255;
	int		MinGreen = 0;
	int		MaxBlue = 255;
	int		MinBlue = 0;
	int		MaxJoined = 255;
	int		MinJoined = 0;


	bool	MakeSquares = false;
	bool	JoinedSizeX = false;
	bool	JoinedSizeY = false;
	bool	JoinedRed = false;
	bool	JoinedGreen = false;
	bool	JoinedBlue = false;
	bool	JoinedAlpha = false;


	int		m_width;
	int		m_height;
	float		m_centerx;
	float		m_centery;
	
	int xa[MAX_BOXES];
	int ya[MAX_BOXES];
	int wa[MAX_BOXES];
	int ha[MAX_BOXES];
	D3DCOLOR dwcolor[MAX_BOXES];

	LPDIRECT3DVERTEXBUFFER9			m_pVB;


BOOL DrawRectangle(int x, int y, int w, int h, D3DCOLOR dwColour)
{
	VOID* pVertices;
    
    //Store each point of the triangle together with it's colour
    MYCUSTOMVERTEX cvVertices[] =
    {
        {(float) x, (float) y+h, 0.0f, 0.5, dwColour,},
        {(float) x, (float) y, 0.0f, 0.5, dwColour,},
		{(float) x+w, (float) y+h, 0.0f, 0.5, dwColour,},
        {(float) x+w, (float) y, 0.0f, 0.5, dwColour,},
    };

	//Create the vertex buffer from our device
    m_pd3dDevice->CreateVertexBuffer(4 * sizeof(MYCUSTOMVERTEX),
                                               D3DUSAGE_WRITEONLY, 
											   D3DFVF_CUSTOMVERTEX,
                                               D3DPOOL_DEFAULT, 
											   &g_pVertexBuffer,
											   NULL);

    //Get a pointer to the vertex buffer vertices and lock the vertex buffer
    DVERIFY(g_pVertexBuffer->Lock(0, sizeof(cvVertices), (void**)&pVertices, 0));

    //Copy our stored vertices values into the vertex buffer
    memcpy(pVertices, cvVertices, sizeof(cvVertices));

    //Unlock the vertex buffer
    g_pVertexBuffer->Unlock();

	// Draw it	
	m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);	
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);			
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);	
    m_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(MYCUSTOMVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// Every time we Create a vertex buffer, we must release one!.
	g_pVertexBuffer->Release();	

	return TRUE;
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

		case 5:
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
	m_pd3dDevice = pd3dDevice;
	m_iWidth = rect.right - rect.left;
	m_iHeight = rect.bottom - rect.top;

	LoadSettings();

	srand(timeGetTime());

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{
	for (int i=NumberOfBoxes - 1 ; i>0; i--){
		xa[i] = xa[i-1] ;
		ya[i] = ya[i-1] ;
		ha[i] = ha[i-1] ;
		wa[i] = wa[i-1] ;
		dwcolor[i] = dwcolor[i-1] ;
	}

	double red = rand() %(MaxRed - MinRed) + MinRed;
	double green = rand() %(MaxGreen - MinGreen) + MinGreen;
	double blue = rand() %(MaxBlue - MinBlue) + MinBlue;
	double alpha = rand() %(MaxAlpha - MinAlpha) + MinAlpha;
	double joined = rand() %(MaxJoined - MinJoined) + MinJoined;

	if(JoinedRed){
		red = joined;
	}
	if(JoinedGreen){
		green = joined;
	}
	if(JoinedBlue){
		blue = joined;
	}
	if(JoinedAlpha){
		alpha = joined;
	}

	dwcolor[0] = D3DCOLOR_RGBA((int) red, (int) green, (int) blue, (int) alpha); 

	xa[0] = rand()%m_iWidth;	
	ya[0] = rand()%m_iHeight;	

	ha[0] = rand() % (MaxSizeY - MinSizeY) + MinSizeY;	
	wa[0] = rand() % (MaxSizeX - MinSizeX) + MinSizeX;


	if(MakeSquares){
		ha[0] = rand() % (MaxSquareSize - MinSquareSize) + MinSquareSize;	
		wa[0] = ha[0]; 
	}

	if(JoinedSizeY){
		ha[0] = (int)joined;
	}
	if(JoinedSizeX){
		wa[0] = (int)joined;
	}
}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	m_pd3dDevice = pd3dDevice;
	
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	for (int i=NumberOfBoxes - 1 ; i>0; i--){	
		DrawRectangle(xa[i],ya[i],wa[i],ha[i], dwcolor[i]);
	}

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{

}

// Load settings from the [screensavername].xml configuration file
// the name of the screensaver (filename) is used as the name of
// the xml file - this is sent to us by XBMC when the Init func
// is called.
void LoadSettings()
{
	XmlNode node, childNode;
	CXmlDocument doc;

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
			if (childNode = doc.GetChildNode(node,"NumberOfBoxes")){
				OutputDebugString("found number of boxes settings");
				NumberOfBoxes = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MaxSizeX")){
				MaxSizeX = atoi(doc.GetNodeText(childNode)); 
			}
			if (childNode = doc.GetChildNode(node,"MinSizeX")){
				MinSizeX = atoi(doc.GetNodeText(childNode));
			}			
			if (childNode = doc.GetChildNode(node,"MaxSizeY")){
				MaxSizeY = atoi(doc.GetNodeText(childNode));
			}			
			if (childNode = doc.GetChildNode(node,"MinSizeY")){
				MinSizeY = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MakeSquares")){
				MakeSquares = !_strcmpi(doc.GetNodeText(childNode),"true");
			}
			if (childNode = doc.GetChildNode(node,"MinSquareSize")){
				MinSquareSize = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MaxSquareSize")){
				MaxSquareSize = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MaxAlpha")){
				MaxAlpha = atoi(doc.GetNodeText(childNode));
			}			
			if (childNode = doc.GetChildNode(node,"MinAlpha")){
				MinAlpha = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MaxRed")){
				MaxRed = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MinRed")){
				MinRed = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MaxGreen")){
				MaxGreen = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MinGreen")){
				MinGreen = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MaxBlue")){
				MaxBlue = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MinBlue")){
				MinBlue = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MaxJoined")){
				MinJoined = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"MinJoined")){
				MinJoined = atoi(doc.GetNodeText(childNode));
			}
			if (childNode = doc.GetChildNode(node,"JoinedsizeX")){
				if(!_strcmpi(doc.GetNodeText(childNode),"true")){
					JoinedSizeX = true;
				}
			}
			if (childNode = doc.GetChildNode(node,"JoinedsizeY")){
				if(!_strcmpi(doc.GetNodeText(childNode),"true")){
					JoinedSizeY = true;
				}
			}
			if (childNode = doc.GetChildNode(node,"JoinedAlpha")){
				if(!_strcmpi(doc.GetNodeText(childNode),"true")){
					JoinedAlpha = true;
				}
			}
			if (childNode = doc.GetChildNode(node,"JoinedRed")){
				if(!_strcmpi(doc.GetNodeText(childNode),"true")){
					JoinedRed = true;
				}
			}
			if (childNode = doc.GetChildNode(node,"JoinedGreen")){
				if(!_strcmpi(doc.GetNodeText(childNode),"true")){
					JoinedGreen = true;
				}
			}
			if (childNode = doc.GetChildNode(node,"JoinedBlue")){
				if(!_strcmpi(doc.GetNodeText(childNode),"true")){
					JoinedBlue = true;
				}
			}
			node = doc.GetNextNode(node);
		}
		doc.Close();
	}
}