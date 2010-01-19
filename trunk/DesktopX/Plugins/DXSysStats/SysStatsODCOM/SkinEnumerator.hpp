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

// HEADER FILE

#ifndef CSKINENUMERATOR_HPP_20_45_33
#define CSKINENUMERATOR_HPP_20_45_33

#include <windows.h>
#include <comutil.h>
#include <map>
#include <vector>

struct CSkinValue
{
	_bstr_t filename;
	_bstr_t name;
	_bstr_t author;
	_bstr_t version;
	_bstr_t description;

	CSkinValue() :
		filename(""),
		name(""),
		author(""),
		version(""),
		description("")
	{
	}

	CSkinValue(const CSkinValue& other) :
		filename(other.filename),
		name(other.name),
		author(other.author),
		version(other.version),
		description(other.description)
	{
	}
};

struct BstrCmp
{
  bool operator()(const _bstr_t &lhs, const _bstr_t &rhs) const
  {
	  return ::_wcsicmp(lhs, rhs) < 0;
  }
};

class CSkinEnumerator
{
private:
	std::map<_bstr_t, CSkinValue, BstrCmp> m_Skins;

public:
	CSkinEnumerator(HINSTANCE hInstance, const WCHAR* deskletPath);
	UINT size() const;
	const _bstr_t& getKey(UINT num) const;
	const CSkinValue& getValue(const _bstr_t& key) const;
	const CSkinValue& getValue(UINT num) const;
};

#endif//CSKINENUMERATOR_HPP_20_45_33
