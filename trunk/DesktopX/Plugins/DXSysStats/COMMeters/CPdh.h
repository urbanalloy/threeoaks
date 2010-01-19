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

#pragma once

#include "Pdh.h"
#include "PdhMsg.h"

#pragma comment(lib, "pdh.lib")

class CPdh
{
public:
	CPdh();
#ifdef notdef
	CPdh(char *szFullCounterPath[], int count);
#endif
	virtual ~CPdh();

	PDH_STATUS ClearCounters();
	PDH_STATUS SetCounters(char *szFullCounterPath[], int count);
	PDH_STATUS Collect() const;
	int getNumCounters() const;

	PDH_STATUS GetFormatted(int counter, DWORD fmt, PDH_FMT_COUNTERVALUE* fmtValue) const;
	double asDouble(int counter) const;
	long asLong(int counter) const;
	PDH_STATUS getCounterName(int i, char *outBuf) const;
	PDH_STATUS getHostName(int i, char *outBuf) const;

private:
	CPdh(const CPdh&);

	HQUERY hQuery;
	HCOUNTER *hCounters;
	int nCounters;
};