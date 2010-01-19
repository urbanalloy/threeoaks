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

#include "StdAfx.h"
#include ".\inifile.h"
#include <windows.h>     // for WideCharToMultiByte
#include <fstream>       // for std::wifstream, std::wostream

#include <algorithm>     // for std::copy()
#include <iterator>      // for std::insert_iterator<T>

CIniFile::CIniFile() :
	m_HasChanged(false)
{
	
}

CIniFile::CIniFile(const std::wstring& fileName) :
	m_HasChanged(false)
{
	Read(fileName);
}

CIniFile::~CIniFile()
{
	if(m_Filename.length() > 0 && GetHasChanged())
		Save();
}

bool CIniFile::GetHasChanged()const
{
	return m_HasChanged;
}

void CIniFile::SetHasChanged(bool val)
{
	m_HasChanged = val;
}

std::wstring CIniFile::GetValue(const std::wstring& sectionName, const std::wstring& key, const std::wstring& defValue)const
{
	std::wstring retVal;

	if(!SectionExists(sectionName))
		retVal = defValue;
	else
		retVal = m_Sections.find(sectionName)->second.GetValue(key, defValue);

	return retVal;
}	

void CIniFile::SetValue(const std::wstring& sectionName, const std::wstring& key, const std::wstring& value)
{
	SetHasChanged(true);
	CIniSection& section = m_Sections[sectionName];
	section.SetValue(key, value);
}

void CIniFile::SetValue(const std::wstring& sectionName, const std::wstring& key, int value)
{
	SetHasChanged(true);
	CIniSection& section = m_Sections[sectionName];
	section.SetValue(key, value);
}

void CIniFile::SetValue(const std::wstring& sectionName, const std::wstring& key, unsigned int value)
{
	SetHasChanged(true);
	CIniSection& section = m_Sections[sectionName];
	section.SetValue(key, value);
}

void CIniFile::SetValue(const std::wstring& sectionName, const std::wstring& key, long value)
{
	SetHasChanged(true);
	CIniSection& section = m_Sections[sectionName];
	section.SetValue(key, value);
}

void CIniFile::SetValue(const std::wstring& sectionName, const std::wstring& key, double value)
{
	SetHasChanged(true);
	CIniSection& section = m_Sections[sectionName];
	section.SetValue(key, value);
}

std::wstring CIniFile::GetString(const std::wstring& sectionName, const std::wstring& key, const std::wstring& defValue)const
{
	std::wstring retVal;

	if(!SectionExists(sectionName))
		retVal = defValue;
	else
		retVal = m_Sections.find(sectionName)->second.GetString(key, defValue);

	return retVal;
}

BSTR CIniFile::GetBSTR(const std::wstring& sectionName, const std::wstring& key, const std::wstring& defValue)const
{
	BSTR retVal;

	if(!SectionExists(sectionName))
		retVal = ::SysAllocString(defValue.c_str());
	else
		retVal = m_Sections.find(sectionName)->second.GetBSTR(key, defValue);

	return retVal;
}

size_t CIniFile::GetString(const std::wstring& sectionName, const std::wstring& key, wchar_t* buffer, size_t bufferLength, const std::wstring& defValue)const
{
	size_t retVal;

	if(!SectionExists(sectionName))
		retVal = 0;
	else
		retVal = m_Sections.find(sectionName)->second.GetString(key, buffer, bufferLength, defValue);

	return retVal;
}

int CIniFile::GetInt(const std::wstring& sectionName, const std::wstring& key, int defValue)const
{
	int retVal;

	if(!SectionExists(sectionName))
		retVal = defValue;
	else
		retVal = m_Sections.find(sectionName)->second.GetInt(key, defValue);

	return retVal;
}

unsigned int CIniFile::GetUnsignedInt(const std::wstring& sectionName, const std::wstring& key, unsigned int defValue)const
{
	unsigned int retVal;

	if(!SectionExists(sectionName))
		retVal = defValue;
	else
		retVal = m_Sections.find(sectionName)->second.GetUnsignedInt(key, defValue);

	return retVal;
}

long CIniFile::GetLong(const std::wstring& sectionName, const std::wstring& key, long defValue)const
{
	long retVal;

	if(!SectionExists(sectionName))
		retVal = defValue;
	else
		retVal = m_Sections.find(sectionName)->second.GetLong(key, defValue);

	return retVal;
}

double CIniFile::GetDouble(const std::wstring& sectionName, const std::wstring& key, double defValue)const
{
	unsigned int retVal;

	if(!SectionExists(sectionName))
		retVal = defValue;
	else
		retVal = m_Sections.find(sectionName)->second.GetDouble(key, defValue);

	return retVal;
}

bool CIniFile::SectionExists(const std::wstring& sectionName)const
{
	return m_Sections.find(sectionName) != m_Sections.end();
}

void CIniFile::Empty()
{
	SetHasChanged(true);
	m_Sections.clear();
}

bool CIniFile::Save()
{
	bool successfull = Save(m_Filename);
	if(successfull)
		SetHasChanged(false);

	return successfull;
}

bool CIniFile::SaveToStream(std::wostream& st)const
{
	bool successfull(true);
	try
	{
		for(sections_iterator iter = m_Sections.begin(); iter != m_Sections.end(); ++iter)
		{
			st << L"[" << iter->first << L"]" << std::endl;
			iter->second.Save(st);
			st << std::endl;
		}
	}
	catch(...)
	{
		successfull = false;
	}

	return successfull;
}


bool CIniFile::Save(const std::wstring& filename)const
{
	bool successfull(true);
	try
	{
		// --start of windows specific code
		// better than using C++ codept conversion functions, since
		// WCTMB maps to better to the system local codepage.
		char cFilename[MAX_PATH] = {0};
		WideCharToMultiByte(CP_OEMCP, 0, filename.c_str(), -1, cFilename, MAX_PATH, NULL, NULL);
		// --end of windows specific code

		std::wofstream *pFile = new std::wofstream(cFilename);
		if(*pFile)
		{
			successfull = SaveToStream(*pFile);
			pFile->close();
		}
		else
		{
			successfull = false;
		}

		delete pFile;
	}
	catch(...)
	{
		successfull = false;
	}

	return successfull;
}

bool CIniFile::Read(const std::wstring& filename)
{
	bool successfull(true);
	try
	{
		Empty();
		SetHasChanged(false);
		m_Filename = filename;

		// --start of windows specific code
		// better than using C++ codept conversion functions, since
		// WCTMB maps to better to the system local codepage.
		char cFilename[MAX_PATH] = {0};
		WideCharToMultiByte(CP_OEMCP, 0, filename.c_str(), -1, cFilename, MAX_PATH, NULL, NULL);
		// --end of windows specific code

		std::wifstream *pFile = new std::wifstream(cFilename);
		if(*pFile)
		{
			successfull = ReadFromStream(*pFile);
			pFile->close();
		}

		delete pFile;
	}
	catch(...)
	{
		successfull = false;
	}

	return successfull;
}

bool CIniFile::ReadFromStream(std::wistream& stream)
{
	bool successfull(true);
	try
	{
		const wchar_t SPACE = L' ';
		const wchar_t SECTION_NAME_START = L'[';
		const wchar_t SECTION_NAME_END   = L']';

		std::wstring line;
		while(std::getline(stream, line))
		{
			std::wstring::size_type begin = line.find_first_not_of(SPACE);
			if(begin != std::wstring::npos && line.at(begin) == SECTION_NAME_START)
			{
				std::wstring sectionName;
				std::wstring::size_type  end = line.find_last_of(SECTION_NAME_END);
				std::copy(line.begin() + begin + 1, line.begin() + end, std::insert_iterator<std::wstring>(sectionName, sectionName.begin()));

				m_Sections[sectionName].Read(stream);
			}
		}
	}
	catch(...)
	{
		successfull = false;
	}

	return successfull;
}