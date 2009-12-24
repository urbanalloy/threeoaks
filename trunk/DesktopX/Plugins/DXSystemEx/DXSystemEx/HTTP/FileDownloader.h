///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
//
// Copyright (c) 2009-2010, Julien Templier
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

#pragma once
#include "stdafx.h"
#include <Winhttp.h>

#define DXSYSTEMEX_USER_AGENT L"DXSystemEx Downloader/1.0"

void static CALLBACK DownloadStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

#include <string>
#include <map>
using namespace std;

class FileDownloader
{
public:
	enum DownloadStatus {
		DownloadOk = 0,
		DownloadCancelled,

		// Connection / Request
		InvalidUrl,
		CannotConnect,
		ConnectionError,
		NameNotResolved,
		SecureFailure,
		TimeOut,
		HttpError,
		ReadError,

		// Download Folder
		InvalidPath = 50,
		FolderDoNotExists,
		FolderNotWritable,
		FileNotWritable,

		// Internal errors
		InternalError = 100,
		ConnectionOpenFailed,
		RequestOpenFailed,
		RequestSendFailed,
		QueryHeadersFailed
	};

	struct REQUEST_CONTEXT {
		// Connection
		HINTERNET   hConnect;       // Connection handle
		HINTERNET   hRequest;       // Resource request handle

		// Request
		int			id;
		string		remoteUrl;
		string		localPath;

		// Progress
		DWORD		completedSize;   // Size of the completed data
		DWORD       totalSize;		 // Size of the total data
		DWORD       receivedSize;	 // Size of the data received
		LPSTR       buffer;			 // Buffer for storing read data

		REQUEST_CONTEXT() : hConnect(NULL),
							hRequest(NULL),
							id(0),
							remoteUrl(""),
							localPath(""),
							completedSize(0),
							totalSize(0),
							receivedSize(0),
							buffer(NULL)
		{
		}
	};

private:
	// Session handle
	HINTERNET m_hSession;

	// Cleanup event
	HANDLE m_hShutdownEvent;

	// DesktopX objectID
	DWORD m_objID;

	typedef map<int, REQUEST_CONTEXT*>::iterator RequestIterator;
	map<int, REQUEST_CONTEXT*> m_requests;

public:
	FileDownloader(DWORD objID);
	~FileDownloader();

	bool IsValid(int id);
	DownloadStatus IsPathValid(string path);
	void Download(int id, string remoteUrl, string localPath);
	void StopDownload(int id);

	void Cleanup();

	// Used by callback
	CRITICAL_SECTION m_criticalSection;
	BOOL m_fClosing;
	void OnHandleClosing(REQUEST_CONTEXT* context);
	void CloseConnection(REQUEST_CONTEXT* context);
	DownloadStatus SaveFile(REQUEST_CONTEXT* context);

	// DX callbacks
	void CompletionCallback(REQUEST_CONTEXT* context, DownloadStatus status);
	void ProgressCallback(REQUEST_CONTEXT* context);
};
