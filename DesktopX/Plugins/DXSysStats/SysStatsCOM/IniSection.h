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

#include <functional> // for binary_function
#include <map>      // for std::map<K,V>
#include <string>   // for std::wstring
#include <cstring>  // for global operator==(wstring, wstring)
#include <iostream> // for std::istream, std::ostream

struct LessI 
{
  bool operator()(const std::wstring& lhs, const std::wstring& rhs) const
  {
	  return ::_wcsicmp(lhs.c_str(), rhs.c_str()) < 0;
  }
};

class CIniSection
{
protected:
	typedef std::map<std::wstring, std::wstring, LessI> values_map;
	typedef std::map<std::wstring, std::wstring, LessI>::const_iterator values_iterator;

	values_map m_Values;

public:
	std::wstring GetValue(const std::wstring& key, const std::wstring& defValue)const;
	void         SetValue(const std::wstring& key, const std::wstring& value);
	void         SetValue(const std::wstring& key, int value);
	void         SetValue(const std::wstring& key, unsigned int value);
	void         SetValue(const std::wstring& key, long value);
	void         SetValue(const std::wstring& key, double value);

	std::wstring GetString(const std::wstring& key, const std::wstring& defValue)const;
	size_t       GetString(const std::wstring& key, wchar_t* buffer, size_t bufferLength, const std::wstring& defValue)const;
	BSTR         GetBSTR(const std::wstring& key, const std::wstring& defValue)const;
	int          GetInt(const std::wstring& key, int defValue)const;
	unsigned int GetUnsignedInt(const std::wstring& key, unsigned int defValue)const;
	long         GetLong(const std::wstring& key, long defValue)const;
	double       GetDouble(const std::wstring& key, double defValue)const;

	bool         KeyExists(const std::wstring& key)const;

	void         EmptySection();

	CIniSection();
	~CIniSection();

	void Save(std::wostream& st)const;
	void Read(std::wistream& st);
};
