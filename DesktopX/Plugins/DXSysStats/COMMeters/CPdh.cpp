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

#include "stdafx.h"
#include <stdio.h>
#include <comdef.h>
#include "CPdh.h"
#include <map>
#include <string>

#ifdef notdef
BOOL GetNameStrings( );
#endif

CPdh::CPdh() : hCounters(NULL), nCounters(0), hQuery(NULL)
{
#ifdef notdef
	GetNameStrings();
	PDH_STATUS ret = PdhOpenQuery(0,0,&hQuery);
	if (ret != ERROR_SUCCESS)
	{
		ret = PdhOpenQuery("",0,&hQuery);
	}
#endif
}

CPdh::~CPdh()
{
	ClearCounters();
//	PdhCloseQuery(hQuery);
}

CPdh::CPdh(const CPdh&)
{
}

#ifdef notdef
static BOOL GetNameStrings( )
{
    HKEY hKeyPerflib009;   // handle to registry key
    DWORD dwMaxValueLen;   // maximum size of key values
    DWORD dwBuffer;        // bytes to allocate for buffers
    LPSTR lpCurrentString; // pointer for enumerating data strings
	LPSTR lpNameStrings;
    DWORD dwCounter;       // current counter index

	// Open the key containing the counter and object names.
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\009",
		0,
		KEY_READ,
		&hKeyPerflib009) != ERROR_SUCCESS
	)
        return FALSE;

	// Get the size of the largest value in the key (Counter or Help).
	if( RegQueryInfoKey( hKeyPerflib009,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&dwMaxValueLen,
		NULL,
		NULL) != ERROR_SUCCESS
	)
		return FALSE;

// Allocate memory for the counter and object names.
	dwBuffer = dwMaxValueLen + 1;
	lpNameStrings = (char*)malloc( dwBuffer * sizeof(CHAR) );
	if (lpNameStrings == NULL)
		return FALSE;

	// Read the counter value.
	if( RegQueryValueEx( hKeyPerflib009,
		"Counters",
		NULL,
		NULL,
		(LPBYTE)lpNameStrings, &dwBuffer ) != ERROR_SUCCESS
	)
		return FALSE;

	// Load maps.
	std::map<std::string, long> *nineToIndex = new std::map<std::string, long>();
	std::map<long, std::string> *indexToNine = new std::map<long, std::string>();

    for( lpCurrentString = lpNameStrings; *lpCurrentString;
         lpCurrentString += (lstrlen(lpCurrentString)+1) )
    {
        dwCounter = atol( lpCurrentString );
        lpCurrentString += (lstrlen(lpCurrentString)+1);
		std::map <std::string, long>::const_iterator it;
		it = nineToIndex->find(lpCurrentString);
		if (it != nineToIndex->end())
			AtlTrace("Key %s is a duplicate\n", (char*)_bstr_t(lpCurrentString));
        (*nineToIndex)[lpCurrentString] = dwCounter;
        (*indexToNine)[dwCounter] = lpCurrentString;
    }

	free (lpNameStrings);
	delete nineToIndex;
	delete indexToNine;

	RegCloseKey( hKeyPerflib009 );

	return TRUE;
}
#endif

PDH_STATUS CPdh::ClearCounters()
{
	PDH_STATUS pdhStatus = ERROR_SUCCESS;
	if (hQuery)
	{
		::PdhCloseQuery(hQuery);
		hQuery = NULL;
	}

	if (hCounters)
	{
#ifdef notdef
		for (int i=0; i<nCounters; i++)
		{
			pdhStatus = ::PdhRemoveCounter(hCounters[i]);
		}
#endif
		::GlobalFree(hCounters);
		hCounters = NULL;
	}

	nCounters = 0;

	return pdhStatus;
}

PDH_STATUS CPdh::SetCounters(char *szFullCounterPath[], int count)
{
	PDH_STATUS pdhStatus;
	pdhStatus = ClearCounters();

	pdhStatus = PdhOpenQuery(0,0,&hQuery);
	if (pdhStatus != ERROR_SUCCESS)
	{
		pdhStatus = PdhOpenQuery("",0,&hQuery);
	}

	hCounters = (HCOUNTER *)GlobalAlloc(GPTR, (sizeof(HCOUNTER) * count));
	nCounters = count;

	for (int i=0; i < count; i++)
	{
		pdhStatus = PdhAddCounter(hQuery,szFullCounterPath[i],0,&hCounters[i]);
	}

	// must prime the pump?
	return Collect();
}
   
PDH_STATUS CPdh::Collect() const 
{
	PDH_STATUS ret = PdhCollectQueryData(hQuery); 
	return ret;
}

int CPdh::getNumCounters() const
{
	return nCounters;
}

PDH_STATUS CPdh::GetFormatted(int counter, DWORD fmt, PDH_FMT_COUNTERVALUE* fmtValue) const
{
	PDH_STATUS pdhStatus = PDH_INVALID_HANDLE;
	DWORD  ctrType;
	if (counter < nCounters)
	{
		pdhStatus = PdhGetFormattedCounterValue(hCounters[counter], fmt, &ctrType, fmtValue);
	}

	return pdhStatus;
}
   
double CPdh::asDouble(int counter) const
{
	PDH_FMT_COUNTERVALUE   fmtValue;
	PDH_STATUS pdhStatus = GetFormatted(counter, PDH_FMT_DOUBLE, &fmtValue);
	if (pdhStatus != ERROR_SUCCESS) 
		return -1.0;

	return fmtValue.doubleValue ;
}

long CPdh::asLong(int counter) const 
{
	PDH_FMT_COUNTERVALUE   fmtValue;
	PDH_STATUS pdhStatus = GetFormatted(counter, PDH_FMT_LONG, &fmtValue);
	if (pdhStatus != ERROR_SUCCESS) 
		return -1;

	return fmtValue.longValue;
}

PDH_STATUS CPdh::getCounterName(int counter, char *outBuf) const
{
	if (counter < nCounters)
	{
		PDH_COUNTER_INFO *pBuf;
		DWORD bufSize = 0;
		PDH_STATUS ret = PdhGetCounterInfo(hCounters[counter], FALSE, &bufSize, 0);
		if (ret == PDH_MORE_DATA || ret == ERROR_SUCCESS)
		{
			pBuf = (PDH_COUNTER_INFO*)malloc(bufSize);
			memset(pBuf,0,bufSize);
			ret = PdhGetCounterInfo(hCounters[counter], FALSE, &bufSize, pBuf);
			if (ret == ERROR_SUCCESS)
			{
				strcpy(outBuf, pBuf->szFullPath);
			}

			free(pBuf);
			return ret;
		}
	}

	return NULL;
}

PDH_STATUS CPdh::getHostName(int counter, char *outBuf) const
{
	if (counter < nCounters)
	{
		PDH_COUNTER_INFO *pBuf;
		DWORD bufSize = 0;
		PDH_STATUS ret = PdhGetCounterInfo(hCounters[counter], FALSE, &bufSize, 0);
		if (ret == PDH_MORE_DATA || ret == ERROR_SUCCESS)
		{
			pBuf = (PDH_COUNTER_INFO*)malloc(bufSize);
			memset(pBuf,0,bufSize);
			ret = PdhGetCounterInfo(hCounters[counter], FALSE, &bufSize, pBuf);
			if (ret == ERROR_SUCCESS)
			{
				strcpy(outBuf, pBuf->szMachineName);
			}

			free(pBuf);
			return ret;
		}
	}

	return NULL;
}