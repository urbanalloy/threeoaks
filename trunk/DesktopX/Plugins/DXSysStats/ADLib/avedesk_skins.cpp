#pragma pack(push, 1)
struct SkinInfo
{
	DWORD dwSize;			// must always equal sizeof(SkinInfo)

	WCHAR* wKey;			// a key that can be set for this skin. for instance, the filename
					// of the skin
	DWORD maxKeyLength;		// length of the wKey buffer

	UINT num;			// the number this skin was/is in the enumeration, 0-based


	WCHAR* wName;			// skin name
	DWORD  maxNameLength;		// length of wName buffer
	WCHAR* wAuthor;			// skin author
	DWORD maxAuthorLength;		// length of wAuthor buffer
	WCHAR* wNotes;			// skin notes
	DWORD maxNotesLength;		// length of wNotes buffer
	int version;			// skin version
};
#pragma pack(pop,1)

// return constants for OnGetSkinW
const AVEDESK_NO_MORE_SKINS	= FALSE;
const AVEDESK_MORE_SKINS	= TRUE;
const AVEDESK_BUF_TOO_SMALL	= -1;

// prototype for DeskletSetSkinInfoW, which will set the skin associated
// with the particular desklet that is identified by hwndDesklet.
// this function WILL ALWAYS needed to be called when the skin is set/changed, even when
// its set thru OnCreateFromSkinW or OnSetSkinW.
BOOL CALLBACK DeskletSetSkinInfoW(HWND hwndDesklet, const SkinInfo* info);


// name: OnCreateSkinEnumeratorW
// params:	HINSTANCE hInstance, the HINSTANCE of the DLL associated with the desklet.
//			const WCHAR* deskletPath, the absolute path to the folder this desklets
//			DLL resides in.
// Return: A pointer to an owner defined ENUMERATOR_DATA type. This
//			pointer will be passed thru all other Skin APIs.
ENUMERATOR_DATA* __stdcall OnCreateSkinEnumeratorW(HINSTANCE hInstance, const WCHAR* deskletPath);


// name: OnDestroySkinEnumeratorW
// params: ENUMERATOR_DATA* ed, A pointer to an earlier created ENUMERATOR_DATA (by OnCreateSkinEnumeratorW)
//		   that will be destroyed and no longer will be used after this call. The function
//		   should clear any memory associated with this ENUMERATOR_DATA.
// Return: none
void __stdcall OnDestroySkinEnumeratorW(ENUMERATOR_DATA* ed);


// name: OnGetSkinW
// params: ENUMERATOR_DATA* ed, a pointer to an earlier created ENUMERATOR_DATA (by OnCreateSkinEnumeratorW)
//		   SkinInfo* info, a SkinInfo structure that should be filled with the skins data.
// Return: AVEDESK_NO_MORE_SKINS, AVEDESK_MORE_SKINS or AVEDESK_BUF_TOO_SMALL
//		   If one of the buffers of the info structure is too small, the matching buffers maxXXLength should
//		   be set to the required size and AVEDESK_BUF_TOO_SMALL should be returned.
//		   If there are no more skins to enumerate, return AVEDESK_NO_MORE_SKINS.
//		   Otherwise, return AVEDESK_MORE_SKINS.
INT __stdcall OnGetSkinW(ENUMERATOR_DATA* ed, SkinInfo* info);


// name: OnCreateFromSkinW
// params: HWND hwnd, HINSTANCE hInstance, similiar to OnCreate()
//		   ENUMERATOR_DATA* ed, A pointer to an earlier created ENUMERATOR_DATA (by OnCreateSkinEnumeratorW).
//			ed can be NULL.
//		   const SkinInfo* info, the SkinInfo structure that holds the data that define which skin to set
//			for this newly constructed desklet.
// Return: similiar to OnCreate()
// Note, ENUMERATOR_DATA* ed can be NULL, in which case the info structure should be used directly
//			to create a skin. If this is not possible, then construct a desklet with the default skin.
DOCKLET_DATA* __stdcall OnCreateFromSkinW(HWND hwnd,HINSTANCE hInstance, ENUMERATOR_DATA* ed, const SkinInfo* info);


// name: OnSetSkinW
// notes: similiar to OnCreateFromSkinW, except that it is applied to an already running desklet.
// Return: TRUE iff succesfull.
BOOL __stdcall OnSetSkinW(HWND hwnd, ENUMERATOR_DATA* ed, const SkinInfo* info);