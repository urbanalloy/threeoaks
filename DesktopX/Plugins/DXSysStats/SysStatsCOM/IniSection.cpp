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
#include "inisection.h"
#include "help_funcs.h" // for toString<T>(), fromString<T>()

#include <algorithm>      // for std::copy()
#include <iterator>       // for std::insert_iterator<T>

CIniSection::CIniSection()
{

}

CIniSection::~CIniSection()
{
}

std::wstring CIniSection::GetValue(const std::wstring& key, const std::wstring& defValue)const
{
	std::wstring value;

	values_iterator iter = m_Values.find(key);
	if(iter != m_Values.end())
		value = iter->second;
	else
		value = defValue;

	return value;
}

void CIniSection::SetValue(const std::wstring& key, const std::wstring& value)
{
	m_Values[key]  = value;
}

void CIniSection::SetValue(const std::wstring& key, int value)
{
	m_Values[key] = toString(value);
}

void CIniSection::SetValue(const std::wstring& key, unsigned int value)
{
	m_Values[key] = toString(value);
}

void CIniSection::SetValue(const std::wstring& key, long value)
{
	m_Values[key] = toString(value);
}

void CIniSection::SetValue(const std::wstring& key, double value)
{
	m_Values[key] = toString(value);
}

std::wstring CIniSection::GetString(const std::wstring& key, const std::wstring& defValue)const
{
	return GetValue(key, defValue);
}

size_t CIniSection::GetString(const std::wstring& key, wchar_t* buffer, size_t bufferLength, const std::wstring& defValue)const
{
	size_t charactersCopied(0);
	std::wstring value = GetValue(key, defValue);

	if(NULL == buffer)
	{
		charactersCopied =  value.length();
	}
	else
	{
		std::wstring::iterator iter = value.begin();
		charactersCopied = 0;
		while(charactersCopied++ < bufferLength - 1)
			*buffer++ = *iter++;

		*buffer = '\0';
	}

	return charactersCopied;
}

BSTR CIniSection::GetBSTR(const std::wstring& key, const std::wstring& defValue)const
{
	std::wstring value = GetValue(key, defValue);
	return ::SysAllocString(value.c_str());
}

int CIniSection::GetInt(const std::wstring& key, int defValue)const
{
	int value(defValue);

	values_iterator iter = m_Values.find(key);
	if(iter != m_Values.end())
		value = fromString(iter->second, defValue);

	return value;
}

unsigned int CIniSection::GetUnsignedInt(const std::wstring& key, unsigned int defValue)const
{
	unsigned int value(defValue);

	values_iterator iter = m_Values.find(key);
	if(iter != m_Values.end())
		value = fromString(iter->second, defValue);

	return value;	
}

long CIniSection::GetLong(const std::wstring& key, long defValue)const
{
	long value(defValue);

	values_iterator iter = m_Values.find(key);
	if(iter != m_Values.end())
		value = fromString(iter->second, defValue);

	return value;	
}

double CIniSection::GetDouble(const std::wstring& key, double defValue)const
{
	double value(defValue);

	values_iterator iter = m_Values.find(key);
	if(iter != m_Values.end())
		value = fromString(iter->second, defValue);

	return value;	
}

bool CIniSection::KeyExists(const std::wstring& key)const
{
	values_iterator iter = m_Values.find(key);

	return iter != m_Values.end();
}

void CIniSection::EmptySection()
{
	m_Values.clear();
}

void CIniSection::Save(std::wostream& st)const
{
	for(values_iterator iter = m_Values.begin(); iter != m_Values.end(); ++iter)
		st << iter->first << L"=" << iter->second << std::endl;
}

void CIniSection::Read(std::wistream& st)
{
	const wchar_t SPACE = L' ';
	const wchar_t SECTION_NAME_START = L'[';
	const wchar_t EQUAL_SIGN = L'=';

	bool atEndSection = false;
	while(!atEndSection)
	{
		// skip whitespaces
		bool isSpace;
		do
		{
			int read = st.peek();
			wchar_t ch = static_cast<wchar_t>(read);

			isSpace = read != EOF && ch == SPACE;
			if(isSpace)
				st.get();

		}while(isSpace);

		int next = st.peek();
		atEndSection = !st || next == EOF || static_cast<wchar_t>(next) == SECTION_NAME_START;
		if(!atEndSection)
		{
			std::wstring line;
			std::getline(st, line);

			std::wstring::size_type equalPos = line.find_first_of(EQUAL_SIGN);
			if(equalPos != std::wstring::npos)
			{
				std::wstring key, value;

				std::copy(line.begin(), line.begin() + equalPos, std::insert_iterator<std::wstring>(key, key.begin()));
				std::copy(line.begin() + equalPos + 1, line.end(), std::insert_iterator<std::wstring>(value, value.begin()));

				m_Values[key] = value;
			}
		}
	}
}