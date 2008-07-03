class Animation
{
public:
	Animation(HWND hwndDocklet, HINSTANCE hInstance);
	~Animation();

	void SetImage(char *szImage);
	Image *NextFrameImage();

	HWND hwndDocklet;
	HINSTANCE hInstance;

	int GetImage(char *szImageRelative);

private:

	static DWORD WINAPI UpdateImageThread(LPVOID lpInput);

	void StartUpdateThread();
	void EndUpdateThread();
	HANDLE hUpdateThread;


	int iFrame;


	void ReleaseImage();
	char szImage[MAX_PATH];
	Bitmap *imageMain;

	void CreateFrames();
	void ReleaseFrames();
	Image **images;
	int nFrames;
};