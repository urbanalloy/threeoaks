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

template <class K, class V>
class DictionaryPtr;

template <class K, class V>
class AutoDictionaryPtr : public DictionaryPtr<K, V>
{
public:
	AutoDictionaryPtr(bool (*pEq)(const K* lhs, const K* rhs)) : DictionaryPtr<K,V>(pEq)
	{
	}

	~AutoDictionaryPtr()
	{
		for (int i=0; i<pTable->count(); i++)
		{
			Entry *e = (*pTable)[i];
			delete e->key;
			delete e->value;
		}
		clearAndDestroy();
	}

	void remove(const K* key)
	{
		int i = getIndex(key);

		if (i >= 0)
		{
			Entry *e = (*pTable)[i];
			delete e->key;
			delete e->value;
			delete e;
			pTable->remove(i);
		}
	}
};

template<class I, const GUID* G, const CATID* C>
class CategoryFactoryT
{
public:
	static I* createInstance(const char* type, IClientModel *model);
	static int typeCount();
	static void listTypes(char** outArray);
	static void getDisplayName(const CLSID& clsid, LPCTSTR szDisplayName, int iLength);
	static void getFriendlyName(const CLSID& clsid, LPCTSTR szFriendlyName, int iLength);
	static DWORD getVersion(const CLSID& clsid);
	static AutoDictionaryPtr<char, CLSID>* getRegistry();

protected:
	static const GUID *interfaceGUID;
	static const CATID *categoryID;
	static SysStatsUtils::CriticalSection createCS;

private:
	static void populateRegistry(AutoDictionaryPtr<char, CLSID>*);
};