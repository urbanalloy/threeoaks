#include <stdio.h>
#include <windows.h>
#include <DockletSDK.h>
#include "CAnimation.h"



Animation::Animation(HWND hwndDocklet, HINSTANCE hInstance)
{
	this->hwndDocklet = hwndDocklet;
	this->hInstance = hInstance;
	hUpdateThread = NULL;
	iFrame = 0;

	strcpy(szImage, "");
	imageMain = NULL;
	images = NULL;
	nFrames = NULL;
}

Animation::~Animation()
{
	ReleaseImage();
}


void Animation::SetImage(char *szImage)
{
	ReleaseImage();

	strcpy(this->szImage, szImage);
	imageMain = DockletLoadGDIPlusImage(szImage);

	CreateFrames();

	StartUpdateThread();
}

void Animation::ReleaseImage()
{
	EndUpdateThread();
	ReleaseFrames();


	if(imageMain)
		delete imageMain;
	imageMain = NULL;
}





void Animation::StartUpdateThread()
{
	EndUpdateThread();

	DWORD lpThreadId = NULL;
	hUpdateThread = CreateThread(NULL, 0, Animation::UpdateImageThread, (VOID *) this, 0, &lpThreadId);
}

void Animation::EndUpdateThread()
{
	if(!hUpdateThread)
		return;

	TerminateThread(hUpdateThread, 0);
	CloseHandle(hUpdateThread);
	hUpdateThread = NULL;
}


void Animation::CreateFrames()
{
	ReleaseFrames();


	int iWidth = imageMain->GetWidth();
	int iHeight = imageMain->GetHeight();

	if(!iWidth || !iHeight)
		return;


	nFrames = ((int)(iWidth / iHeight));
	iFrame = 0;


	images = new Image*[nFrames];

	int i;
	for(i = 0 ; i < nFrames ; i++)
	{
		images[i] = imageMain->Clone((i*iHeight), 0, iHeight, iHeight, PixelFormatDontCare);
	}
}

void Animation::ReleaseFrames()
{
	if(!images)
	{
		nFrames = 0;
		iFrame = 0;
		return;
	}


	int i;
	for(i = 0 ; i < nFrames ; i++)
	{
		if(images[i])
			delete images[i];
		images[i] = NULL;
	}

	delete images;

	images = NULL;
	nFrames = NULL;
}



Image *Animation::NextFrameImage()
{
	if(!images || (nFrames <= 0) )
		return NULL;

	Image *imageReturn = images[iFrame];

	iFrame++;
	iFrame %= nFrames;
	
	return imageReturn;
}


DWORD WINAPI Animation::UpdateImageThread(LPVOID lpInput)
{
	Animation *lpAnimation = (Animation *) lpInput;


	Image *imageNext = NULL;
	while( imageNext = lpAnimation->NextFrameImage() )
	{
		DockletSetImage(lpAnimation->hwndDocklet, imageNext, FALSE);

		Sleep(75);
	}

	DockletSetImage(lpAnimation->hwndDocklet, NULL, TRUE);

	return 0;
}

int Animation::GetImage(char *szImageRelative)
{
	if(szImageRelative)
		strcpy(szImageRelative, szImage);

	return strlen(szImage);
}