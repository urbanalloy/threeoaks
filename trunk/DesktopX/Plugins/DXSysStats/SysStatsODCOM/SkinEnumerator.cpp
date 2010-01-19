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
#include "SkinEnumerator.hpp"
#include <comutil.h>
#pragma comment(lib, "Shlwapi.lib")

//  IMPLEMENTATION FILE
CSkinEnumerator::CSkinEnumerator(HINSTANCE hInstance, const wchar_t* deskletPath)
{
	char path[MAX_PATH+1] = {0};
	_bstr_t _path_t(deskletPath);
	strcpy(path, _path_t);
	PathAppend(path,"configs\\"); // search in the configs directory for skins/configs.
	_bstr_t s = path;
	s += "*.ini";					// only make INI files show up in our search

	WIN32_FIND_DATA data = {0};
	HANDLE h = FindFirstFile(s, &data);
	if(h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(strcmp(data.cFileName,".") != 0 && strcmp(data.cFileName,"..") != 0)
			{
				char file[MAX_PATH+1] = {0};
				strcpy(file, path);
				PathAppend(file, data.cFileName);

				CSkinValue value;
				value.filename = file;

				::PathRemoveExtension(data.cFileName);

				char buf[1024];
				::GetPrivateProfileString("General", "name", data.cFileName, buf, sizeof(buf), file);
				if (*buf == 0)
				{
					// name property was left blank
					strncpy(buf, data.cFileName, sizeof(buf));
				}
				value.name = buf;

				::GetPrivateProfileString("General", "author", "", buf, sizeof(buf), file);
				value.author = buf;

				::GetPrivateProfileString("General", "dversion", "", buf, sizeof(buf), file);
				value.version = buf;

				::GetPrivateProfileString("General", "description", "", buf, sizeof(buf), file);
				value.description = buf;

				m_Skins[_bstr_t(data.cFileName)] = value;
			}
		} while (FindNextFile(h,&data));
		FindClose(h);
	}
}

size_t CSkinEnumerator::size() const
{
	return m_Skins.size();
}

const _bstr_t& CSkinEnumerator::getKey(UINT num) const
{
	std::map<_bstr_t, CSkinValue, BstrCmp>::const_iterator iter = m_Skins.begin();
	std::advance(iter,num);
	return iter->first;
}

const CSkinValue& CSkinEnumerator::getValue(const _bstr_t& key) const
{
	std::map<_bstr_t, CSkinValue, BstrCmp>::const_iterator iter = m_Skins.find(key);
	return iter->second;
}


const CSkinValue& CSkinEnumerator::getValue(UINT num) const
{
	std::map<_bstr_t, CSkinValue, BstrCmp>::const_iterator iter = m_Skins.begin();
	std::advance(iter,num);
	return iter->second;
}
