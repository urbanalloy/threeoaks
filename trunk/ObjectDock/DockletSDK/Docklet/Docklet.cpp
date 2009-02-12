///////////////////////////////////////////////////////////////////////////////////////////////
//
// DockletSDK : Docklet Class
//
// (c) 2004 - Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Docklet.h"

Docklet::Docklet(HWND hwndDocklet,HINSTANCE hInstance)
{
	data = new DOCKLET_STATIC_DATA;
	PopulateStaticData(hwndDocklet, hInstance);
}

Docklet::~Docklet()
{
	delete(data);
}


///////////////////////////////////////////////////////////////////////////////////////////////
///  Populate the static data structure used by the Docklet class
void Docklet::PopulateStaticData(HWND hwndDocklet,HINSTANCE hInstance)
{
	data->hInstance = hInstance;
	data->hwndDocklet = hwndDocklet;

	char root[MAX_PATH+1];
	strcpy(root, "");
	_GetRootFolder(root);
	data->rootFolder = string(root);

	char relative[MAX_PATH+1];
	strcpy(relative,"");
	_GetRelativeFolder(relative);
	data->relativeFolder = string(relative);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void Docklet::_GetRootFolder(char *szRootFolder)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, char *szRootFolder);
	DUMMY_TYPEDEF HostDockletGetRootFolder = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletGetRootFolder");
	if(!HostDockletGetRootFolder)
		return;

	HostDockletGetRootFolder(data->hwndDocklet, szRootFolder);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void Docklet::_GetRelativeFolder(char *szFolder)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, char *szFolder);
	DUMMY_TYPEDEF HostDockletGetRelativeFolder = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletGetRelativeFolder");
	if(!HostDockletGetRelativeFolder)
		return;

	HostDockletGetRelativeFolder(data->hwndDocklet, szFolder);
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Get the handle to the window that owns this docklet.
HWND Docklet::GetHWND()
{
	return data->hwndDocklet;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Get the handle to the instance of this docklet DLL. 
HINSTANCE Docklet::GetInstance()
{
	return data->hInstance;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Query for whether or not the specified Docklet is currently visible
bool Docklet::IsVisible()
{
	typedef BOOL(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet);
	DUMMY_TYPEDEF HostDockletIsVisible = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletIsVisible");
	if(!HostDockletIsVisible)
		return false;

	return HostDockletIsVisible(data->hwndDocklet) != 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Retrieves the bounding rectangle of this Docklet in screen coordinates.
bool Docklet::GetRect(RECT* rect)
{
	typedef BOOL(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, RECT *rcDocklet);
	DUMMY_TYPEDEF HostDockletGetRect = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletGetRect");
	if(!HostDockletGetRect)
		return false;

	return HostDockletGetRect(data->hwndDocklet, rect) != 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Changes the current label of this Docklet. 
void Docklet::SetLabel(string label)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, char *szLabel);
	DUMMY_TYPEDEF HostDockletSetLabel = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletSetLabel");
	if(!HostDockletSetLabel)
		return;

	HostDockletSetLabel(data->hwndDocklet, (char*)label.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Retrieves the current label of this Docklet. 
string Docklet::GetLabel()
{
	typedef int(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, char *szLabel);
	DUMMY_TYPEDEF HostDockletGetLabel = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletGetLabel");
	if(!HostDockletGetLabel)
		return 0;

	char label[MAX_PATH+1];
	strcpy(label, "");
	HostDockletGetLabel(data->hwndDocklet, label);
	
	return string(label);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Loads a GDI+ image based on the given relative path.
Bitmap* Docklet::LoadGDIPlusImage(string imagePath)
{
	typedef Bitmap *(__stdcall *DUMMY_TYPEDEF)(char *szImage);
	DUMMY_TYPEDEF HostDockletLoadGDIPlusImage = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletLoadGDIPlusImage");
	if(!HostDockletLoadGDIPlusImage)
		return NULL;

	return HostDockletLoadGDIPlusImage((char*)imagePath.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Sets this docklet's image to a given GDI+ Image
void Docklet::SetImage(Image *image, bool automaticallyDeleteImage)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, Image *lpImageNew, BOOL bAutomaticallyDeleteImage);
	DUMMY_TYPEDEF HostDockletSetImage = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletSetImage");
	if(!HostDockletSetImage)
		return;

	HostDockletSetImage(data->hwndDocklet, image, automaticallyDeleteImage);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Sets this docklet's image to that of the given file. 
void Docklet::SetImageFile(string image)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, char *szImage);
	DUMMY_TYPEDEF HostDockletSetImageFile = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletSetImageFile");
	if(!HostDockletSetImageFile)
		return;

	HostDockletSetImageFile(data->hwndDocklet, (char*)image.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Sets this docklet's image overlay to a given GDI+ Image
void Docklet::SetImageOverlay(Image *imageOverlay, bool automaticallyDeleteImage)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, Image *imageOverlay, BOOL bAutomaticallyDeleteImage);
	DUMMY_TYPEDEF HostDockletSetImageOverlay = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletSetImageOverlay");
	if(!HostDockletSetImageOverlay)
		return;

	HostDockletSetImageOverlay(data->hwndDocklet, imageOverlay, automaticallyDeleteImage);
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Shows the standard ObjectDock "Choose Image" dialog template, and allows you to select
/// an image file for any purpose you wish, including choosing images for different states
/// a docklet might have.
bool Docklet::BrowseForImage(HWND hwndParent, char* image, string alternativeRelativeRoot)
{
	typedef BOOL(__stdcall *DUMMY_TYPEDEF)(HWND hwndParent, char *szImage, char *szAlternateRelativeRoot);
	DUMMY_TYPEDEF HostDockletBrowseForImage = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletBrowseForImage");
	if(!HostDockletBrowseForImage)
		return false;

	return HostDockletBrowseForImage(hwndParent, image, (char*)alternativeRelativeRoot.c_str()) != 0;	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Lock the mouse effect (e.g. zooming, etc) of this docklet's owning dock
void Docklet::LockMouseEffect(bool lock)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, BOOL bLock);
	DUMMY_TYPEDEF HostDockletLockMouseEffect = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletLockMouseEffect");
	if(!HostDockletLockMouseEffect)
		return;

	HostDockletLockMouseEffect(data->hwndDocklet, lock);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Causes this Docklet to animate in the dock to try to get the user's attention
void Docklet::DoAttentionAnimation()
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet);
	DUMMY_TYPEDEF HostDockletDoAttentionAnimation = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletDoAttentionAnimation");
	if(!HostDockletDoAttentionAnimation)
		return;

	HostDockletDoAttentionAnimation(data->hwndDocklet);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Causes this Docklet to animate in the dock to try to get the user's attention
void Docklet::DoClickAnimation()
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet);
	DUMMY_TYPEDEF HostDockletDoClickAnimation = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletDoClickAnimation");
	if(!HostDockletDoClickAnimation)
		return;

	HostDockletDoClickAnimation(data->hwndDocklet);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Remove the current docklet from the dock
void Docklet::RemoveSelf(WPARAM wParam)
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, WPARAM wParam);
	DUMMY_TYPEDEF HostDockletRemoveSelf = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletRemoveSelf");
	if(!HostDockletRemoveSelf)
		return;

	HostDockletRemoveSelf(data->hwndDocklet, wParam);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Retrieves the relative path of the folder that contains this Docklet's DLL
string Docklet::GetRootFolder()
{
	return data->rootFolder;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Retrieves the absolute location of the ObjectDock root folder,
/// from which all things are relative
string Docklet::GetRelativeFolder()
{
	return data->relativeFolder;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Opens up the default configuration dialog for this Docklet.
void Docklet::DefaultConfigDialog()
{
	typedef void(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet);
	DUMMY_TYPEDEF HostDockletDefaultConfigDialog = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletDefaultConfigDialog");
	if(!HostDockletDefaultConfigDialog)
		return;

	HostDockletDefaultConfigDialog(data->hwndDocklet);
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Retrieves which edge of the screen the dock with this Docklet is located on.
Docklet::EDGE Docklet::QueryDockEdge()
{
	typedef int(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet);
	DUMMY_TYPEDEF HostDockletQueryDockEdge = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletQueryDockEdge");
	if(!HostDockletQueryDockEdge)
		return (Docklet::EDGE)0; //-1;

	return (Docklet::EDGE)HostDockletQueryDockEdge(data->hwndDocklet);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Retrieves the alignment of the dock with this Docklet
Docklet::ALIGN Docklet::QueryDockAlign() 
{
	typedef int(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet);
	DUMMY_TYPEDEF HostDockletQueryDockAlign = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletQueryDockAlign");
	if(!HostDockletQueryDockAlign)
		return (ALIGN)0; //-1;

	return (Docklet::ALIGN)HostDockletQueryDockAlign(data->hwndDocklet);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Changes which edge of the screen the dock with this Docklet is located on.
bool Docklet::SetDockEdge(Docklet::EDGE newEdge)
{
	typedef BOOL(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, int iNewEdge);
	DUMMY_TYPEDEF HostDockletSetDockEdge = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletSetDockEdge");
	if(!HostDockletSetDockEdge)
		return false;

	return HostDockletSetDockEdge(data->hwndDocklet, (int)newEdge) != 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	Changes the alignment of the dock with this Docklet.
bool Docklet::SetDockAlign(Docklet::ALIGN newAlign) 
{
	typedef BOOL(__stdcall *DUMMY_TYPEDEF)(HWND hwndDocklet, int iNewAlign);
	DUMMY_TYPEDEF HostDockletSetDockAlign = (DUMMY_TYPEDEF) GetProcAddress(GetModuleHandle(NULL), "DockletSetDockAlign");
	if(!HostDockletSetDockAlign)
		return false;

	return HostDockletSetDockAlign(data->hwndDocklet, (int)newAlign) != 0;
}




///////////////////////////////////////////////////////////////////////////////////////////////
///								HELPER FUNCTIONS										    ///
///////////////////////////////////////////////////////////////////////////////////////////////
int WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName)
{
	//////////////////////////////////////////////////////////////////////////
	///Helper function included to quickly & easily save integers to an Ini
	char szNumber[100];
	strcpy(szNumber, "");
	itoa(iValue, szNumber, 10);
	return WritePrivateProfileString(lpAppName, lpKeyName, szNumber, lpFileName);
}


/// EOF