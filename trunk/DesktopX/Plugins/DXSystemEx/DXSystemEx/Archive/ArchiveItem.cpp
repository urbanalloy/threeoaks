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
#include "ArchiveItem.h"

void CArchiveItem::Init(ArchiveItem archiveItem)
{
	this->item = archiveItem;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CArchiveItem::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IArchiveItem
	};

	for (unsigned int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISystemEx
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CArchiveItem::get_Name(BSTR* name)
{
	*name = item.filename.copy();

	return S_OK;
}

STDMETHODIMP CArchiveItem::get_IsDirectory(VARIANT_BOOL *isDirectory)
{
	if (item.isDirectory)
		*isDirectory = VARIANT_TRUE;
	else
		*isDirectory = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CArchiveItem::get_CompressedSize(long* compressedSize)
{
	*compressedSize = item.compressedSize;

	return S_OK;
}