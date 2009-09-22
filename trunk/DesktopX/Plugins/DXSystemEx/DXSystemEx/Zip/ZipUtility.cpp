///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
//
// Copyright (c) 2009, Julien Templier
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
#include "Zip/ZipUtility.h"

#include "Zip/XZip.h"
#include "Zip/XUnzip.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CZipUtility::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IZipUtility
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
STDMETHODIMP CZipUtility::get_InputPath(BSTR *pVal)
{
	*pVal = m_bstrInputPath.copy();

	return S_OK;
}

STDMETHODIMP CZipUtility::put_InputPath(BSTR newVal)
{
	m_bstrInputPath = newVal;

	return S_OK;
}

STDMETHODIMP CZipUtility::get_OutputPath(BSTR *pVal)
{
	*pVal = m_bstrOutputPath.copy();

	return S_OK;
}

STDMETHODIMP CZipUtility::put_OutputPath(BSTR newVal)
{
	m_bstrOutputPath = newVal;

	return S_OK;
}

STDMETHODIMP CZipUtility::get_FileName(BSTR *pVal)
{
	*pVal = m_bstrFileName.copy();

	return S_OK;
}

STDMETHODIMP CZipUtility::put_FileName(BSTR newVal)
{
	m_bstrFileName = newVal;

	return S_OK;
}

STDMETHODIMP CZipUtility::AddFile(BSTR strFileName, BSTR strNewName)
{
	ZipFileInfo zipInfo;

	// can add ONLY if the zip file is being created
	zipInfo.bstrFileName = strFileName;
	zipInfo.bstrNewName = strNewName;
	zipInfo.bstrPath = m_bstrInputPath;
	m_vtrFiles.push_back(zipInfo);

	return S_OK;
}

STDMETHODIMP CZipUtility::get_Count(long *pVal)
{
	*pVal = (long)m_vtrFiles.size();

	return S_OK;
}

STDMETHODIMP CZipUtility::Zip()
{
	HZIP hZip;
	_bstr_t bstrFullFileName;

	hZip = CreateZip((char *)CalcFullFileName(), 0, ZIP_FILENAME);
	if (!hZip) return S_FALSE;

	for (ZipFileInfoVtr::iterator iterBegin = m_vtrFiles.begin(); iterBegin != m_vtrFiles.end(); ++iterBegin)
	{
		ZipFileInfo zipInfo = *iterBegin;

		bstrFullFileName = m_bstrInputPath + _bstr_t(_T("\\")) + zipInfo.bstrFileName;

		// TODO: Add error handling
		ZipAdd(hZip, zipInfo.bstrNewName, (char *)bstrFullFileName, 0, ZIP_FILENAME);
	}
	CloseZip(hZip);

	return S_OK;
}

STDMETHODIMP CZipUtility::Unzip()
{
	HZIP hZip;
	ZIPENTRY zipEntry;
	ZIPENTRY zipItem;
	_bstr_t bstrOutputFile;

	m_vtrFiles.empty();

	hZip = OpenZip((char *)CalcFullFileName(), 0, ZIP_FILENAME);
	if (!hZip) return S_FALSE;

	GetZipItem(hZip, -1, &zipEntry);
	for (int i = 0; i < zipEntry.index; i++)
	{
		ZipFileInfo zipInfo;

		GetZipItem(hZip, i, &zipItem);
		bstrOutputFile = m_bstrOutputPath + _bstr_t(_T("\\")) + _bstr_t(zipItem.name);

		UnzipItem(hZip, i, (char *)bstrOutputFile, 0, ZIP_FILENAME);
	}
	CloseZip(hZip);

	return S_OK;
}

STDMETHODIMP CZipUtility::get_FullFileName(BSTR *pVal)
{
	*pVal = CalcFullFileName().copy();

	return S_OK;
}

_bstr_t CZipUtility::CalcFullFileName()
{
	_bstr_t bstrFullFile;

	bstrFullFile = m_bstrOutputPath;
	bstrFullFile += _bstr_t(_T("\\"));
	bstrFullFile += m_bstrFileName;
	return bstrFullFile;
}

STDMETHODIMP CZipUtility::Open()
{
	HZIP hZip;
	ZIPENTRY zipEntry;
	ZIPENTRY zipItem;

	m_vtrFiles.empty();

	hZip = OpenZip((char *)CalcFullFileName(), 0, ZIP_FILENAME);
	if (!hZip) return S_FALSE;

	GetZipItem(hZip, -1, &zipEntry);
	for (int i = 0; i < zipEntry.index; i++)
	{
		ZipFileInfo zipInfo;

		GetZipItem(hZip, i, &zipItem);
		zipInfo.bstrFileName = zipItem.name;
		zipInfo.bstrNewName = zipItem.name;
		zipInfo.lCompSize = zipItem.comp_size;
		m_vtrFiles.push_back(zipInfo);
	}
	CloseZip(hZip);

	return S_OK;
}

STDMETHODIMP CZipUtility::ExistsFile(BSTR strFileName, BOOL* pVal)
{
	BOOL bExists = FALSE;
	_bstr_t bstrFileName = strFileName;

	for (ZipFileInfoVtr::iterator iterBegin = m_vtrFiles.begin(); iterBegin != m_vtrFiles.end(); ++iterBegin)
	{
		ZipFileInfo zipInfo;

		zipInfo = *iterBegin;
		if (bstrFileName == zipInfo.bstrFileName) {
			bExists = TRUE;
			break;
		}
	}

	*pVal = bExists;

	return S_OK;
}
