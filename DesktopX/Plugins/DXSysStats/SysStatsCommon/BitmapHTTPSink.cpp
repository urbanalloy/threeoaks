// BitmapHTTPSink.cpp: implementation of the BitmapHTTPSink class.
//
//////////////////////////////////////////////////////////////////////

#import "WinHttp.dll" raw_interfaces_only no_namespace

#include <atlbase.h>

#include "RemoteBitmap.h"
#include "BitmapHTTPSink.h"

DWORD WINAPI BitmapHTTPSink::_updateThread(LPVOID in)
{
	BitmapHTTPSink *ut = (BitmapHTTPSink*)in;
	DWORD ret = 0;

	try {
		ret = ut->updateThread();
		delete ut;
	} catch (...) {
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitmapHTTPSink::BitmapHTTPSink(RemoteBitmap *bmp, BSTR bstrURL, CSRef *cs) :
	bmp(bmp),
	pIWinHttpRequest(0),
	bstrURL(::SysAllocString(bstrURL)),
	localBmp(0),
	cs(cs)
{
	cs->AddRef();
	DWORD threadID;

	hUpdateThread = CreateThread(NULL, 0, _updateThread, (VOID *) this, 0, &threadID);
}

BitmapHTTPSink::~BitmapHTTPSink()
{
    if (pIWinHttpRequest)
        pIWinHttpRequest->Release();

	CloseHandle(hUpdateThread);
	hUpdateThread = NULL;

	{
		SysStatsUtils::Lock lock(cs);
		if (bmp)
			bmp->loadThread = NULL;
		bmp = NULL;
	}
	cs->RemoveRef();
	::SysFreeString(bstrURL);
}

void BitmapHTTPSink::ReadResponse()
{
	AtlTrace("Reading response\n");
	HRESULT hr;
    VARIANT varResponse;
	::VariantInit(&varResponse);

	hr = pIWinHttpRequest->get_ResponseStream(&varResponse);

	if (SUCCEEDED(hr))
	{    // Print response to file temp1.gif.
		IStream*    pStream = NULL; 
		HANDLE      hf = INVALID_HANDLE_VALUE; 
		// Check that an IStream was received.
		if (VT_UNKNOWN == V_VT(&varResponse) || 
			VT_STREAM == V_VT(&varResponse)) 
		{ 
			// Get IStream interface pStream.
			hr = V_UNKNOWN(&varResponse)->QueryInterface(IID_IStream, 
								 reinterpret_cast<void**>(&pStream)); 
			// Check(hr, "QI for IStream"); 
		} 
		else 
		{ 
			AtlTrace(L"Unexpected vartype for Response\n"); 
			return; 
		} 

#ifdef notdef
		BYTE        bBuffer[8192]; 
		DWORD       cb, cbRead, cbWritten; 
		HANDLE hFile; 
		// Open file Temp2.gif for output.
		hFile = CreateFile("Temp2.gif",     
			GENERIC_WRITE,                // Open for writing. 
			0,                            // Do not share. 
			NULL,                         // No security. 
			CREATE_ALWAYS,                // Overwrite existing.
			FILE_ATTRIBUTE_NORMAL,        // Normal file.
			NULL);                        // No attribute template.
		if (hFile == INVALID_HANDLE_VALUE) 
		{
			wprintf(L"Could not open file.");  // Process error.
		}
		else
		{
			// Copy data from the response stream to file. 
			cb = sizeof(bBuffer); 
			hr = pStream->Read(bBuffer, cb, &cbRead); 
			while (SUCCEEDED(hr) && 0 != cbRead) 
			{ 
				if (!WriteFile(hFile, bBuffer, 
							   cbRead, &cbWritten, NULL)) 
				{ 
					TraceErr("WriteFile fails with 0x%08lx\n", 
							 HRESULT_FROM_WIN32(GetLastError())); 
					return -1; 
				} 
				hr = pStream->Read(bBuffer, cb, &cbRead); 
			} 
		}
		CloseHandle(hFile);
#else
		if (SUCCEEDED(hr)) {
			localBmp = Bitmap::FromStream(pStream);
			pStream->Release(); 
		}

		AtlTrace("Image loaded\n");

		SysStatsUtils::Lock lock(cs);
		if (bmp) {
			if (bmp->bmp)
				delete bmp->bmp;

			bmp->bmp = localBmp;
			bmp->notifier.loaded();
			bmp->loadThread = NULL;
			bmp = NULL;
		}
#endif
	}
	else
		AtlTrace(L"ReadResponse: failed to load image %s - %d\n", bstrURL, hr);

    // Release memory.
	VariantClear(&varResponse);
}

HRESULT BitmapHTTPSink::SendRequest(BSTR bstrUrl)
{
    VARIANT varAsync;
    VARIANT varEmpty;
    
    VariantInit(&varAsync);
    V_VT(&varAsync)   = VT_BOOL;
    V_BOOL(&varAsync) = VARIANT_FALSE;

    VariantInit(&varEmpty);
    V_VT(&varEmpty) = VT_ERROR;

    CLSID           clsid;
	HRESULT hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(clsid, NULL, 
							  CLSCTX_INPROC_SERVER, 
							  __uuidof(IWinHttpRequest), 
							  (void **)&pIWinHttpRequest);
	}

	if (SUCCEEDED(hr))
	{    // Open WinHttpRequest for synchronous access.
		hr = pIWinHttpRequest->Open(L"GET", bstrUrl, varAsync);
	}
	if (SUCCEEDED(hr))
	{    // Send Request.
		hr = pIWinHttpRequest->Send(varEmpty);
	}

	VariantClear(&varAsync);
	VariantClear(&varEmpty);

	return hr;
}

/*
 * The thread that retrieves the data
 */
DWORD WINAPI BitmapHTTPSink::updateThread(/*LPVOID lpInput*/)
{
	::CoInitialize(NULL);

	HRESULT hr = SendRequest(bstrURL);
	if (SUCCEEDED(hr))
		ReadResponse();
	else
		AtlTrace(L"Send: failed to load image %s - %d\n", bstrURL, hr);

	{
		SysStatsUtils::Lock lock(cs);
		if (bmp)
			bmp->loadThread = NULL;
		bmp = NULL;
	}

	::CoUninitialize();

	return 0;
}
