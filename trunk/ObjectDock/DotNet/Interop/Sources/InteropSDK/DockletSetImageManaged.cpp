///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2008, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer. 
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution. 
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "DockletSetImageManaged.h"
#include "DockletSDK.h"

void DockletSetImageManaged(HWND hwndDocklet, int* newImage, int width, int height, BOOL bOverlay)
{
	Bitmap* image = new Bitmap(width, height, PixelFormat32bppARGB);
	BitmapData* data = new BitmapData();

	Rect* rect = new Rect(0, 0, width, height);
	int status = image->LockBits(rect, ImageLockModeWrite, PixelFormat32bppARGB, data);

	if (status != Ok) {
		// Cleanup
		delete(rect);
		delete(data);
		delete(image);
		return;
	}

	// copy data from newImage
	// format: 32bppArgb
	memcpy(data->Scan0, newImage, width*height*4);

	image->UnlockBits(data);

	// Call into the SDK
	if (bOverlay)
		DockletSetImageOverlay(hwndDocklet, image, true);
	else
		DockletSetImage(hwndDocklet, image, true);

	// Cleanup
	delete(rect);
	delete(data);
	// image is automatically deleted
}
