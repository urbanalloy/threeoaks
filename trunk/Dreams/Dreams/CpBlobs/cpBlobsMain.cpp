// cpBlobs
// XBMC screensaver displaying metaballs moving around in an environment
// Simon Windmill (siw@coolpowers.com)

#include "cpBlobsMain.h"

#include "Blobby.h"

#include "DreamSDK.h"

Blobby *m_pBlobby;

////////////////////////////////////////////////////////////////////////////////

static float g_fTicks = 0.0f;

////////////////////////////////////////////////////////////////////////////////

// these global parameters can all be user-controlled via the XML file

float g_fTickSpeed = 0.01f;

D3DXVECTOR3 g_WorldRotSpeeds;
char g_strCubemap[1024];
char g_strDiffuseCubemap[1024];
char g_strSpecularCubemap[1024];

bool g_bShowCube = true;
	
DWORD g_BlendStyle;

DWORD g_BGTopColor, g_BGBottomColor;

float g_fFOV, g_fAspectRatio;

////////////////////////////////////////////////////////////////////////////////

// stuff for the environment cube
struct CubeVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
};

#define FVF_CUBEVERTEX D3DFVF_XYZ | D3DFVF_NORMAL

// man, how many times have you typed (or pasted) this data for a cube's
// vertices and normals, eh?
CubeVertex g_cubeVertices[] =
{
    {D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR3(0.0f, 0.0f,1.0f), },
    {D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR3(0.0f, 0.0f,1.0f), },
    {D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR3(0.0f, 0.0f,1.0f), },
    {D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR3(0.0f, 0.0f,1.0f), },

    {D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), },
    {D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), },
    {D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), },
    {D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), },

    {D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), },
    {D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), },
    {D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), },
    {D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), },

    {D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR3(0.0f,1.0f, 0.0f), },
    {D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR3(0.0f,1.0f, 0.0f), },
    {D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR3(0.0f,1.0f, 0.0f), },
    {D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR3(0.0f,1.0f, 0.0f), },

    {D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), },
    {D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), },
    {D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), },
    {D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), },

    {D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), },
    {D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), },
    {D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), },
    {D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), }
};

LPDIRECT3DVERTEXBUFFER9 g_pCubeVertexBuffer = NULL;

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


void RenderGradientBackground(LPDIRECT3DDEVICE9 pd3dDevice)
{
    // clear textures
    pd3dDevice->SetTexture( 0, NULL );
	pd3dDevice->SetTexture( 1, NULL );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	// don't write to z-buffer
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_BGVertices, sizeof(BG_VERTEX) );

	// restore state
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 

	return;
}

////////////////////////////////////////////////////////////////////////////////

LPDIRECT3DCUBETEXTURE9	g_pCubeTexture	= NULL;
LPDIRECT3DCUBETEXTURE9	g_pDiffuseCubeTexture	= NULL;
LPDIRECT3DCUBETEXTURE9	g_pSpecularCubeTexture	= NULL;

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
		return DEFAULT_SCREENUPDATEFREQUENCY;
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
	
	m_iWidth = rect.right - rect.left;
	m_iHeight = rect.bottom - rect.top;

	m_pBlobby = new Blobby();
	m_pBlobby->m_iNumPoints = 5;

	// Load the settings
	LoadSettings();

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );

	m_pBlobby->Init( pd3dDevice );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	D3DXCreateCubeTextureFromFile( pd3dDevice, g_strCubemap, &g_pCubeTexture );
	D3DXCreateCubeTextureFromFile( pd3dDevice, g_strDiffuseCubemap, &g_pDiffuseCubeTexture );
	D3DXCreateCubeTextureFromFile( pd3dDevice, g_strSpecularCubemap, &g_pSpecularCubeTexture );

	pd3dDevice->CreateVertexBuffer( 24*sizeof(CubeVertex), 0, FVF_CUBEVERTEX, D3DPOOL_DEFAULT, &g_pCubeVertexBuffer, NULL );

	void *pVertices = NULL;

	g_pCubeVertexBuffer->Lock( 0, sizeof(g_cubeVertices), (void**)&pVertices, 0 );
	memcpy( pVertices, g_cubeVertices, sizeof(g_cubeVertices) );
	g_pCubeVertexBuffer->Unlock();

	SetupGradientBackground( g_BGTopColor, g_BGBottomColor );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{
	// I know I'm not scaling by time here to get a constant framerate,
	// but I believe this to be acceptable for this application
	m_pBlobby->AnimatePoints( g_fTicks );	
	m_pBlobby->March();
}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

	// setup rotation
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationYawPitchRoll( &matWorld, g_WorldRotSpeeds.x * g_fTicks, g_WorldRotSpeeds.y * g_fTicks, g_WorldRotSpeeds.z * g_fTicks );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// setup viewpoint
	D3DXMATRIX matView;
	D3DXVECTOR3 campos( 0.0f, 0.0f, -0.8f );
	D3DXVECTOR3 camto( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 upvec( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &campos, &camto, &upvec );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// setup projection
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( g_fFOV ), g_fAspectRatio, 0.05f, 100.0f );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	// setup cubemap
	pd3dDevice->SetTexture( 0, g_pCubeTexture );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	// rotate the cubemap to match the world
	pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matWorld );
	pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matWorld );

	// draw the box (inside-out)
	if ( g_bShowCube )
	{
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetFVF( FVF_CUBEVERTEX );
		pd3dDevice->SetStreamSource( 0, g_pCubeVertexBuffer, 0, sizeof(CubeVertex) );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  0, 2 );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  4, 2 );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  8, 2 );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 12, 2 );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 16, 2 );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 20, 2 );
	}
	else
		RenderGradientBackground(pd3dDevice);

	pd3dDevice->SetTexture( 0, g_pDiffuseCubeTexture );
	pd3dDevice->SetTexture( 1, g_pSpecularCubeTexture );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   g_BlendStyle );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	m_pBlobby->Render( pd3dDevice );

	// increase tick count
	g_fTicks += g_fTickSpeed;

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	if( g_pCubeTexture != NULL ) 
		g_pCubeTexture->Release();

	if( g_pDiffuseCubeTexture != NULL ) 
		g_pDiffuseCubeTexture->Release();

	if( g_pSpecularCubeTexture != NULL ) 
		g_pSpecularCubeTexture->Release();

	delete m_pBlobby;

	if ( g_pCubeVertexBuffer != NULL )
	{
		g_pCubeVertexBuffer->Release();
		g_pCubeVertexBuffer = NULL;
	}
}
