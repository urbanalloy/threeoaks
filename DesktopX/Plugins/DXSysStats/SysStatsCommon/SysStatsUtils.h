/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SYSSTATSUTILS_H
#define SYSSTATSUTILS_H
#pragma once

#define DUMMY_IUNKNOWN \
virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) { return S_OK; } \
virtual ULONG   __stdcall AddRef() { return 0; } \
virtual ULONG   __stdcall Release() { return 0; }

#define DUMMY_IDISPATCH \
virtual long __stdcall GetTypeInfoCount(unsigned int *) { return 0; } \
virtual long __stdcall GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** ) { return 0; } \
virtual long __stdcall GetIDsOfNames(const struct _GUID &,unsigned short ** ,unsigned int,unsigned long,long *) { return 0; } \
virtual long __stdcall Invoke(long,const struct _GUID &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *) { return 0; }

#define DUMMY_DUAL \
	DUMMY_IUNKNOWN \
	DUMMY_IDISPATCH

#include <Windows.h>

class _bstr_t;

namespace SysStatsUtils
{
	struct CriticalSection
	{
		CriticalSection() { ::InitializeCriticalSection(&cs); }
		~CriticalSection() { ::DeleteCriticalSection(&cs); }

		CRITICAL_SECTION cs;
	};

	struct Lock
	{
		Lock(CriticalSection* _cs) : m_cs(_cs) { if (m_cs) ::EnterCriticalSection(&m_cs->cs); }
		Lock(CriticalSection& _cs) : m_cs(&_cs) { if (m_cs) ::EnterCriticalSection(&m_cs->cs); }
		~Lock() { if (m_cs) ::LeaveCriticalSection(&m_cs->cs); }

	private:
		CriticalSection* m_cs;
	};

	void CompactHeaps();
	void SetLFHeaps();
	void DumpResources(HINSTANCE hModule);
	void checkError();
	HRESULT getUniqueFilename(const char *root, const char *postfix, _bstr_t *pRet);
	bool createFilePath(const char *path);
	long VariantToInteger(VARIANT *v);
	HRESULT VariantToInteger(VARIANT *v, long *pRet);
	HRESULT VariantToDouble(VARIANT *v, double *pRet);
	HRESULT VariantToBSTR(VARIANT *v, BSTR *pRet);
	HRESULT VariantToString(VARIANT *v, char *out, int len);
	HRESULT StringToVariant(char *in, VARTYPE vt, VARIANT *v);
	HRESULT FreeVariant(const VARIANT &var);
	void CLSIDToName(const CLSID& clsid, LPCTSTR szFriendlyName, int iLength);
	void EncodeXMLString(const char *in, _bstr_t *appendTo);
};

#endif // SYSSTATSUTILS_H