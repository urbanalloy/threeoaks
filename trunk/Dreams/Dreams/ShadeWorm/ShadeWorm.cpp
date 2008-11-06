

#include "Utils/stdafx.h"
#include <stdio.h>

#include "ShadeWorm.h"
#include "XBMC\XmlDocument.h"

#include "DreamSDK.h"

// Pixel shader for colour remapping
const char* ShadeWorm_c::m_shaderColourMapSrc = 
			"ps.1.1\n"
			"tex t0\n"
			"texreg2ar t1, t0\n"
			"mul r0, v0.a, t1\n";			
			

//"xps.1.1\n"
//"tex t0\n"
//"texreg2ar t1, t0\n"
//"mul r0, v0.a, t1\n"
//"xfc zero, zero, zero, r0.rgb, zero, zero, 1-zero\n";

ShadeWorm_c::ShadeWorm_c()
{
	m_spriteTexture = NULL;
	m_bufferTexture = NULL;
	m_colMapTexture = NULL;
	m_pShader		= 0;
	m_pUcode		= NULL;
	m_pVertexBuffer = NULL;
	m_numColMaps	= 0;
}

ShadeWorm_c::~ShadeWorm_c()
{

}

//-- Create -------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShadeWorm_c::Create(LPDIRECT3DDEVICE9 device, int width, int height)
{
	m_device = device;
	m_width = width;
	m_height = height;

} // Create

//-- Start --------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool ShadeWorm_c::Start()
{

	char contentPath[MAX_PATH];
	getDreamContentPath((char*)&contentPath);

	char particle[MAX_PATH];
	sprintf((char*)&particle, "%s\\%s", contentPath, "Resources\\Particle.bmp");

	if (D3D_OK != D3DXCreateTextureFromFile(m_device, (char *)&particle, &m_spriteTexture))
		return false;

	if (D3D_OK  != m_device->CreateTexture(512, 512, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_bufferTexture, NULL))
		return false;

	if (D3D_OK  != m_device->CreateTexture(1, 256, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_colMapTexture, NULL))
		return false;

	if (D3D_OK  != m_device->CreateVertexBuffer(4*sizeof(TEXVERTEX), NULL, D3DFVF_TEXVERTEX, D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL))
		return false;
	
	if (S_OK != D3DXAssembleShader(m_shaderColourMapSrc, (UINT)strlen(m_shaderColourMapSrc), 0, NULL, NULL, &m_pUcode, NULL)) 						
		return false;	

	if (D3D_OK  != m_device->CreatePixelShader((DWORD*)m_pUcode->GetBufferPointer(), &m_pShader))	
		return false;	

	LoadSettings(m_saverName);

	if (m_numColMaps == 0)
		return false;

	return true;

} // Start

//-- Stop ---------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShadeWorm_c::Stop()
{
	if (m_bufferTexture)
		m_pVertexBuffer->Release();

	if (m_pShader)
		m_pShader->Release();		

	if (m_pUcode)
		m_pUcode->Release();

	if (m_colMapTexture)
		m_colMapTexture->Release();

	if (m_bufferTexture)
		m_bufferTexture->Release();

	if (m_spriteTexture)
		m_spriteTexture->Release();

	for (int i = 0; i < m_numColMaps; i++)
	{
		delete[] m_colMaps[i].m_colMap;
	}

} // Stop

//-- Render -------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShadeWorm_c::Render()
{
	static State_e state = STATE_NEWCOL;
	static int fadeCol = 0;

	IDirect3DSurface9* oldSurface;
	IDirect3DSurface9* newSurface;
	m_device->GetRenderTarget(NULL, &oldSurface);
	oldSurface->Release();

	m_bufferTexture->GetSurfaceLevel(0, &newSurface);
	m_device->SetRenderTarget(0, newSurface);
	newSurface->Release();

	if (state == STATE_NEWCOL)
	{
		if (m_randomColMap)
		{
			int newCol = rand() % m_numColMaps;
			if (newCol == m_colMap)
				m_colMap = (m_colMap+1) % m_numColMaps;
			else
				m_colMap = newCol;
		}
		else
		{
			m_colMap = (m_colMap+1) % m_numColMaps;
		}
		CreateColMap(m_colMap);

		m_device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

		float x = 256.0f + (100 - (rand() % 200));
		float y = 256.0f + (100 - (rand() % 200));

		for (int i = 0; i < m_numWorms; i++)
		{
			m_worms[i].vx = 0;
			m_worms[i].vy = 0;
			m_worms[i].x = x;
			m_worms[i].y = y;
		}

		state = STATE_FADEUP;
		m_timer = 60 * m_drawTime;
	}

	m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	if (state == STATE_FADEDOWN || state == STATE_DRAWING)
	{
		for (int i = 0; i < m_numWorms; i++)
		{
			m_worms[i].vx += GetRand();
			Clamp(-1, 1, m_worms[i].vx);
			m_worms[i].vy += GetRand();
			Clamp(-1, 1, m_worms[i].vy);

			m_worms[i].x += m_worms[i].vx;
			Clamp(0, 512, m_worms[i].x);
			m_worms[i].y += m_worms[i].vy;
			Clamp(0, 512, m_worms[i].y);
		}

		m_device->SetTexture(0, m_spriteTexture);

		for (int i = 0; i < m_numWorms; i++)
			RenderSprite(m_worms[i].x , m_worms[i].y, 5, 5, 0x05ffffff);
	}

	if (state == STATE_FADEUP)
	{
		fadeCol += 0x08;
		if (fadeCol >= 0xff)
		{
			fadeCol = 0xff;
			state = STATE_DRAWING;
		}
	}
	else if (state == STATE_FADEDOWN)
	{
		fadeCol -= 0x08;
		if (fadeCol <= 0)
		{
			fadeCol = 0;
			state = STATE_NEWCOL;
		}
	}
	else if (state == STATE_DRAWING)
	{
		m_timer--;
		if (m_timer <= 0)
			state = STATE_FADEDOWN;
	}

	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_device->SetRenderTarget(0, oldSurface);

	m_device->SetTexture(0, m_bufferTexture);
	m_device->SetTexture(1, m_colMapTexture);

	m_device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_device->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
	m_device->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
	m_device->SetPixelShader(m_pShader);

	float size = fadeCol / 255.0f;
	RenderSprite(m_width/2.0f, m_height/2.0f, m_width/2.0f * size, m_height/2.0f * size, fadeCol << 24);

	m_device->SetPixelShader(0);
	m_device->SetTexture(0, NULL);
	m_device->SetTexture(1, NULL);

} // Render

//-- CreateColMap -------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShadeWorm_c::CreateColMap(int colIndex)
{
	D3DLOCKED_RECT	lockedRect;
	if (FAILED(m_colMapTexture->LockRect(0, &lockedRect, NULL, 0)))
		return;

	unsigned char* data = (unsigned char*)lockedRect.pBits; 
	unsigned char* data2; 

	ColMap_t&	colMap = m_colMaps[colIndex];
	int colMapSize = colMap.m_numCols;

	for (int j=0; j<colMapSize-1; j++)
	{
		D3DCOLOR	srcCol = colMap.m_colMap[j].m_col;
		D3DCOLOR	dstCol = colMap.m_colMap[j+1].m_col;

		int srcR = (srcCol & 0x00ff0000) >> 16;
		int srcG = (srcCol & 0x0000ff00) >> 8;
		int srcB = (srcCol & 0x000000ff) >> 0;
		int dstR = (dstCol & 0x00ff0000) >> 16;
		int dstG = (dstCol & 0x0000ff00) >> 8;
		int dstB = (dstCol & 0x000000ff) >> 0;
		int range = (colMap.m_colMap[j+1].m_index - colMap.m_colMap[j].m_index) + 1;

		for (int i=0; i<range; i++)
		{
			float d = i / (float)range;
			int resR = srcR + int((dstR - srcR) * d);
			int resG = srcG + int((dstG - srcG) * d);
			int resB = srcB + int((dstB - srcB) * d);

			data2 = data + 	lockedRect.Pitch*(i+colMap.m_colMap[j].m_index);
			data2[0] = resB; // b
			data2[1] = resG; // g
			data2[2] = resR; // r
			data2[3] = 0; // a
		}
	}

	m_colMapTexture->UnlockRect(0);

} // CreateColMap

//--  RenderSprite  ------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ShadeWorm_c::RenderSprite(float x, float y, float sizeX, float sizeY, int col)
{
	TEXVERTEX image[] =
	{
		{x - sizeX, y - sizeY, 0.0f, 1.0f, col, 0.0f, 0.0f,},
		{x + sizeX, y - sizeY, 0.0f, 1.0f, col, 1.0f, 0.0f,},		
		{x + sizeX, y + sizeY, 0.0f, 1.0f, col, 1.0f, 1.0f,},
		{x - sizeX, y + sizeY, 0.0f, 1.0f, col, 0.0f, 1.0f,},
	}; 

	VOID* pVertices;
	if (FAILED(m_pVertexBuffer->Lock( 0, sizeof(image), (void**)&pVertices, 0)))
		return;
	
	memcpy(pVertices, image, sizeof(image));

	m_pVertexBuffer->Unlock();

	m_device->SetFVF(D3DFVF_TEXVERTEX);
	m_device->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(TEXVERTEX));
	
	m_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);			

} // RenderSprite

//-- Clamp --------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShadeWorm_c::Clamp(float min, float max, float& val)
{
	if (val < min)
		val = min;
	if (val > max)
		val = max;

} // Clamp

//-- Clamp --------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShadeWorm_c::Clamp(int min, int max, int& val)
{
	if (val < min)
		val = min;
	if (val > max)
		val = max;

} // Clamp

//-- GetRand ------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
float ShadeWorm_c::GetRand()
{
	return 0.5f - ((rand() % 100) / 100.0f);

} // GetRand

//-- LoadSettings -------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShadeWorm_c::LoadSettings(const char* name)
{
	int colId;
	int index;
	int col;

	// Set up the defaults
	m_numWorms = 16;
	m_drawTime = 8;
	m_numColMaps = 0;
	m_randomColMap = false;

	XmlNode node;
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
			if (!_strcmpi(doc.GetNodeTag(node),"NumWorms"))
			{
				m_numWorms = atoi(doc.GetNodeText(node));
				Clamp(1, 32, m_numWorms);
			}
			else if (!_strcmpi(doc.GetNodeTag(node),"DrawTime"))
			{
				m_drawTime = atoi(doc.GetNodeText(node));
				Clamp(1, 10, m_drawTime);
			}
			else if (!_strcmpi(doc.GetNodeTag(node),"RandomColMap"))
			{
				m_randomColMap = !_strcmpi(doc.GetNodeText(node),"true");
			}
			else if (!_strcmpi(doc.GetNodeTag(node),"Map"))
			{
				node = doc.GetNextNode(node);

				if (m_numColMaps < MAX_COLMAPS)
				{

					colId = 0;
					m_colMaps[m_numColMaps].m_numCols = 0;
					m_colMaps[m_numColMaps].m_colMap = new ColMapEntry_t[256];

					while (node>0)
					{
						if (!_strcmpi(doc.GetNodeTag(node),"Col"))
						{
							if (colId < 256)
							{
								char* txt = doc.GetNodeText(node);

								sscanf(txt, "%d %x\n", &index, &col);
								if (index > 255) index = 255;
								if (index < 0) index = 0;

								m_colMaps[m_numColMaps].m_colMap[colId].m_index = index;
								m_colMaps[m_numColMaps].m_colMap[colId].m_col = col;
								colId++;
							}
						}
						else if (!_strcmpi(doc.GetNodeTag(node),"/Map"))
						{
							m_colMaps[m_numColMaps].m_numCols = colId;
							m_numColMaps++;
							break;
						}

						node = doc.GetNextNode(node);
					}
				}
			}

			node = doc.GetNextNode(node);
		}
		doc.Close();
	}

} // LoadSettings
