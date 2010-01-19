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

#include <map>      // for std::map<K,V>
#include <string>   // for wstring
#include <cstring>  // for global operator==(wstring, wstring)
#include <iostream> // for std::wistream, std::wostream

#include "./IniSection.h" // for CIniSection

class CIniFile
{
protected:
	typedef std::map<std::wstring, CIniSection> sections_map;
	typedef std::map<std::wstring, CIniSection>::const_iterator sections_iterator;
	sections_map m_Sections;

	std::wstring m_Filename;
	bool         m_HasChanged;

public:
	// Constructs a CIniFile not binded to a file
	CIniFile();

	// Constructs a CIniFile binded to fileName
	CIniFile(const std::wstring& fileName);

	// D`tor
	~CIniFile();

	// Gets the value in sectionName, for key if available, otherwise defValue is returned
	std::wstring GetValue(const std::wstring& sectionName, const std::wstring& key, const std::wstring& defValue)const;

	// Sets the string value in sectionName, for key.
	void         SetValue(const std::wstring& sectionName, const std::wstring& key, const std::wstring& value);
	// Sets the integer value in sectionName, for key.
	void         SetValue(const std::wstring& sectionName, const std::wstring& key, int value);
	// Sets the unsigned integer value in sectionName, for key.
	void         SetValue(const std::wstring& sectionName, const std::wstring& key, unsigned int);
	// Sets the long value in sectionName, for key.
	void         SetValue(const std::wstring& sectionName, const std::wstring& key, long);
	// Sets the unsigned integer value in sectionName, for key.
	void         SetValue(const std::wstring& sectionName, const std::wstring& key, double);

	// Gets the string in sectionName, for key if available, otherwise defValue is returned
	std::wstring GetString(const std::wstring& sectionName, const std::wstring& key, const std::wstring& defValue)const;
	// Gets the string in sectionName as a BSTR, for key if available, otherwise defValue is returned
	BSTR GetBSTR(const std::wstring& sectionName, const std::wstring& key, const std::wstring& defValue)const;
	// Gets the strings in sectionName for key if available and copies it into buffer, otherwise defValue is copied into the buffer.
	// The number of characters copied (not including the zero-terminator) is returned. If buffer is NULL, the number of
	// characters (not including the zero-terminator) is returned.
	size_t       GetString(const std::wstring& sectionName, const std::wstring& key, wchar_t* buffer, size_t bufferLength, const std::wstring& defValue)const;
	// Gets the integer value in sectionName for key, if available. Otherwise, defValue is returned.
	int          GetInt(const std::wstring& sectionName, const std::wstring& key, int defValue)const;
	// Gets the unsigned integer value in sectionName for key, if available. Otherwise, defValue is returned.
	unsigned int GetUnsignedInt(const std::wstring& sectionName, const std::wstring& key, unsigned int defValue)const;
	// Gets the long value in sectionName for key, if available. Otherwise, defValue is returned.
	long GetLong(const std::wstring& sectionName, const std::wstring& key, long defValue)const;
	// Gets the unsigned integer value in sectionName for key, if available. Otherwise, defValue is returned.
	double GetDouble(const std::wstring& sectionName, const std::wstring& key, double defValue)const;

	// Checks if a section with sectionName exists.
	bool SectionExists(const std::wstring& sectionName)const;

	// Empties the ini-file (all sections and their data will be deleted)
	void Empty();

	// Saves the CIniFile to a the file it was constructed from.
	bool Save();
	// Saves the CIniFile to filename.
	bool Save(const std::wstring& filename)const;
	// Saves the CIniFile to a C++ output stream.
	bool SaveToStream(std::wostream& st)const;

	// Reads the CIniFile from a file.
	bool Read(const std::wstring& fileName);
	// Reads the CIniFile from a C++ input stream.
	bool ReadFromStream(std::wistream& st);

	// Checks if the CIniFile has changed.
	bool GetHasChanged()const;
	// Sets if the CIniFile has changed.
	void SetHasChanged(bool val);
};
