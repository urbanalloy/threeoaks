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

#include <SDPlugin.h>
#include <atlpath.h>
#include <shlobj.h>

extern HINSTANCE g_hInstance;	
extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);

void CArchive::clear() 
{
	m_filename = "";
	m_path = "";
	m_password = "";
	m_files.empty();
	m_type = kArchiveNone;

#if ENABLE_ZIP_FALLBACK
	if (!m_hZip)
		return;

	CloseZip(m_hZip);
	m_hZip = NULL;
#endif
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

void CArchive::load7zip() {
	char pluginsPath[MAX_PATH], objectPath[MAX_PATH], programFilesPath[MAX_PATH], dll7zip[MAX_PATH];

	GetModuleFileName(g_hInstance, pluginsPath, MAX_PATH);
	SDHostMessage(SD_GET_OBJECT_DIRECTORY, (DWORD)objectPath, 0);
	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, (char *)&programFilesPath);
	sprintf_s((char *)&programFilesPath, MAX_PATH, "%s\\7-Zip\\7z.dll", (char *)&programFilesPath);

#define TRYLOAD_7ZLIB(path)  \
	{ \
		CPath dllPath(path); \
		dllPath.RemoveFileSpec(); \
		sprintf_s((char*)&dll7zip, MAX_PATH, "%s\\7z.dll", (LPCTSTR)dllPath); \
		m_h7zip = LoadLibrary((char*)&dll7zip); \
		if (m_h7zip) \
			return; \
	}

	TRYLOAD_7ZLIB(objectPath)
	TRYLOAD_7ZLIB(pluginsPath)
	TRYLOAD_7ZLIB(programFilesPath)
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

	for (unsigned int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
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
	*count = (long)m_files.size();

	return S_OK;
}

STDMETHODIMP CArchive::get_Items(VARIANT *items)
{
	// Create SafeArray of VARIANTS
	SAFEARRAY *pSA;
	SAFEARRAYBOUND aDim[1];

	aDim[0].lLbound = 0;
	aDim[0].cElements = m_files.size(); // when no file is opened, will return an empty array

	pSA = SafeArrayCreate(VT_VARIANT, 1, aDim);

	if (pSA != NULL && m_files.size() > 0) {
		for (long l = aDim[0].lLbound; l < (signed)(aDim[0].cElements + aDim[0].lLbound); l++) {

			VARIANT vOut;
			VariantInit(&vOut);
			vOut.vt = VT_DISPATCH;

			// Init MonitorInfo
			CComObject<CArchiveItem>* pArchiveItem;
			CComObject<CArchiveItem>::CreateInstance(&pArchiveItem);
			pArchiveItem->Init(m_files[l]);
			pArchiveItem->QueryInterface(IID_IArchiveItem, (void**)&vOut.pdispVal);

			HRESULT hr = SafeArrayPutElement(pSA, &l, &vOut);

			if (FAILED(hr)) {
				VariantClear(&vOut);
				SafeArrayDestroy(pSA); // does a deep destroy of source VARIANT

				return hr;
			}

			VariantClear(&vOut);
		}
	}

	// return SafeArray as VARIANT
	V_VT(items) = VT_ARRAY | VT_VARIANT;
	V_ARRAY(items)= pSA;

	return S_OK;
}

STDMETHODIMP CArchive::get_Type(int* type)
{
	*type = m_type;

	return S_OK;
}

STDMETHODIMP CArchive::get_SupportedExtensions(VARIANT* extensions) 
{	
	// Compute extensions
	vector<string> exts;

	if (m_h7zip) {

	} else {
#if ENABLE_ZIP_FALLBACK	
		exts.push_back("zip");
#endif
	}

	// Create SafeArray of VARIANT BSTRs
	USES_CONVERSION;
	SAFEARRAY *pSA;
	SAFEARRAYBOUND aDim[1];

	aDim[0].lLbound = 0;
	aDim[0].cElements = exts.size(); // when no file is opened, will return an empty array

	pSA = SafeArrayCreate(VT_VARIANT, 1, aDim);

	if (pSA != NULL && aDim[0].cElements > 0) {
		variant_t vOut;

		for (long l = aDim[0].lLbound; l < (signed)(aDim[0].cElements + aDim[0].lLbound); l++) {
			vOut = (OLECHAR*) T2OLE(exts[l].c_str());

			HRESULT hr = SafeArrayPutElement(pSA, &l, &vOut);

			if (FAILED(hr)) {
				SafeArrayDestroy(pSA); // does a deep destroy
				return hr;
			}
		}	

	}
	
	// return SafeArray as VARIANT
	V_VT(extensions) = VT_ARRAY | VT_VARIANT;
	V_ARRAY(extensions)= pSA;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CArchive::Create(BSTR filePath, int type, VARIANT_BOOL* status)
{
	USES_CONVERSION;

	// Close previously opened/created file, if any
	clear();

	//if (m_h7zip) {
	// switch type for archive type
	//	return S_OK;
	//}

#if ENABLE_ZIP_FALLBACK	
	if (type != kArchiveZip)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("Error: Not available"), "The plugin cannot find the 7zip dll and the zip fallback path only supports ZIP archives", 0, NULL);		

	m_hZip = CreateZip(W2A(filePath), 0, ZIP_FILENAME, LPCTSTR(m_password));
	if (!m_hZip) {
		*status = VARIANT_FALSE;
		return S_FALSE;
	}

	m_type = kArchiveZip;

	// Store filename & path
	parseInputFilename(filePath);
	*status = VARIANT_TRUE;

	return S_OK;
#else 
	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("Error: Not available"), "The plugin cannot find the 7zip dll and the zip fallback path was not compiled in!", 0, NULL);	
#endif
}

STDMETHODIMP CArchive::Open(BSTR filePath, VARIANT_BOOL* status) 
{
	USES_CONVERSION;
	
	// Close previously opened/created file, if any
	clear();	

	//if (m_h7zip) {
	//	return S_OK;
	//}

#if ENABLE_ZIP_FALLBACK	
	ZIPENTRY zipEntry;
	ZIPENTRY zipItem;

	m_hZip = OpenZip(W2A(filePath), 0, ZIP_FILENAME, LPCTSTR(m_password));
	if (!m_hZip) {
		*status = VARIANT_FALSE;
		return S_FALSE;
	}

	m_type = kArchiveZip;

	// Store filename & path
	parseInputFilename(filePath);

	GetZipItem(m_hZip, -1, &zipEntry);
	for (int i = 0; i < zipEntry.index; i++)
	{
		CArchiveItem::ArchiveItem archiveItem;

		GetZipItem(m_hZip, i, &zipItem);
		archiveItem.filename = zipItem.name;
		archiveItem.isDirectory = (bool)(zipItem.attr & FILE_ATTRIBUTE_DIRECTORY);
		archiveItem.compressedSize = zipItem.comp_size;
		m_files.push_back(archiveItem);
	}
	
	*status = VARIANT_TRUE;
	return S_OK;
#else 
	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("Error: Not available"), "The plugin cannot find the 7zip dll and the zip fallback path was not compiled in!", 0, NULL);	
#endif
}

STDMETHODIMP CArchive::ExistsFile(BSTR filename, VARIANT_BOOL* result)
{
	if (!m_hZip && !m_h7zip)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("No file opened!"), "You need to create or open a zip file before calling ExistsFile", 0, NULL);

	*result = FALSE;
	_bstr_t itemName = filename;

	for (ArchiveItemList::iterator iterBegin = m_files.begin(); iterBegin != m_files.end(); ++iterBegin)
		if (itemName == ((CArchiveItem::ArchiveItem)*iterBegin).filename) {
			*result = TRUE;
			break;
		}

	return S_OK;
}

STDMETHODIMP CArchive::AddFile(BSTR fileName, VARIANT_BOOL* status)
{
	if (!m_hZip && !m_h7zip)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("No file opened!"), "You need to create or open a zip file before calling ExistsFile", 0, NULL);

	//if (m_h7zip) {
	//	return S_OK;
	//}

#if ENABLE_ZIP_FALLBACK
	CArchiveItem::ArchiveItem archiveItem;
	*status = VARIANT_FALSE;

	// can add ONLY if the zip file is being created
	archiveItem.filename = fileName;
	m_files.push_back(archiveItem);

	_bstr_t fullFilename = m_inputFolder + _bstr_t(_T("\\")) + archiveItem.filename;
	if (ZipAdd(m_hZip, archiveItem.filename, (char *)fullFilename, 0, ZIP_FILENAME) == ZR_OK) {
		*status = VARIANT_TRUE;
		// TODO update compressed size
	}

	return S_OK;
#else 
	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("Error: Not available"), "The plugin cannot find the 7zip dll and the zip fallback path was not compiled in!", 0, NULL);	
#endif
}

STDMETHODIMP CArchive::ExtractFile(BSTR filename, BSTR outputPath, VARIANT_BOOL* status)
{
	if (!m_hZip && !m_h7zip)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("No file opened!"), "You need to create or open a zip file before calling ExtractFile", 0, NULL);

	*status = VARIANT_FALSE;

	//if (m_h7zip) {
	//	return S_OK;
	//}

#if ENABLE_ZIP_FALLBACK
	_bstr_t itemName = filename;

	// TODO rewrite and remove index
	int i = 0;
	for (ArchiveItemList::iterator iterBegin = m_files.begin(); iterBegin != m_files.end(); ++iterBegin)
	{
		CArchiveItem::ArchiveItem archiveItem;

		archiveItem = *iterBegin;
		if (itemName == archiveItem.filename) {
			_bstr_t outputFile = outputPath + _bstr_t(_T("\\")) + _bstr_t(archiveItem.filename);
			UnzipItem(m_hZip, i, (char *)outputFile, 0, ZIP_FILENAME);
			break;
		}

		i++;
	}

	return S_OK;
#else 
	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("Error: Not available"), "The plugin cannot find the 7zip dll and the zip fallback path was not compiled in!", 0, NULL);	
#endif
}

STDMETHODIMP CArchive::Extract(BSTR outputDirectory, VARIANT_BOOL* status)
{	
	USES_CONVERSION;
	_bstr_t outputFile;
	_bstr_t outputPath = outputDirectory;

	*status = VARIANT_FALSE;

	//if (m_h7zip) {
	//	return S_OK;
	//}

#if ENABLE_ZIP_FALLBACK
	ZIPENTRY zipEntry;
	ZIPENTRY zipItem;

	GetZipItem(m_hZip, -1, &zipEntry);
	for (int i = 0; i < zipEntry.index; i++)
	{
		CArchiveItem::ArchiveItem archiveItem;

		GetZipItem(m_hZip, i, &zipItem);
		outputFile = outputPath + _bstr_t(_T("\\")) + _bstr_t(zipItem.name);

		// TODO add error handling
		UnzipItem(m_hZip, i, (char *)outputFile, 0, ZIP_FILENAME);
	}

	*status = VARIANT_TRUE;
	return S_OK;
#else 
	return CCOMError::DispatchError(SYNTAX_ERR, CLSID_Archive, _T("Error: Not available"), "The plugin cannot find the 7zip dll and the zip fallback path was not compiled in!", 0, NULL);	
#endif
}

STDMETHODIMP CArchive::Close()
{
	// Close previously opened/created file, if any
	clear();

	return S_OK;
}

STDMETHODIMP CArchive::IsArchive(BSTR filename, VARIANT_BOOL* status)
{
	*status = VARIANT_FALSE;

	_bstr_t file(filename); 
	CString s;
	s.Format(_T("%s"), (LPCTSTR)file);

	CPath path(s);
	CString ext = path.GetExtension();

	//if (m_h7zip) {
	//	*status = VARIANT_FALSE;

	//	return S_OK;
	//}

#if ENABLE_ZIP_FALLBACK
	if (ext == ".zip")
		*status = VARIANT_TRUE;
#endif

	return S_OK;
}
