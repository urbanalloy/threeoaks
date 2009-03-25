///////////////////////////////////////////////////////////////////////////////////////////////
//
// Dream Dll Loader
//
// (c) 2007 - Three Oaks Crossing
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy $
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>

using namespace std;

// Encapsulate a Dream dll instance
class DreamDll
{
	private:
		// Dream instance
		HINSTANCE hLibrary;
		LPCSTR dllName;

		// Deskscape lib
		HINSTANCE hDeskscape;

		// Load Plugin
		void LoadPluginInstance( INT nCount, ... );
		
		// Informations
		struct DREAM_SETTINGS {
			int d3dFormat;
			int videoTexture;
			int screenUpdateFrequency;
			int query_4;
			int query_5;
		};

		DWORD signatureID;
		DWORD signatureData;

	public:

		DREAM_SETTINGS *settings;

		// State
		BOOL initialized;

		// Time delta (delta between calls to achieve the target fps)
		float deltaTime;
	
		DreamDll(LPCSTR dll);
		~DreamDll();
	
		typedef int  (__stdcall *DREAM_QUERYDEFAULTSETTINGS) (int type, int value);	
		typedef void (__stdcall *DREAM_NOTIFICATIONPROCEDURE) (int type, int value);
		typedef void (__stdcall *DREAM_NOTIFICATIONPROCEDURE_SEC) (int type, int value, DWORD* SecureSignature);
		typedef BOOL (__stdcall *DREAM_SETUPPLUGIN) (LPDIRECT3DDEVICE9 pd3dDevice, RECT rect, int nMonitors, HWND hDeskscapes);
		typedef BOOL (__stdcall *DREAM_CONFIGPLUGIN) (HWND hWndParent);
		typedef void (__stdcall *DREAM_UPDATESCENE) ();	
		typedef int  (__stdcall *DREAM_RENDERSCENEONSCREEN) (BOOL isRenderingDisabled, LPDIRECT3DDEVICE9 pd3dDevice, RECT rect, RECT screen, HWND hDeskscapes);
		typedef void (__stdcall *DREAM_FREEPLUGIN) (HWND hDeskscapes, int nMonitors, LPDIRECT3DDEVICE9 pd3dDevice);			
		
		// Dream functions
				
		DREAM_SETUPPLUGIN SetupPlugin;
		DREAM_CONFIGPLUGIN ConfigPlugin;
		DREAM_QUERYDEFAULTSETTINGS QueryDefaultSettings;
		DREAM_NOTIFICATIONPROCEDURE NotificationProcedure;
		DREAM_UPDATESCENE UpdateScene;
		DREAM_RENDERSCENEONSCREEN RenderSceneOnScreen;
		DREAM_FREEPLUGIN FreePlugin;
		DREAM_NOTIFICATIONPROCEDURE_SEC NotificationProcedureSecure;

		void Notify(int type, int value);
		BOOL Render(BOOL isRenderingDisabled, LPDIRECT3DDEVICE9 pd3dDevice, RECT rect, RECT screen, HWND hDeskscapes);
		void LoadSignatureID();
		BOOL LoadSignedDll();
		void LoadSignatureData();

		void QuerySettings(D3DFORMAT format);


		PSID DreamDll::GetUserSID();
		BOOL DreamDll::ComputeSID(PSID pSid, LPSTR lpString, int dSize);
};