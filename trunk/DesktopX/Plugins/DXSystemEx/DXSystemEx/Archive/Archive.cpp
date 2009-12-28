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

#include "Archive/Archive.h"

#include <atlpath.h>

void CArchive::clear() {
	
	m_filename = "";
	m_path = "";
	m_password = "";
	m_files.empty();

	if (!m_hZip)
		return;

	CloseZip(m_hZip);
	m_hZip = NULL;
}

void CArchive::parseInputFilename(BSTR file)
{
	_bstr_t bstrStart(file); 
	CString s;
	s.Format(_T("%s"), (LPCTSTR)bstrStart);

	CPath filename(s);
	filename.StripPath();	

	CPath path(s);
	path.RemoveFileSpec();

	m_path = LPCTSTR(path);
	m_filename = LPCTSTR(filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CArchive::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IArchive
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IZipUtility
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CArchive::get_InputFolder(BSTR *inputPath)
{
	*inputPath = m_inputFolder.copy();

	return S_OK;
}

STDMETHODIMP CArchive::put_InputFolder(BSTR inputPath)
{
	m_inputFolder = inputPath;

	return S_OK;
}

STDMETHODIMP CArchive::get_Path(BSTR *path)
{
	*path = m_path.copy();

	return S_OK;
}

STDMETHODIMP CArchive::get_FileName(BSTR *filename)
{
	*filename = m_filename.copy();

	return S_OK;
}

STDMETHODIMP CArchive::get_Password(BSTR *password)
{
	*password = m_password.copy();

	return S_OK;
}

STDMETHODIMP CArchive::put_Password(BSTR password)
{
	m_password = password;

	return S_OK;
}

STDMETHODIMP CArchive::get_Count(long *count)
{
	//if (!m_hZip)
	//	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_ZipFile, _T("No file opened!"), "You need to create or open a zip file before calling Count", 0, NULL);

	*count = (long)m_files.size();

	return S_OK;
}

STDMETHODIMP CArchive::get_Files(VARIANT *files)
{
	//if (!m_hZip)
	//	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_ZipFile, _T("No file opened!"), "You need to create or open a zip file before calling Files", 0, NULL);

	// Create SafeArray of VARIANT BSTRs
	SAFEARRAY *pSA;
	SAFEARRAYBOUND aDim[1];

	aDim[0].lLbound = 0;
	aDim[0].cElements = m_files.size(); // when no file is opened, will return an empty array

	pSA = SafeArrayCreate(VT_VARIANT, 1, aDim);

	if (pSA != NULL) {
		variant_t vOut;

		for (long l = aDim[0].lLbound; l < (signed)(aDim[0].cElements + aDim[0].lLbound); l++) {
			vOut = m_files[l].filename;

			HRESULT hr = SafeArrayPutElement(pSA, &l, &vOut);

			if (FAILED(hr)) {
				SafeArrayDestroy(pSA); // does a deep destroy
				return hr;
			}
		}
	}

	// return SafeArray as VARIANT
	V_VT(files) = VT_ARRAY | VT_VARIANT;
	V_ARRAY(files)= pSA;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CArchive::Create(BSTR filePath, VARIANT_BOOL* status)
{
	// Close previously opened/created file, if any
	clear();

	USES_CONVERSION;
	m_hZip = CreateZip(W2A(filePath), 0, ZIP_FILENAME, LPCTSTR(m_password));
	if (!m_hZip) {
		*status = VARIANT_FALSE;
		return S_FALSE;
	}

	// Store filename & path
	parseInputFilename(filePath);

	*status = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CArchive::Open(BSTR filePath, VARIANT_BOOL* status) 
{
	ZIPENTRY zipEntry;
	ZIPENTRY zipItem;

	// Close previously opened/created file, if any
	clear();	

	USES_CONVERSION;
	m_hZip = OpenZip(W2A(filePath), 0, ZIP_FILENAME, LPCTSTR(m_password));
	if (!m_hZip) {
		*status = VARIANT_FALSE;
		return S_FALSE;
	}

	// Store filename & path
	parseInputFilename(filePath);

	GetZipItem(m_hZip, -1, &zipEntry);
	for (int i = 0; i < zipEntry.index; i++)
	{
		FileInfo fileInfo;

		GetZipItem(m_hZip, i, &zipItem);
		fileInfo.filename = zipItem.name;
		fileInfo.newFilename = zipItem.name;
		fileInfo.compressedSize = zipItem.comp_size;
		m_files.push_back(fileInfo);
	}

	*status = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CArchive::ExistsFile(BSTR filename, VARIANT_BOOL* result)
{
	if (!m_hZip)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("No file opened!"), "You need to create or open a zip file before calling ExistsFile", 0, NULL);

	*result = FALSE;
	_bstr_t itemName = filename;

	for (FileInfoList::iterator iterBegin = m_files.begin(); iterBegin != m_files.end(); ++iterBegin)
	{
		FileInfo fileInfo;

		fileInfo = *iterBegin;
		if (itemName == fileInfo.filename) {
			*result = TRUE;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP CArchive::AddFile(BSTR strFileName, BSTR strNewName, VARIANT_BOOL* status)
{
	if (!m_hZip)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("No file opened!"), "You need to create or open a zip file before calling ExistsFile", 0, NULL);

	FileInfo fileInfo;
	*status = VARIANT_FALSE;

	// can add ONLY if the zip file is being created
	fileInfo.filename = strFileName;
	fileInfo.newFilename = strNewName;
	fileInfo.path = m_inputFolder;
	m_files.push_back(fileInfo);

	_bstr_t fullFilename = m_inputFolder + _bstr_t(_T("\\")) + fileInfo.filename;
	
	if (ZipAdd(m_hZip, fileInfo.filename, (char *)fullFilename, 0, ZIP_FILENAME) == ZR_OK)
		*status = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CArchive::ExtractFile(BSTR filename, BSTR outputPath, VARIANT_BOOL* status)
{
	if (!m_hZip)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("No file opened!"), "You need to create or open a zip file before calling ExtractFile", 0, NULL);

	*status = VARIANT_FALSE;

	_bstr_t itemName = filename;

	// TODO rewrite and remove index
	int i = 0;
	for (FileInfoList::iterator iterBegin = m_files.begin(); iterBegin != m_files.end(); ++iterBegin)
	{
		FileInfo fileInfo;

		fileInfo = *iterBegin;
		if (itemName == fileInfo.filename) {
			_bstr_t outputFile = outputPath + _bstr_t(_T("\\")) + _bstr_t(fileInfo.filename);
			UnzipItem(m_hZip, i, (char *)outputFile, 0, ZIP_FILENAME);
			break;
		}

		i++;
	}

	return S_OK;
}

STDMETHODIMP CArchive::Close()
{
	// Close previously opened/created file, if any
	clear();

	return S_OK;
}

STDMETHODIMP CArchive::Extract(BSTR filename, BSTR outputDirectory, VARIANT_BOOL* status)
{
	HZIP hZip;
	ZIPENTRY zipEntry;
	ZIPENTRY zipItem;
	_bstr_t bstrOutputFile;
	_bstr_t bstrOutputPath = outputDirectory;

	m_files.empty();

	USES_CONVERSION;
	hZip = OpenZip((char *)W2A(filename), 0, ZIP_FILENAME, LPCTSTR(m_password));
	if (!hZip) {
		*status = VARIANT_FALSE;
		return S_FALSE;
	}

	GetZipItem(hZip, -1, &zipEntry);
	for (int i = 0; i < zipEntry.index; i++)
	{
		FileInfo fileInfo;

		GetZipItem(hZip, i, &zipItem);
		bstrOutputFile = outputDirectory + _bstr_t(_T("\\")) + _bstr_t(zipItem.name);

		// TODO add error handling
		UnzipItem(hZip, i, (char *)bstrOutputFile, 0, ZIP_FILENAME);
	}
	CloseZip(hZip);

	*status = VARIANT_TRUE;
	return S_OK;
}
