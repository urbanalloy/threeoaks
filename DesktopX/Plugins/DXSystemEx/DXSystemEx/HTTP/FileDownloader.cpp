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

#include "stdafx.h"
#include "FileDownloader.h"

#include <io.h>
#include <Shlwapi.h>
#include <atlpath.h>
#include <SDPlugin.h>

static FileDownloader* pFileDownloader;
extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);

FileDownloader::FileDownloader(DWORD objID) : m_objID(objID),
											  m_hSession(NULL),
											  m_fClosing(FALSE)
{
	pFileDownloader = this;

	m_hShutdownEvent = CreateEvent(NULL,
								   TRUE,  // manual reset
								   FALSE, // not initially set
								   NULL);

	InitializeCriticalSection(&m_criticalSection);

	// Will get an internal error when calling download
	if (m_hShutdownEvent == NULL)
		return;

	// Enable tracing
	BOOL enable = TRUE;
	WinHttpSetOption(NULL, WINHTTP_OPTION_ENABLETRACING, &enable, sizeof(enable));

	// Open session
	m_hSession = WinHttpOpen(DXSYSTEMEX_USER_AGENT,
							 WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
							 WINHTTP_NO_PROXY_NAME,
							 WINHTTP_NO_PROXY_BYPASS,
							 WINHTTP_FLAG_ASYNC);
}

FileDownloader::~FileDownloader(void)
{
	// Check if any connection is still open
	if (!m_requests.empty()) {
		// Wait until all connections have been closed
		WaitForSingleObject(m_hShutdownEvent, /*60000*/INFINITE);
	}

	if (m_hShutdownEvent) {
		CloseHandle(m_hShutdownEvent);
		m_hShutdownEvent = NULL;
	}

	DeleteCriticalSection(&m_criticalSection);
}

void FileDownloader::Cleanup(void)
{
	// Set all connection to close
	m_fClosing = TRUE;

	for (int id = 0; id < (signed)m_requests.size(); id++)
		StopDownload(id);

	if (m_hSession) {
		WinHttpCloseHandle(m_hSession);
		m_hSession = NULL;
	}
}

void FileDownloader::OnHandleClosing(REQUEST_CONTEXT* context)
{
	// Remove context from connections
	m_requests.erase(context->id);

	delete context;
	context = NULL;

	if (m_fClosing)
		if (m_requests.empty())
			SetEvent(m_hShutdownEvent);
}

//////////////////////////////////////////////////////////////////////////
// DesktopX Callbacks
//////////////////////////////////////////////////////////////////////////

// SystemEx_OnDownloadFinish(id, status, http_code, path)
void FileDownloader::CompletionCallback(REQUEST_CONTEXT* context, DownloadStatus status)
{
	if (m_fClosing)
		return;

	SD_SCRIPTABLE_EVENT se;
	se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
	se.flags = 0;
	lstrcpy(se.szEventName, PLUGIN_PREFIX "OnDownloadFinish");

	// Message parameters
	memset(&se.dp, 0, sizeof(DISPPARAMS));
	se.dp.cArgs = 3;
	VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*3);
	VariantInit(&lpvt[0]);
	VariantInit(&lpvt[1]);
	VariantInit(&lpvt[2]);

	lpvt[2].vt = VT_I4;
	lpvt[2].lVal = context->id;

	lpvt[1].vt = VT_I4;
	lpvt[1].lVal = (int)status;

	CComBSTR bstr(context->localPath.c_str());
	lpvt[0].vt = VT_BSTR;
	lpvt[0].bstrVal = bstr.Detach();

	se.dp.rgvarg = lpvt;

	SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, m_objID, (DWORD) &se);

	free(se.dp.rgvarg);
}

// SystemEx_OnDownloadProgress(id, completed, total)
void FileDownloader::ProgressCallback(REQUEST_CONTEXT* context)
{
	if (m_fClosing)
		return;

	SD_SCRIPTABLE_EVENT se;
	se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
	se.flags = 0;
	lstrcpy(se.szEventName, PLUGIN_PREFIX "OnDownloadProgress");

	// Message parameters
	memset(&se.dp, 0, sizeof(DISPPARAMS));
	se.dp.cArgs = 3;
	VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*3);
	VariantInit(&lpvt[0]);
	VariantInit(&lpvt[1]);
	VariantInit(&lpvt[2]);

	lpvt[2].vt = VT_I4;
	lpvt[2].lVal = context->id;

	lpvt[1].vt = VT_I4;
	lpvt[1].lVal = (int)context->completedSize;

	lpvt[0].vt = VT_I4;
	lpvt[0].lVal = (int)context->totalSize;

	se.dp.rgvarg = lpvt;

	SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, m_objID, (DWORD) &se);

	free(se.dp.rgvarg);
}


//////////////////////////////////////////////////////////////////////////
// Connection
//////////////////////////////////////////////////////////////////////////
void FileDownloader::CloseConnection(REQUEST_CONTEXT* context)
{
	if (context == NULL)
		return;

	if (context->hRequest) {		
		context->hRequest = NULL;
		WinHttpCloseHandle(context->hRequest);
	}

	if (context->hConnect) {
		context->hConnect = NULL;
		WinHttpCloseHandle(context->hConnect);
	}

	if (context->buffer) {
		delete [] context->buffer;
		context->buffer = NULL;
	}

	// Remove context from connections
	m_requests.erase(context->id);
}

//////////////////////////////////////////////////////////////////////////
// Download
//////////////////////////////////////////////////////////////////////////
void FileDownloader::Download(int id, string remoteUrl, string localPath)
{
	// Initialize context
	REQUEST_CONTEXT* context = new REQUEST_CONTEXT();
	context->id = id;
	context->remoteUrl = remoteUrl;
	context->localPath = localPath;

	m_requests[id] = context;

	// check that the downloading session is opened correctly
	if (!m_hSession) {
		CompletionCallback(context, InternalError);
		goto error_exit;
	}

	// Check path for validity
	DownloadStatus pathStatus = IsPathValid(localPath);
	if (pathStatus != DownloadOk)
	{
		CompletionCallback(context, pathStatus);
		goto error_exit;
	}

	// Check the url
	URL_COMPONENTS UrlComponents;
	ZeroMemory(&UrlComponents, sizeof(UrlComponents));
	UrlComponents.dwStructSize = sizeof(UrlComponents);

	UrlComponents.dwSchemeLength    = (DWORD)-1;
	UrlComponents.dwHostNameLength  = (DWORD)-1;
	UrlComponents.dwUrlPathLength   = (DWORD)-1;

	USES_CONVERSION;
	if (WinHttpCrackUrl(A2W(remoteUrl.c_str()),  remoteUrl.length(), 0, &UrlComponents) == NULL)
	{
		CompletionCallback(context, InvalidUrl);
		goto error_exit;
	}

	// Connect to the server
	WCHAR wCharSave = UrlComponents.lpszHostName[UrlComponents.dwHostNameLength];
	UrlComponents.lpszHostName[UrlComponents.dwHostNameLength] = L'\0';

	context->hConnect = WinHttpConnect(m_hSession, UrlComponents.lpszHostName, UrlComponents.nPort, 0);

	UrlComponents.lpszHostName[UrlComponents.dwHostNameLength] = wCharSave;

	if (context->hConnect == NULL)
	{
		CompletionCallback(context, ConnectionOpenFailed);
		goto error_exit;
	}

	// Open the request
	context->hRequest = WinHttpOpenRequest(context->hConnect,
										   L"GET",
									       UrlComponents.lpszUrlPath,
									       L"HTTP/1.1",
										   WINHTTP_NO_REFERER,
										   WINHTTP_DEFAULT_ACCEPT_TYPES,
									       (UrlComponents.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);

	if (context->hRequest == NULL)
	{
		CompletionCallback(context, RequestOpenFailed);
		goto error_exit;
	}

	// Set the callback
	WinHttpSetStatusCallback(context->hRequest,
							 (WINHTTP_STATUS_CALLBACK)&DownloadStatusCallback,
						     WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS |
							 WINHTTP_CALLBACK_FLAG_HANDLES,   // to listen to the HANDLE_CLOSING event
							 0);


	// Send a request
	if (WinHttpSendRequest(context->hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, (DWORD_PTR)context) == FALSE)
	{
		CompletionCallback(context, RequestSendFailed);
		goto error_exit;
	}

	return;

error_exit:
	CloseConnection(context);
}

void FileDownloader::StopDownload(int id)
{
	RequestIterator it = m_requests.find(id);
	bool isDownloadPresent = !(it == m_requests.end());

	// If the download does not exist, do not do anything
	if (!isDownloadPresent) {
		return;
	}

	REQUEST_CONTEXT* context = (*it).second;
	CompletionCallback(context, DownloadCancelled);
	CloseConnection(context);
}

//////////////////////////////////////////////////////////////////////////
// Download Callback
//////////////////////////////////////////////////////////////////////////
void CALLBACK DownloadStatusCallback(HINTERNET,
									 DWORD_PTR dwContext,
									 DWORD dwInternetStatus,
									 LPVOID lpvStatusInformation,
									 DWORD dwStatusInformationLength)
{
	FileDownloader::REQUEST_CONTEXT* context = (FileDownloader::REQUEST_CONTEXT*)dwContext;

	// Cleanup on closing
	if (pFileDownloader->m_fClosing) {
		WinHttpSetStatusCallback(context->hRequest, NULL, NULL, NULL);
		SetEvent(pFileDownloader->m_hShutdownEvent);
		return;
	}

	switch(dwInternetStatus)
	{
		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
		{
			if (!context->hRequest || pFileDownloader->m_fClosing)
				return;

			// Prepare the request handle to receive a response.
			if(WinHttpReceiveResponse(context->hRequest, NULL ) == FALSE)
			{
				int error = GetLastError();
				FileDownloader::DownloadStatus status = FileDownloader::InternalError;

				switch(error)
				{
					case ERROR_WINHTTP_CANNOT_CONNECT:
						status = FileDownloader::CannotConnect;
						break;

					case ERROR_WINHTTP_CONNECTION_ERROR:
						status = FileDownloader::ConnectionError;
						break;

					case ERROR_WINHTTP_INVALID_URL:
						status = FileDownloader::InvalidUrl;
						break;

					case ERROR_WINHTTP_NAME_NOT_RESOLVED:
						status = FileDownloader::NameNotResolved;
						break;

					case ERROR_WINHTTP_SECURE_FAILURE:
						status = FileDownloader::SecureFailure;
						break;

					case ERROR_WINHTTP_TIMEOUT:
						status = FileDownloader::TimeOut;
						break;

					default:
						break;
				}

				pFileDownloader->CompletionCallback(context, status);
				pFileDownloader->CloseConnection(context);
			}
			break;
		}

		case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
		{
			if (!context->hRequest || pFileDownloader->m_fClosing)
				return;

			DWORD dwStatusCode = 0;
			DWORD dwSize = sizeof(DWORD);
			DWORD dwContentLength = 0;
			DWORD dwCLSize = sizeof(DWORD);

			// Get the status code
			if (WinHttpQueryHeaders(context->hRequest,
									WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
									NULL,
									&dwStatusCode,
									&dwSize,
									NULL) == FALSE)
			{
				pFileDownloader->CompletionCallback(context, FileDownloader::QueryHeadersFailed);
				pFileDownloader->CloseConnection(context);
				return;
			}

			if (dwStatusCode != HTTP_STATUS_OK)
			{
				pFileDownloader->CompletionCallback(context, FileDownloader::HttpError);
				pFileDownloader->CloseConnection(context);
				return;
			}

			// Get the content-length
			if (WinHttpQueryHeaders(context->hRequest,
									WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
									NULL,
									&dwContentLength,
									&dwCLSize,
									NULL))
			{
				context->totalSize = dwContentLength;
			}

			// Start querying data
			if (WinHttpQueryDataAvailable(context->hRequest, NULL) == FALSE)
			{
				int error = GetLastError();
				FileDownloader::DownloadStatus status = FileDownloader::InternalError;

				switch(error)
				{
					case ERROR_WINHTTP_CONNECTION_ERROR:
						status = FileDownloader::ConnectionError;
						break;

					case ERROR_WINHTTP_TIMEOUT:
						status = FileDownloader::TimeOut;
						break;

					default:
						break;
				}

				pFileDownloader->CompletionCallback(context, status);
				pFileDownloader->CloseConnection(context);
			}
			break;
		}

		case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
		{
			if (!context->hRequest || pFileDownloader->m_fClosing)
				return;

			context->receivedSize = *((LPDWORD)lpvStatusInformation);

			// If there is no data, the process is complete.
			if (context->receivedSize == 0)
			{
				pFileDownloader->ProgressCallback(context);
				FileDownloader::DownloadStatus saveStatus = pFileDownloader->SaveFile(context);
				pFileDownloader->CompletionCallback(context, saveStatus);
				pFileDownloader->CloseConnection(context);
			}
			else
			// Otherwise, if there is more data, read the next block
			{
				LPSTR lpOutBuffer = new char[context->receivedSize + 1];
				ZeroMemory(lpOutBuffer, context->receivedSize + 1);

				if(WinHttpReadData(context->hRequest, (LPVOID)lpOutBuffer, context->receivedSize, NULL ) == FALSE)
				{
					int error = GetLastError();
					FileDownloader::DownloadStatus status = FileDownloader::InternalError;

					switch(error)
					{
						case ERROR_WINHTTP_CONNECTION_ERROR:
							status = FileDownloader::ConnectionError;
							break;

						case ERROR_WINHTTP_TIMEOUT:
							status = FileDownloader::TimeOut;
							break;

						default:
							break;
					}

					delete [] lpOutBuffer;
					pFileDownloader->CompletionCallback(context, status);
					pFileDownloader->CloseConnection(context);
				}
			}
			break;
		}

		case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
		{
			if (!context->hRequest || pFileDownloader->m_fClosing)
				return;

			DWORD dwBytesRead  = dwStatusInformationLength;
			LPSTR lpReadBuffer = (LPSTR) lpvStatusInformation;

			// Copy the data and delete the buffers.
			if(dwBytesRead != 0)
			{
				context->receivedSize = dwBytesRead;

				if(!context->buffer)
				{
					// If there is no document buffer, use the current temporary one.
					context->buffer = lpReadBuffer;
				}
				else
				{
					// Save the previous buffer, and create a new one big enough to hold both the old and new data.
					LPSTR lpOldBuffer = context->buffer;
					context->buffer = new char[context->completedSize + context->receivedSize];

					// Copy the old and read buffer into the new context buffer.
					memcpy(context->buffer, lpOldBuffer, context->completedSize);
					memcpy(context->buffer + context->completedSize, lpReadBuffer, context->receivedSize);

					// Free the memory allocated to the old and read buffers.
					delete [] lpOldBuffer;
					delete [] lpReadBuffer;
				}

				// Keep track of the completed size.
				context->completedSize += context->receivedSize;
				pFileDownloader->ProgressCallback(context);
			}

			if (!context->hRequest || pFileDownloader->m_fClosing)
				return;

			// Now check for more data. If there is no more,
			// close the context
			if (WinHttpQueryDataAvailable(context->hRequest, NULL) == FALSE)
			{
				int error = GetLastError();
				FileDownloader::DownloadStatus status = FileDownloader::InternalError;

				switch(error)
				{
				case ERROR_WINHTTP_CONNECTION_ERROR:
					status = FileDownloader::ConnectionError;
					break;

				case ERROR_WINHTTP_TIMEOUT:
					status = FileDownloader::TimeOut;
					break;

				default:
					break;
				}

				pFileDownloader->CompletionCallback(context, status);
				pFileDownloader->CloseConnection(context);
			}
			break;
		}

		case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
		{
			pFileDownloader->OnHandleClosing(context);
			break;
		}

		default:
			break;
	}
}


//////////////////////////////////////////////////////////////////////////
// Helper methods
//////////////////////////////////////////////////////////////////////////
bool FileDownloader::IsValid(int id)
{
	bool isDownloadPresent = (m_requests.find(id) == m_requests.end());

	return (isDownloadPresent);
}

FileDownloader::DownloadStatus FileDownloader::IsPathValid(string path)
{
	// Path length
	if (path.length() > MAX_PATH)
		return InvalidPath;

	CPath filePath(path.c_str());

	// Check that path is absolute
	if (filePath.IsRelative())
		return InvalidPath;

	// Path should point to a file, not a folder
	if (filePath.IsFileSpec())
		return InvalidPath;

	BOOL fileExists = filePath.FileExists();

	// Check that the folder part exists
	filePath.RemoveFileSpec();
	if (!filePath.IsDirectory())
		return FolderDoNotExists;

	// Check that we can write to this folder
	if (_access(filePath.m_strPath, 2 /* write */) == -1)
		return FolderNotWritable;

	// Check that we can write to the file
	if (fileExists)
		if (_access(path.c_str(), 2) == -1)
			return FileNotWritable;

	return DownloadOk;
}

FileDownloader::DownloadStatus FileDownloader::SaveFile(REQUEST_CONTEXT* context)
{
	DownloadStatus pathStatus = IsPathValid(context->localPath);
	if (pathStatus != DownloadOk)
		return pathStatus;

	// Open file for writing
	HANDLE hFile;
	DWORD dwBytesToWrite = context->completedSize;
	DWORD dwBytesWritten = 0;

	hFile = CreateFile(context->localPath.c_str(),
					   GENERIC_WRITE,          // open for writing
					   0,                      // do not share
					   NULL,                   // default security
					   CREATE_ALWAYS,          // overwrite existing
					   FILE_ATTRIBUTE_NORMAL,  // normal file
					   NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FileNotWritable;

	// Loop until all the buffer is written to the file (in most case, will not repeat)
	while (dwBytesWritten < dwBytesToWrite)
	{
		if(WriteFile(hFile,
					 context->buffer + dwBytesWritten,      // start of data to write
					 dwBytesToWrite - dwBytesWritten,		// number of bytes to write
					 &dwBytesWritten,						// number of bytes that were written
					 NULL) == FALSE)
		{
			CloseHandle(hFile);
			return FileNotWritable;
		}
	}

	CloseHandle(hFile);

	return DownloadOk;
}